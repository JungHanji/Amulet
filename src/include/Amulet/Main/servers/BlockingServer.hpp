#pragma once

#include <map>
#include <functional>
#include <memory>

#include <Amulet/Raw/socket.hpp>
#include <Amulet/Logging/logger.hpp>
#include <Amulet/Plugins/ServerPlugin.hpp>

#include "ServerLayers.hpp"
#include "ClientData.hpp"

namespace amulet::main::servers {
    class Mercury;

    class BlockingServer {
            using ServerType = BlockingServer;
            raw::Address address;
            raw::Socket socket;

            unsigned maxClients;
            unsigned maxReadSize;
            std::map<int, ClientData> clients;

            logging::Logger logger;
            std::shared_ptr<plugins::Plugin<BlockingServer>> plugin;
        public:

            logging::Logger &getLogger(){
                return logger;
            }

            void startup(unsigned maxClients){
                if(!plugin->canIStartUP(this)) return;
                logger.newlayer("Startup");
                raw::ERROR_CODE status;
                this->maxClients = maxClients;

                logger.log(logging::CALLING, "Binding...");
                status = socket.bind();
                if(status != raw::SUCCESS){
                    plugin->whenICantBind(this);
                    logger.log(logging::FATAL, "Can not bind server to requested address: " + (std::string)address + ", because: " + raw::getStringError(status));
                    // throw std::runtime_error("Can not bind server to requested address: " + (std::string)address + ", because: " + raw::getStringError(status));
                }
                logger.log(logging::INFO, "Success!");

                logger.log(logging::CALLING, "Start to listen clients (" + std::to_string(this->maxClients) + ')');
                status = socket.listen(this->maxClients);
                if(status != raw::SUCCESS){
                    plugin->whenICantStartListen(this);
                    logger.log(logging::FATAL, "Server can not start listening, because: " + raw::getStringError(status));
                    // throw std::runtime_error("Server can not start listening, because: " + raw::getStringError(status));
                }
                logger.log(logging::INFO, "Success!");

                logger.log(logging::INITED, "Start to accepting a clients");
                logger.newlayer("Acception cycle");
                for(int i = 0; i < this->maxClients; i++){
                    logger.log(logging::CALLING, "Trying to accept a client #" + std::to_string(i));
                    status = socket.accept(clients[i]);

                    if(status != raw::SUCCESS){
                        plugin->whenICantAccept(this);
                        logger.log(logging::FATAL, "Client #" + std::to_string(i) + " can not be accepted, because: " + raw::getStringError(status));
                        // throw std::runtime_error("Can not accept client connection (" + std::to_string(i) + "), because: " + raw::getStringError(status));
                    }
                    logger.log(logging::INFO, "Success! Client with address " + (std::string)clients[i].socket.address() + " connected");
                    if(!plugin->canIAcceptThisClient(clients[i], this)){
                        fdisconnect(i);
                        continue;
                    }

                    if(onClientConnected){
                        logger.log(logging::CALLING, "Calling callback [onClientConnected]");
                        onClientConnected(clients[i]);
                    } else {
                        logger.log(logging::WARNING, "No callback [onClientConnected] set. Client #" + std::to_string(i) + " will be not procceded");
                        plugin->whenIGetWarning("No callback [onClientConnected] set. Client #" + std::to_string(i) + " will be not procceded", raw::ERROR_CODE::SUCCESS);
                    }

                    plugin->whenClientConnected(clients[i], this);
                }
                logger.poplayer();
                logger.poplayer();
            }

            void fdisconnect(int ClientIndex){
                logger.newlayer("Force diconnect");
                
                if(clients.find(ClientIndex) != clients.end()){
                    plugin->whenICantFindClient(ClientIndex, FDISCONNECT, this);
                    logger.poplayer();
                    return;
                }
                std::shared_ptr<ClientData> client = std::make_shared<ClientData>(clients[ClientIndex]);

                if(!plugin->canIFDisconnectThisClient(*client, this)){
                    logger.poplayer();
                    return;
                }

                if(onClientForcefullyDisconnected){
                    logger.log(logging::CALLING, "Calling callback [onClientForcefullyDisconnected]");
                    onClientForcefullyDisconnected(*client);
                } else {
                    logger.log(logging::WARNING, "No callback [onClientForcefullyDisconnected] set. Client " + std::to_string(ClientIndex) + " will be not procceded");
                    plugin->whenIGetWarning("No callback [onClientForcefullyDisconnected] set. Client " + std::to_string(ClientIndex) + " will be not procceded", raw::ERROR_CODE::SUCCESS);
                }

                logger.log(logging::INFO, "Closing socket and cleaning client map");
                if(client->socket.close() != raw::SUCCESS){
                    plugin->whenICantProperlyFDisconnect(*client, this);
                }
                plugin->whenClientProperlyFDisconnected(*client, this);
                clients.erase(ClientIndex);
                logger.log(logging::INFO, "Client " + std::to_string(ClientIndex) + " is fully f-disconnected");
                logger.poplayer();
            }

