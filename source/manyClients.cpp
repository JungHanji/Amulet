#include <Amulet/Main/clients/BlockingClient.hpp>
#include <iostream>
#include <memory>
#include <thread>
#include <map>

using namespace amulet::main::clients;

std::map<int, BlockingClient> clients;

void processClient(int i){
    std::string message = "Hello from client #" + std::to_string(i);

    std::cout << i << " sended: " << message << std::endl;
    std::string response = clients[i].cycle(message);
    std::cout << i << " received response: " << response << std::endl;
}

int main(){

    
    std::vector<std::shared_ptr<std::thread>> threads;

    for(int i = 0; i < 3; i++){
        threads.push_back(
            std::make_shared<std::thread>([i]{
                clients[i] = BlockingClient({"127.0.0.1", 9000}, true);
                clients[i].connect();
            })
        );
    }

    for(int i = 0; i < 3; i++){
        threads[i]->join();
    }

    threads.clear();

    for(int i = 0; i < 3; i++){
        threads.push_back(
            std::make_shared<std::thread>([i]{
                processClient(i);
            })
        );
    }

    for(int i = 0; i < 3; i++){
        threads[i]->join();
    }

    return 0;
}