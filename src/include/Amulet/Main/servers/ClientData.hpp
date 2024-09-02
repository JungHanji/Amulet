#pragma once

#include <Amulet/Raw/socket.hpp>

namespace amulet::main::servers {
    struct ClientData {
        raw::Socket socket;
        int selfID;

        operator int(){
            return selfID;
        }

        operator raw::Socket&(){
            return socket;
        }
    };
};