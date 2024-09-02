#include <iostream>
#include <Amulet/Main/servers/DetachedServer.hpp>

using namespace amulet::main;

int main(){
    servers::DetachedServer server(
        {"127.0.0.1", 9000},
        1024,
        true
    );

    server.callbackOnClientConnected(
        [&](servers::ClientData &client){
            std::cout << "Client with address \"" << (std::string)client.socket.address() << "\" connected" << std::endl;
        }
    );
    
    server.callbackOnClientDisconnected(
        [&](servers::ClientData &client){
            std::cout << "Client with address \"" << (std::string)client.socket.address() << "\" diconnected" << std::endl;
        }
    );
    
    server.callbackOnClientForcefullyDisconnected(
        [&](servers::ClientData &client){
            std::cout << "Client with address \"" << (std::string)client.socket.address() << "\" forcefully disconnected" << std::endl;
        }
    );

    server.callbackOnClientMessage(
        [&](servers::ClientData &client, const std::string &message, std::string &response){
            response = "Echo: " + message;

            std::cout << "Client with address \"" << (std::string)client.socket.address() << "\" sent \"" << message << '"' << std::endl;
            std::cout << "...and server replied: " << response << std::endl;
        }
    );
    
    server.startup(3);
    
    for(int i = 0; i < 10; i++){
        std::cout << "doing math...\n";
        sleep(2);
    }
    
    server.stop();
}