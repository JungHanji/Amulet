#include <Amulet/Main/clients/BlockingClient.hpp>
#include <iostream>

using namespace amulet::main::clients;


int main(){
    BlockingClient client({"127.0.0.1", 9000}, true);
    client.connect();

    std::string message = "Hello from client!";

    std::cout << "Client sended: " << message << std::endl;
    std::string response = client.cycle(message);
    std::cout << "Client received response: " << response << std::endl;

    return 0;
}