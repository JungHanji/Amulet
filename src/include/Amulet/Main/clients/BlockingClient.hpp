#pragma once

#include <Amulet/Raw/socket.hpp>
#include <Amulet/Logging/logger.hpp>

namespace amulet::main::clients{
    class BlockingClient {
            raw::Socket socket;
            raw::Address servAddress;
            
            logging::Logger logger;
        public:

            void connect(){
                logger.newlayer("Connect");
                logger.log(logging::CALLING, "Trying to connect...");
                auto status = socket.connect();

                if(status != raw::SUCCESS) {
                    logger.log(logging::FATAL, "Can not connect to the server (" + (std::string)servAddress + "), because: " + raw::getStringError(status));
                    throw std::runtime_error("Can not connect to the server (" + (std::string)servAddress + "), because: " + raw::getStringError(status));
                }

                logger.log(logging::INFO, "Success!");

                logger.poplayer();
            }

            void disconnect(){
                logger.newlayer("Disconnect");
                logger.log(logging::CALLING, "Disconnecting...");
                auto status = socket.close();

                if(status != raw::SUCCESS) {
                    logger.log(logging::ERROR, "Can not properlly disconnect from the server (" + (std::string)servAddress + "), because: " + raw::getStringError(status));
                } else {
                    logger.log(logging::INFO, "Success!");
                }
                logger.poplayer();
            }

            std::string recv(unsigned maxReadSize = 1024){
                logger.newlayer("Receieving");
                std::string response;
                logger.log(logging::CALLING, "Trying to receive data from the server...");
                auto status = socket.recv(response, maxReadSize);

                if(status != raw::SUCCESS) {
                    logger.log(logging::FATAL, "Can not receive data from the server (" + (std::string)servAddress + "), because: " + raw::getStringError(status));
                    throw std::runtime_error("Can not receive data from the server (" + (std::string)servAddress + "), because: " + raw::getStringError(status));
                }

                logger.log(logging::INFO, "Success!");                

                logger.poplayer();
                return response;
            }

            void send(const std::string &message){
                logger.newlayer("Sending");
                logger.log(logging::CALLING, "Trying to send data to the server...");
                auto status = socket.send(message);

                if(status != raw::SUCCESS) {
                    logger.log(logging::FATAL, "Can not send data to the server (" + (std::string)servAddress + "), because: " + raw::getStringError(status));
                    throw std::runtime_error("Can not send data to the server (" + (std::string)servAddress + "), because: " + raw::getStringError(status));
                }

                logger.log(logging::INFO, "Success!");
                logger.poplayer();
            }

            std::string cycle(const std::string &message, unsigned maxReadSize = 1024){
                logger.log(logging::INFO, "Performing cycle...");
                send(message);
                return recv(maxReadSize);
            }

            void startLogging(){
                logger.activate(true, false);
            }

            BlockingClient(
                raw::Address servAddress,
                bool startLogging = false
            ): 
                servAddress(servAddress), 
                socket(servAddress, raw::SOCK_CLI),
                logger(
                    logging::BOTH,
                    "./logs/blocking-client-log",
                    false,
                    false,
                    true,
                    startLogging
                )
            {
                logger.log(logging::INITED, "Blocking client");
                logger.newlayer("Blocking client");
            }

            BlockingClient(){}
            ~BlockingClient(){
                logger.poplayer();
                logger.log(logging::INFO, "Client stopped");
                logger.deactivate();
            }
    };
}