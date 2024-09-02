#pragma once

#include <vector>
#include <stdexcept>
#include <iostream>
#include "methods.hpp"
#include "types.hpp"

namespace amulet::raw{
    enum SOCKET_TYPE {
        SOCK_SERV,
        // SOCK_ACCCLI,
        SOCK_CLI
    };

    enum ERROR_CODE {
        INVALID_TYPE,
        METHOD_FAILURE,
        RAW_METHOD_FAILURE,
        SUCCESS
    };

    std::string getStringError(ERROR_CODE type){
        switch(type){
            case INVALID_TYPE:
                return "Invalid socket type";
            case METHOD_FAILURE:
                return "Method failed";
            case RAW_METHOD_FAILURE:
                return "Raw method failed";
            case SUCCESS:
                return "Success";
            default:
                return "Unknown error";
        }
    }

    struct raw_data{
        int socket_fd;
        int addrlen;
        struct sockaddr_in address;
    };

    class Socket {
            Address iAddress;
            SOCKET_TYPE type;

            raw_data rdata;
        public:

            raw_data &data(){
                return rdata;
            }

            Address &address(){
                return iAddress;
            }

            ERROR_CODE bind(bool retry = true){
                if (type != SOCK_SERV) return INVALID_TYPE;

                int status = raw::__bind(
                    rdata.socket_fd,
                    (struct sockaddr *)&rdata.address,
                    rdata.addrlen
                );

                if(!retry && status != 0){
                    std::cout << "closing" << std::endl;
                    close();
                }

                while (retry && status != 0){
                    std::cout << "Retrying...\n";
                    close();
                    rdata.socket_fd = socket(AF_INET, SOCK_STREAM, 0);
                    status = raw::__bind(
                        rdata.socket_fd,
                        (struct sockaddr *)&rdata.address,
                        rdata.addrlen
                    );
                    sleep(3);
                }

                std::cout << "Binded\n";

                return (status == 0 ? SUCCESS: RAW_METHOD_FAILURE);
            }

            ERROR_CODE listen(unsigned clients){
                if (type != SOCK_SERV) return INVALID_TYPE;

                int status = raw::__listen(
                    rdata.socket_fd,
                    clients
                );

                return (status == 0? SUCCESS: RAW_METHOD_FAILURE);
            }

            ERROR_CODE accept(Socket &client){
                if (type != SOCK_SERV) return INVALID_TYPE;

                client.rdata.addrlen = sizeof(client.rdata.address);
                client.rdata.socket_fd = raw::__accept(
                    rdata.socket_fd,
                    (struct sockaddr *)&client.rdata.address,
                    (socklen_t*)&client.rdata.addrlen
                );

                if(client.rdata.socket_fd < 0) {
                    return RAW_METHOD_FAILURE;
                }

                client.type = SOCK_CLI;
                
                char buffer[INET_ADDRSTRLEN]; int port;
                inet_ntop(AF_INET, &client.rdata.address.sin_addr, buffer, INET_ADDRSTRLEN);
                port = ntohs(client.rdata.address.sin_port);
                client.iAddress = Address(buffer, port);

                return SUCCESS;
            }

            ERROR_CODE connect(){
                if (type != SOCK_CLI) return INVALID_TYPE;

                int status = raw::__connect(
                    rdata.socket_fd,
                    (struct sockaddr *)&rdata.address,
                    rdata.addrlen
                );

                return (status >= 0? SUCCESS: RAW_METHOD_FAILURE);
            }
            
            ERROR_CODE close(){
                if(rdata.socket_fd < 0) return METHOD_FAILURE;

                int status = raw::__close(rdata.socket_fd);

                if(status < 0) {
                    return RAW_METHOD_FAILURE;
                }

                return SUCCESS;
            }

            ERROR_CODE recv(std::string &response, Socket &client, unsigned maxReadSize = 1024){
                if(rdata.socket_fd < 0) return METHOD_FAILURE;
                if(type == SOCK_CLI) return INVALID_TYPE;
                
                std::vector<char> buffer(maxReadSize);
                
                ssize_t bytesReceived = read(
                    client.rdata.socket_fd,
                    buffer.data(),
                    buffer.size()
                );

                if(bytesReceived <= 0) {
                    return RAW_METHOD_FAILURE;
                }

                response.assign(buffer.begin(), buffer.begin() + bytesReceived);
                return SUCCESS;
            }

            ERROR_CODE send(std::string request, Socket &client){
                if(rdata.socket_fd < 0) return METHOD_FAILURE;
                if(type == SOCK_CLI) return INVALID_TYPE;

                int status = raw::__send(
                    client.rdata.socket_fd,
                    request.c_str(),
                    request.size(),
                    0
                );

                return (status >= 0? SUCCESS: RAW_METHOD_FAILURE);
            }

            ERROR_CODE recv(std::string &request, unsigned maxReadSize = 1024){
                if(rdata.socket_fd < 0) return METHOD_FAILURE;
                if(type == SOCK_SERV) return INVALID_TYPE;
                
                std::vector<char> buffer(maxReadSize);
                
                ssize_t bytesReceived = raw::__recv(
                    rdata.socket_fd,
                    buffer.data(),
                    buffer.size(),
                    0
                );

                if(bytesReceived <= 0) {
                    return RAW_METHOD_FAILURE;
                }

                request.assign(buffer.begin(), buffer.begin() + bytesReceived);
                return SUCCESS;
            }
            ERROR_CODE send(std::string response){
                if(rdata.socket_fd < 0) return METHOD_FAILURE;
                if(type == SOCK_SERV) return INVALID_TYPE;

                int status = raw::__send(
                    rdata.socket_fd,
                    response.c_str(),
                    response.size(),
                    0
                );

                return (status >= 0? SUCCESS: RAW_METHOD_FAILURE);
            }

            Socket(
                Address addr,
                SOCKET_TYPE type
            ): iAddress(addr), type(type) {
                
                rdata.addrlen = sizeof(rdata.address);
                rdata.address.sin_family = AF_INET;

                switch(this->type) {
                    case SOCK_SERV: {
                        rdata.address.sin_addr.s_addr = inet_addr(&iAddress);
                        rdata.address.sin_port = htons(iAddress);
                        break;
                    };

                    case SOCK_CLI: {
                        rdata.address.sin_port = htons(iAddress);
                        int status = inet_pton(AF_INET, &addr, &rdata.address.sin_addr);

                        if(status!= 1) {
                            throw std::runtime_error("Invalid address format");
                        }
                        break;
                    };

                    default: {
                        throw std::runtime_error("Invalid socket type (" + std::to_string(type) + ')');
                    }
                }

                rdata.socket_fd = socket(AF_INET, SOCK_STREAM, 0);

                if(rdata.socket_fd < 0) {
                    throw std::runtime_error("Failed to create socket");
                }
            }

            Socket(){}
            ~Socket(){}
    };
};