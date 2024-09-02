#pragma once

namespace amulet::main::servers {
    enum LAYERS {
        BINDING,
        LISTENING,
        ACCEPTING,
        RECEVEING,
        SENDING,
        CLOSING,
        FDISCONNECT,
        DISCONNECT,
        CYCLE,
        RUNITER
    };
}