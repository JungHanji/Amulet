#pragma once

#include <functional>
#include <iostream>

#include <Amulet/Logging/logger.hpp>
#include <Amulet/Main/servers/ClientData.hpp>
#include <Amulet/Main/servers/ServerLayers.hpp>

#include "Version.hpp"
#include "Answer.hpp"

namespace amulet::main::servers::plugins {

    template<class ServerType>
    class Plugin {
        public:

        Version version;

        virtual bool canIAcceptThisClient(const ClientData &client, ServerType *server) = 0; // Yes
        virtual bool canIDisconnectThisClient(const ClientData &client, ServerType *server) = 0; // Yes
        virtual bool canIFDisconnectThisClient(const ClientData &client, ServerType *server) = 0; // Yes
        virtual bool canIReceiveMessageFromThisClient(const ClientData &client, ServerType *server) = 0; 
        virtual bool canISendMessageToThisClient(const ClientData &client, const std::string &message, const std::string &hisRequest, ServerType *server) = 0; // Yes
        virtual bool canILogMe(ServerType *server) = 0; // Yes
        virtual bool canIStartUP(ServerType *server) = 0; // Yes

        // virtual Answer whenICantStartLogging(logging::Logger &logger, ServerType *server) = 0;
        virtual Answer whenICantAccept(ServerType *server) = 0; // Yes
        virtual Answer whenICantBind(ServerType *server) = 0; // Yes
        virtual Answer whenICantStartListen(ServerType *server) = 0; // Yes
        virtual Answer whenICantReceiveMessage(ClientData &client, ServerType *server) = 0; // Yes
        virtual Answer whenICantSendMessage(const ClientData &client, const std::string &message, ServerType *server) = 0; // Yes
        virtual Answer whenICantProperlyClose(ServerType *server) = 0; // Yes
        virtual Answer whenICantProperlyFDisconnect(const ClientData &client, ServerType *server) = 0; // Yes
        virtual Answer whenICantFindClient(int clientIndex, LAYERS layer, ServerType *server) = 0; // Yes

        virtual Answer whenClientSendsMessage(ClientData &client, std::string message, std::string &response, ServerType *server) = 0; // Yes
        virtual Answer whenClientDisconnects(ClientData &client, ServerType *server) = 0; // Yes
        virtual Answer whenClientProperlyFDisconnected(ClientData &client, ServerType *server) = 0; // Yes
        virtual Answer whenClientConnected(ClientData &client, ServerType *server) = 0; // Yes

        // virtual Answer whenIGetFatalError(const std::string message, raw::ERROR_CODE error_code) = 0;
        virtual Answer whenIGetWarning(const std::string message, raw::ERROR_CODE error_code) = 0;
        virtual Answer whenIGetError(const std::string message, raw::ERROR_CODE error_code) = 0; 

        virtual void whenPluginFails() = 0;
        virtual void whenServerFails() = 0;
        virtual void onPluginException() noexcept = 0;
        // virtual void onServerException() noexcept = 0;

        virtual ~Plugin() = 0; 

        Plugin(){}
    };
};