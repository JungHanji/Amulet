#pragma once

#include <string>
#include "AnswerEnums.hpp"

namespace amulet::main::servers::plugins {

    class Answer {
            ANSWER_CODE generalCode;
            std::string generalMessage;

            ACCEPTION_CODE acceptionCode;
            MESSAGEPROC_CODE meessageProcCode;
            ERRORPROC_CODE errorProcCode;
            bool isGeneralCodeSpecified;
        public:
            ANSWERCODE_TYPE type;
            ANSWER_FORMAT   format;

            operator bool(){
                return generalCode == SUCCESS;
            }

            operator std::string(){
                switch(format) {
                    case FULL_ANSWER:
                        if (isGeneralCodeSpecified) {
                            return generalMessage;
                        } else {
                            std::string answer;
                            if(!generalMessage.empty()){
                                answer += generalMessage;
                            }
                            
                            answer += " in general: " + to_string(generalCode) + ", in detales: ";

                            switch(type){
                                case ACCEPTION:
                                    answer += to_string(acceptionCode);
                                case MESSAGEPROC:
                                    answer += to_string(meessageProcCode);
                                case ERRORPROC:
                                    answer += to_string(errorProcCode);
                                case UNKNOWN:
                                    answer += "Unknown error type";
                            }

                            return answer;
                        }
                    case MESSAGE_ONLY:
                        return generalMessage;
                    case ERROR_ONLY: {
                        switch(type){
                            case ACCEPTION:
                                return to_string(acceptionCode);
                            case MESSAGEPROC:
                                return to_string(meessageProcCode);
                            case ERRORPROC:
                                return to_string(errorProcCode);
                            case UNKNOWN:
                                return "Unknown error type";
                        }
                    }
                        
                    case GENERAL_ONLY:
                        return to_string(generalCode);
                    default:
                        return "No such answer format (" + std::to_string(format) + ')';
                }
            }

            Answer(ANSWER_CODE    general, 
                    ACCEPTION_CODE speccode, 
                    std::string    message = "no message specified", 
                    ANSWER_FORMAT  format = FULL_ANSWER):
                generalCode(general), 
                acceptionCode(speccode),
                format(format), 
                generalMessage(message) 
            {}

            Answer(ANSWER_CODE      general, 
                    MESSAGEPROC_CODE speccode, 
                    std::string      message = "no message specified", 
                    ANSWER_FORMAT    format = FULL_ANSWER):
                generalCode(general), 
                meessageProcCode(speccode), 
                format(format), 
                generalMessage(message)
            {}

            Answer(ANSWER_CODE    general, 
                    ERRORPROC_CODE speccode, 
                    std::string    message = "no message specified", 
                    ANSWER_FORMAT  format = FULL_ANSWER):
                generalCode(general), 
                errorProcCode(speccode),
                format(format), 
                generalMessage(message)
            {}

            Answer(){}
            ~Answer(){}
    };
}