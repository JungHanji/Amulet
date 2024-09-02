#include <iostream>

class Plugin{
    public:

    virtual void func1() = 0;
    virtual void func2() = 0;
    virtual void func3() = 0;
};

class Server {
    public:
    Plugin* plugin;

    void loadPlugin(Plugin* p){
        plugin = p;
    }

    void start(){
        plugin->func1();
        plugin->func2();
        plugin->func3();
    }
};

class MyPlugin: public Plugin {
    public:
    
    void func1() override {
        std::cout << "MyPlugin func1" << std::endl;
    }

    void func2() override {
        std::cout << "MyPlugin func2" << std::endl;
    }

    void func3() override {
        std::cout << "MyPlugin func3" << std::endl;
    }
};

int main(){
    Server server;
    MyPlugin myPlugin;

    server.loadPlugin(&myPlugin);

    server.start();
}