            bool cycle(int ClientIndex){
                logger.newlayer("Cycle");
                raw::ERROR_CODE status;
                std::string request;
                std::string response;

                logger.log(logging::INITED, "Trying to get a reference to client #" + std::to_string(ClientIndex));
                if(clients.find(ClientIndex) != clients.end()){
                    plugin->whenICantFindClient(ClientIndex, CYCLE, this);
                    logger.poplayer();
                    return false;
                }
                auto &client = clients[ClientIndex];

                logger.log(logging::CALLING, "Trying to receive a request from client #" + std::to_string(ClientIndex));
                
                if(!plugin->canIReceiveMessageFromThisClient(client, this)){
                    logger.poplayer();
                    return false;
                }

                status = socket.recv(request, client, maxReadSize);
                if(status != raw::SUCCESS){
                    plugin->whenICantReceiveMessage(client, this);
                    // logger.log(logging::WARNING, "Failed to receive a request, because: " + raw::getStringError(status));
                    // disconnect(ClientIndex);
                    // logger.poplayer();
                    return false;
                }

                if(onClientMessage){
                    logger.log(logging::CALLING, "Calling callback [onClientMessage]");
                    onClientMessage(client, request, response);
                } else {
                    logger.log(logging::WARNING, "No callback [onClientMessage] set. Client #" + std::to_string(ClientIndex) + " will be not procceded");
                    plugin->whenIGetWarning("No callback [onClientMessage] set. Client #" + std::to_string(ClientIndex) + " will be not procceded", raw::SUCCESS);
                }

                plugin->whenClientSendsMessage(client, request, response, this);

                logger.log(logging::INFO, "Sending response to client #" + std::to_string(ClientIndex));
                
                if(!plugin->canISendMessageToThisClient(client, response, request, this)){
                    logger.poplayer();
                    return false;
                }
                
                status = socket.send(response, client);
                if(status != raw::SUCCESS){
                    plugin->whenICantSendMessage(client, response, this);
                    // logger.log(logging::WARNING, "Failed to send a response, because: " + raw::getStringError(status));
                    // disconnect(ClientIndex);
                    // logger.poplayer();
                    return false;
                }

                logger.poplayer();
                return true;
            }

            bool runiter(){
                logger.newlayer("Runiter");

                logger.log(logging::INITED, "Starting for-cycle for all clients");
                for(auto &client: clients){
                    
                    logger.log(logging::CALLING, "Starting Cycle for client #" + std::to_string(client.second.selfID));
                    if(!cycle(client.second.selfID)){
                        logger.log(logging::WARNING, "Failed to call cycle function");
                    }
                }
                logger.poplayer();
                return true;
            }

            void stop(){
                logger.newlayer("Stop");

                logger.log(logging::INFO, "Stopping all clients");
                // Собираем все ключи клиентов
                std::vector<int> clientKeys;
                for(auto &client : clients){
                    clientKeys.push_back(client.first);
                }

                // Отключаем клиентов по собранным ключам
                for(int key : clientKeys){
                    fdisconnect(key);
                }
                logger.log(logging::INFO, "Closing main socket");
                if(socket.close()!=raw::SUCCESS){
                    plugin->whenICantProperlyClose(this);
                }
            }

            void startLogging(){
                if(plugin->canILogMe(this))
                    logger.activate(true, false);
            }

            BlockingServer(
                raw::Address address,
                unsigned maxReadSize = 1024,
                bool logServer = false
            ): 
                address(address), 
                socket(address, raw::SOCK_SERV),
                maxReadSize(maxReadSize),
                logger(
                    logging::BOTH,
                    "./logs/blocking-server-log",
                    false,
                    false,
                    true,
                    logServer
                )
            {
                if(!plugin->canILogMe(this))
                    logger.deactivate();
                logger.newlayer("Blocking server");
                logger.log(logging::INITED, "Server inited");
            }

            BlockingServer(){}
            ~BlockingServer(){
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
                        plugin->whenICantFindClient(ClientIndex, DISCONNECT, this);
                        // logger.log(logging::WARNING, "Client " + std::to_string(ClientIndex) + " does not exist. May be it is already disconnected?");
                        // logger.poplayer();
                        return;
                    }

                    if(!plugin->canIDisconnectThisClient(clients[ClientIndex], this)) {
                        logger.poplayer();
                        return;
                    }

                    plugin->whenClientDisconnects(clients[ClientIndex], this);

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