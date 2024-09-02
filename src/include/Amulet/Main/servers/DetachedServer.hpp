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
    class DetachedServer {
            raw::Address address;
            raw::Socket socket;

            unsigned maxClients;
            unsigned maxReadSize;
            std::map<int, ClientData> clients;
            std::map<int, std::shared_ptr<std::thread>> cli_threads;

            std::mutex clientMutex;
            logging::Logger logger;
        public:

            void startup(unsigned maxClients){
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
                    cli_threads[i] = std::make_shared<std::thread>([&](int cliind){
                        
                        raw::Socket newsock;
                        raw::ERROR_CODE nstatus = socket.accept(newsock);
                        clientMutex.lock();
                            ClientData data;
                            data.socket = newsock;
                            data.selfID = cliind;
                            clients[cliind] = data;
                        clientMutex.unlock();

                        if(nstatus != raw::SUCCESS){
                            logger.log(logging::FATAL, "Client #" + std::to_string(cliind) + " can not be accepted, because: " + raw::getStringError(status));
                            throw std::runtime_error("Can not accept client connection (" + std::to_string(cliind) + "), because: " + raw::getStringError(nstatus));
                        }

                        if(onClientConnected){
                            clientMutex.lock();
                                onClientConnected(clients[cliind]);
                            clientMutex.unlock();
                        }

                        cycle(cliind);

                    }, i);
                    cli_threads[i]->detach();
                    logger.log(logging::INFO, "Inited and call an acception thread for the client");
                }
                logger.poplayer();
                logger.poplayer();
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
                    clientMutex.lock();
                        onClientForcefullyDisconnected(clients[ClientIndex]);
                    clientMutex.unlock();
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

            DetachedServer(
                raw::Address address,
                unsigned maxReadSize = 1024,
                bool logServer = false
            ): 
                address(address), 
                socket(address, raw::SOCK_SERV),
                maxReadSize(maxReadSize),
                logger(
                    logging::BOTH,
                    "./logs/detached-server-log",
                    false,
                    false,
                    true,
                    logServer
                )
            {
                logger.log(logging::INITED, "Detached server");
                logger.newlayer("Detached server");
            }

            DetachedServer(){}
            ~DetachedServer(){
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

                    if(clients.find(ClientIndex) == clients.end()){
                        return;
                    }

                    if(onClientDisconnected){
                            onClientDisconnected(clients[ClientIndex]);
                    }
                    
                    clients[ClientIndex].socket.close();
                    clients.erase(ClientIndex);
                }

                void cycle(int ClientIndex){
                    raw::ERROR_CODE status;
                    std::string request;
                    std::string response;

                    clientMutex.lock();
                    status = socket.recv(request, clients[ClientIndex], maxReadSize);
                    clientMutex.unlock();

                    if(status != raw::SUCCESS){
                        disconnect(ClientIndex);
                    }

                    if(onClientMessage){
                        clientMutex.lock();
                        onClientMessage(clients[ClientIndex], request, response);
                        clientMutex.unlock();
                    }

                    clientMutex.lock();
                    status = socket.send(response, clients[ClientIndex]);
                    clientMutex.unlock();

                    if(status != raw::SUCCESS){
                        disconnect(ClientIndex);
                    }

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