#pragma once

#include <string>

namespace amulet::main::servers::plugins {
    class Version {
        public:
        std::string fullname;
        
        int major;
        int minor;
        int patch;

        operator std::string(){

            if (fullname.empty()){
                return std::to_string(major) + '.' + std::to_string(minor) + '.' + std::to_string(patch);
            } else if (major == -1) {
                return fullname;
            } else if (minor == -1){
                return fullname + " v" + std::to_string(major);
            } else {
                return fullname + ':' + std::to_string(major) + '.' + std::to_string(minor) + '.' + std::to_string(patch);
            }
        }

        Version(int major): major(major), minor(0), patch(0), fullname("") {}
        Version(std::string fullname): major(-1), minor(0), patch(0), fullname(fullname) {}
        Version(std::string fullname, int major): major(major), minor(-1), patch(0), fullname(fullname) {}
        Version(int major, int minor, int patch): major(major), minor(minor), patch(patch), fullname("") {}
        Version(int major, int minor, int patch, std::string fullname): major(major), minor(minor), patch(patch), fullname(fullname) {}
        Version(): major(0), minor(0), patch(0) {}
    };
}