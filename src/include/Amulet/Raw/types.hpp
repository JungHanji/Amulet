#pragma once

#include <string>
#include <netinet/in.h>

namespace amulet::raw {
    class Address {
            std::vector<char> ip = std::vector<char>(INET_ADDRSTRLEN);
            int port;

            std::string getstringip(){
                std::string strip;
                for(char &ch: ip){
                    strip += ch;
                    if(ch == '\0') break;
                }
                return strip;
            }
        public:
            Address(
                std::string ip,
                int port
            ){
                this->port = port;
                this->ip.assign(ip.begin(), ip.end());
            }

            Address(
                const char *ip,
                int port
            ){
                this->port = port;
                this->ip.assign(ip, ip + INET_ADDRSTRLEN);
            }

            Address(){}
            ~Address(){}

            char* operator&(){
                return &ip[0];
            }

            operator int(){
                return port;
            }

            operator std::string(){
                return getstringip() + ":" + std::to_string(port);
            }
    };
};