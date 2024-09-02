#include <iostream>
#include <Amulet/Main/servers/AsyncServer.hpp>

using namespace amulet::main;

int main(){
    servers::AsyncServer server(
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
    
    auto acceptthreads = server.startup(3);
    
    for(auto &thread: acceptthreads){
        thread.join();
        
    }

    std::cout << "All clients accepted\n";
    
    auto cyclethreads = server.runiter();
    
    std::cout << "Clients to procceed: " << cyclethreads.size() << std::endl;

    int i = 0;
    for(auto &thread: cyclethreads){
        thread.join();
        std::cout << "Client #" << ++i << " processed\n";
    }
    
    server.stop();
}