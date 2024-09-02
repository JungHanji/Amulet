#include <Amulet/Main/clients/BlockingClient.hpp>
#include <iostream>

using namespace amulet::main::clients;


int main(){

    std::string ip;
    int port;

    std::cout << "Enter server's IP to connect: ";
    std::cin >> ip;
    std::cout << "Enter opened port on the server to connect: ";
    std::cin >> port;

    BlockingClient client({ip, port}, true);
    client.connect();

    std::string message = "Hello from client!";

    std::cout << "Client sended: " << message << std::endl;
    std::string response = client.cycle(message);
    std::cout << "Client received response: " << response << std::endl;

    return 0;
}