#pragma once

#include <map>
#include <thread>
#include <mutex>
#include <memory>
#include <functional>


#include <Amulet/Raw/socket.hpp>
#include <Amulet/Logging/logger.hpp>

#include <bits/refwrap.h>
#include "ClientData.hpp"

namespace amulet::main::servers {
    class AsyncServer {
            raw::Address address;
            raw::Socket socket;

            unsigned maxClients;
            unsigned maxReadSize;
            std::map<int, ClientData> clients;

            std::mutex clientMutex;
            logging::Logger logger;
        public:

            std::vector<std::thread> startup(unsigned maxClients){
                logger.newlayer("Startup");
                raw::ERROR_CODE status;
                this->maxClients = maxClients;

                logger.log(logging::CALLING, "Binding...");
                status = socket.bind();
                if(status != raw::SUCCESS){
                    logger.log(logging::FATAL, "Can not bind server to requested address: " + (std::string)address + ", because: " + raw::getStringError(status));
                    throw std::runtime_error("Can not bind server to requested address: " + (std::string)address + ", because: " + raw::getStringError(status));
                }
                logger.log(logging::INFO, "Success!");

                logger.log(logging::CALLING, "Start to listen clients (" + std::to_string(this->maxClients) + ')');
                status = socket.listen(this->maxClients);
                if(status != raw::SUCCESS){
                    throw std::runtime_error("Server can not start listening, because: " + raw::getStringError(status));
                }
                logger.log(logging::INFO, "Success!");

                std::vector<std::thread> output;
                
                logger.log(logging::INITED, "Start to accepting a clients");
                logger.newlayer("Acception cycle");
                for(int i = 0; i < this->maxClients; i++){
                    logger.log(logging::CALLING, "Ini-calling a new acception thread for a client #" + std::to_string(i));
                    output.push_back(std::thread([&](int cliind){
                        raw::Socket newsock;
                        raw::ERROR_CODE nstatus = socket.accept(newsock);

                        if(nstatus != raw::SUCCESS){
                            logger.log(logging::FATAL, "Client #" + std::to_string(cliind) + " can not be accepted, because: " + raw::getStringError(status));
                            throw std::runtime_error("Can not accept client connection (" + std::to_string(cliind) + "), because: " + raw::getStringError(nstatus));
                        }

                        clientMutex.lock();
                            ClientData data;
                            data.socket = newsock;
                            data.selfID = cliind;
                            clients[cliind] = data;
                        clientMutex.unlock();

                        if(onClientConnected){
                            onClientConnected(clients[cliind]);
                        }
                    }, i));
                    logger.log(logging::INFO, "Inited and call an acception thread for the client");
                }
                logger.poplayer();
                logger.poplayer();
                return output;
            }

            void fdisconnect(int ClientIndex){
                logger.newlayer("Force disconnect");
                if(clients.find(ClientIndex) == clients.end()){
                    logger.log(logging::WARNING, "Client " + std::to_string(ClientIndex) + " does not exist. May be it is already disconnected?");
                    logger.poplayer();
                    return;
                }

                if(onClientForcefullyDisconnected){
                    logger.log(logging::CALLING, "Calling callback [onClientForcefullyDisconnected]");
                    onClientForcefullyDisconnected(clients[ClientIndex]);
                } else {
                    logger.log(logging::WARNING, "No callback [onClientForcefullyDisconnected] set. Client " + std::to_string(ClientIndex) + " will be not procceded");
                }

                logger.log(logging::INFO, "Closing socket and cleaning client map");
                
                clientMutex.lock();
                    clients[ClientIndex].socket.close();
                    clients.erase(ClientIndex);
                clientMutex.unlock();

                logger.log(logging::INFO, "Client " + std::to_string(ClientIndex) + " is fully f-disconnected");
                logger.poplayer();
            }

            void cycle(int ClientIndex){
                logger.newlayer("Cycle");
                raw::ERROR_CODE status;
                std::string request;
                std::string response;

                logger.log(logging::CALLING, "Trying to receive a request from client #" + std::to_string(ClientIndex));
                status = socket.recv(request, clients[ClientIndex], maxReadSize);

                if(status != raw::SUCCESS){
                    logger.log(logging::WARNING, "Failed to receive a request, because: " + raw::getStringError(status));
                    disconnect(ClientIndex);
                    logger.poplayer();
                    return;
                }

                if(onClientMessage){
                    onClientMessage(clients[ClientIndex], request, response);
                }

                status = socket.send(response, clients[ClientIndex]);

                if(status != raw::SUCCESS){
                    logger.log(logging::WARNING, "Failed to send a response, because: " + raw::getStringError(status));
                    disconnect(ClientIndex);
                    logger.poplayer();
                    return;
                }
                logger.poplayer();
            }

