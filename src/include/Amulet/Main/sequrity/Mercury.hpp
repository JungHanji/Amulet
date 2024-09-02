#pragma once
#include <Amulet/Main/servers/BlockingServer.hpp>
#include <Amulet/Main/servers/AsyncServer.hpp>
#include <Amulet/Main/servers/DetachedServer.hpp>

namespace amulet::sequrity::blocking::mercury {
    class Mercury {
        main::servers::BlockingServer &server;
        public:

        bool block(raw::Address address){
        }

        bool block(raw::Socket socket){}

        Mercury(main::servers::BlockingServer &server):
            server(server) 
        {}
    };
}

namespace amulet::sequrity::acync::mercury {
    class Mercury {
        public:

    };
}

namespace amulet::sequrity::detached::mercury {
    class Mercury {
        public:

    };
}