#pragma once

#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <string>

namespace amulet::raw {
    #include <sys/socket.h>

    int __bind (int __fd, __CONST_SOCKADDR_ARG __addr, socklen_t __len){
        return bind(__fd, __addr, __len);
    }

    int __listen (int __fd, int __n){
        return listen(__fd, __n);
    }

    int __accept (int __fd, __SOCKADDR_ARG __addr, socklen_t *__restrict __addr_len){
        return accept(__fd, __addr, __addr_len);
    }

    int __connect (int __fd, __CONST_SOCKADDR_ARG __addr, socklen_t __len){
        return connect(__fd, __addr, __len);
    }

    ssize_t __send (int __fd, const void *__buf, unsigned __n, int __flags){
        return send(__fd, __buf, __n, __flags);
    }

    ssize_t __recv (int __fd, void *__buf, unsigned __n, int __flags){
        return recv(__fd, __buf, __n, __flags);
    }

    int __close (int __fd){
        return close(__fd);
    }
};