            std::vector<std::thread> runiter(){
                logger.newlayer("Runiter");

                std::vector<std::thread> output;
                std::vector<int> clientKeys;
                
                logger.log(logging::INITED, "Starting for-cycle to get keys for all clients ");
                for(auto &client : clients){
                    clientKeys.push_back(client.first);
                }
                
                logger.log(logging::INITED, "Starting for-cycle to procceed for all clients");
                logger.newlayer("Cycle for-cycle");
                for(int key : clientKeys){
                    logger.log(logging::CALLING, "Ini-calling a new thread for 1 cycleing a client #" + std::to_string(key));
                    output.push_back(std::thread(
                        [&](int cli){
                            cycle(cli);
                        }, 
                        key
                    ));
                    logger.log(logging::INITED, "Inited and called a cycle-thread for the client");
                }
                logger.poplayer();
                logger.poplayer();

                return output;
            }

            void stop(){
                logger.newlayer("Stop");

                logger.log(logging::INFO, "Stopping all clients");
                std::vector<int> clientKeys;
                for(auto &client : clients){
                    clientKeys.push_back(client.first);
                }

                for(int key : clientKeys){
                    fdisconnect(key);
                }
                logger.log(logging::INFO, "Closing main socket");
                socket.close();
                logger.poplayer();
            }

            void startLogging(){
                logger.activate(true, false);
            }

            AsyncServer(
                raw::Address address,
                unsigned maxReadSize = 1024,
                bool logServer = false
            ): 
                address(address), 
                socket(address, raw::SOCK_SERV),
                maxReadSize(maxReadSize),
                logger(
                    logging::BOTH,
                    "./logs/async-server-log",
                    false,
                    false,
                    true,
                    logServer
                )
            {
                logger.log(logging::INITED, "Async server");
                logger.newlayer("Async server");
            }

            AsyncServer(){}
            ~AsyncServer(){
                logger.poplayer();
                logger.log(logging::INFO, "Server stopped");
                logger.deactivate();
            }

            private:
                std::function<void(ClientData&, const std::string&, std::string&)> onClientMessage;
                std::function<void(ClientData&)> onClientDisconnected;
                std::function<void(ClientData&)> onClientConnected;
                std::function<void(ClientData&)> onClientForcefullyDisconnected;

                void disconnect(int ClientIndex){
                    logger.newlayer("Disconnect");
                    if(clients.find(ClientIndex) == clients.end()){
                        logger.log(logging::WARNING, "Client " + std::to_string(ClientIndex) + " does not exist. May be it is already disconnected?");
                        logger.poplayer();
                        return;
                    }

                    if(onClientDisconnected){
                        logger.log(logging::CALLING, "Calling callback [onClientDisconnected]");
                        onClientDisconnected(clients[ClientIndex]);
                    } else {
                        logger.log(logging::WARNING, "No callback [onClientDisconnected] set. Client #" + std::to_string(ClientIndex) + " will be not procceded");
                    }
                    
                    logger.log(logging::INFO, "Closing socket and cleaning client map");
                    clients[ClientIndex].socket.close();
                    clients.erase(ClientIndex);
                    logger.log(logging::INFO, "Client " + std::to_string(ClientIndex) + " is fully disconnected");
                    logger.poplayer();
                }
            
            public:
                void callbackOnClientMessage(
                    std::function<void(ClientData&, const std::string&, std::string&)> onClientMessage
                ){ this->onClientMessage = onClientMessage; }

                void callbackOnClientDisconnected(
                    std::function<void(ClientData&)> onClientDisconnected
                ){ this->onClientDisconnected = onClientDisconnected; }

                void callbackOnClientConnected(
                    std::function<void(ClientData&)> onClientConnected
                ){ this->onClientConnected = onClientConnected; }

                void callbackOnClientForcefullyDisconnected(
                    std::function<void(ClientData&)> onClientForcefullyDisconnected
                ){ this->onClientForcefullyDisconnected = onClientForcefullyDisconnected;  }
    };
}