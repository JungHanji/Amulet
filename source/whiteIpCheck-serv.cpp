#include <iostream>
#include <Amulet/Main/servers/BlockingServer.hpp>

using namespace amulet::main;

int main(){

    std::string ip;
    int port;

    std::cout << "Enter your local IP to bind: ";
    std::cin >> ip;
    std::cout << "Enter port to bind the server: ";
    std::cin >> port;

    servers::BlockingServer server(
        {ip, port},
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

    server.startup(1);
    server.runiter();

    server.stop();
}