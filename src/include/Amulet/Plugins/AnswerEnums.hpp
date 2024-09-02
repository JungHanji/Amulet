#pragma once

#include <string>

namespace amulet::main::servers::plugins {
    enum ANSWER_CODE {
        SUCCESS,
        FAILURE,
        FORBIDDEN,
        UNKNOWN_ERROR,
        IGNORE,
        WE_GIVE_UP
    };

    enum ACCEPTION_CODE {
        ACCEPTED,
        PERMANENTLY_REJECTED,
        PERMANENTLY_BLOCKED,
        ACCEPTION_IGNORED
    };

    enum MESSAGEPROC_CODE {
        PROCESSED,
        REJECTED,
        IGNORED
    };

    enum ERRORPROC_CODE {
        CAN_CONTINUE,
        CANNOT_CONTINUE
    };
    
    enum ANSWERCODE_TYPE {
        ACCEPTION,
        MESSAGEPROC,
        ERRORPROC,
        UNKNOWN
    };

    enum ANSWER_FORMAT {
        FULL_ANSWER,
        MESSAGE_ONLY,
        ERROR_ONLY,
        GENERAL_ONLY
    };
}

namespace amulet::main::servers::plugins {
    std::string to_string(ANSWER_CODE code){
        switch(code){
            case SUCCESS: return "success";
            case FAILURE: return "failures";
            case FORBIDDEN: return "forbidden";
            case UNKNOWN_ERROR: return "unknown error";
            case IGNORE: return "ignore";
            case WE_GIVE_UP: return "we give up =(";
        }
        return "unknown answer_code " + std::to_string(code) + ')';
    }

    std::string to_string(ACCEPTION_CODE code){
        switch(code){
            case ACCEPTED: return "accepted";
            case PERMANENTLY_REJECTED: return "permanently rejected";
            case PERMANENTLY_BLOCKED: return "permanently blocked";
            case ACCEPTION_IGNORED: return "ignored";
        }
        return "unknown acception_code " + std::to_string(code) + ')';
    }

    std::string to_string(MESSAGEPROC_CODE code) {
        switch(code) {
            case PROCESSED: return "processed";
            case REJECTED: return "rejected";
            case IGNORED: return "ignored";
        }
        return "unknown messageproc code (" + std::to_string(code) + ')';
    }

    std::string to_string(ERRORPROC_CODE code) {
        switch(code) {
            case CAN_CONTINUE: return "can continue";
            case CANNOT_CONTINUE: return "for a reason you can not continue";
        }
        return "unknown errorproc_code " + std::to_string(code) + ')';
    }

    std::string to_string(ANSWERCODE_TYPE code){
        switch(code){
            case ACCEPTION: return "acception";
            case MESSAGEPROC: return "message processing";
            case ERRORPROC: return "error processing";
            case UNKNOWN: return "unknown code";
        }
        return "unknown answercode_type type " + std::to_string(code) + ')';;
    }

    std::string to_string(ANSWER_FORMAT code){
        switch(code){
            case FULL_ANSWER: return "full answer";
            case MESSAGE_ONLY: return "message only";
            case ERROR_ONLY: return "error only";
            case GENERAL_ONLY: return "general information only";
        }
        return "unknown format " + std::to_string(code) + ')';
    }
}