#pragma once

#include "colors.hpp"
#include <vector>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <fstream>

namespace amulet::logging {
    enum MESSAGE_TYPE {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        FATAL,
        NEW_LAYER,
        LAYER_POPPED,
        INITED,
        CALLING
    };

    enum LOGGING_TYPE {
        CONSOLE_LOGGING,
        FILE_LOGGING,
        BOTH
    };

    class Logger {
            LOGGING_TYPE lgtype;
            std::string logFilePath;
            std::string startDate;
        
            std::vector<std::string> layers;
            bool logLayers;
            bool isActive;
        public:

            void setlayer(std::string layer){
                if(!isActive) return;
                if(logLayers && layer != layers.back()){
                    log(
                        NEW_LAYER,
                        layer
                    );
                }
                
                layers.clear();
                layers.push_back(layer);
            }

            void setlayers(std::vector<std::string> layer){
                if(!isActive) return;
                std::string layersPath;
                for(auto &_layer: layer){
                    layersPath += '[' + _layer + ']';
                }

                if(logLayers && layer != layers)
                    log(
                        NEW_LAYER,
                        layersPath
                    );
                
                layers = layer;
            }

            std::vector<std::string> getlayers(){
                if(!isActive) return {};
                return layers;
            }
            
            void newlayer(std::string layer){
                if(!isActive) return;
                if(logLayers)
                    log(
                        NEW_LAYER,
                        layer
                    );
                
                layers.push_back(layer);
            }

            std::string poplayer(){
                if(!isActive) return "not active";
                if(logLayers)
                    log(
                        LAYER_POPPED,
                        layers.back()
                    );
                
                std::string layer = layers.back();
                layers.pop_back();
                return layer;
            }

            void log(MESSAGE_TYPE type, const std::string& message){
                if(!isActive) return;

                std::string layersPath;
                std::string time;

                for(auto &layer: layers){
                    layersPath += '[' + layer + ']';
                }

                auto now = std::chrono::system_clock::now();
                auto now_time = std::chrono::system_clock::to_time_t(now);
                std::stringstream timess;
                timess << std::put_time(std::localtime(&now_time), "%T");
                time = timess.str();

                std::stringstream messagess;
                std::stringstream messagess_colorless;
                switch(type){
                    case DEBUG:
                        messagess << colors::LightFore.gray << time << colors::Fore.cyan << " [DEBUG]" << layersPath << ' ' << colors::LightFore.red << message << colors::Fore.reset;
                        messagess_colorless << time << " [DEBUG]" << layersPath << ' ' << message;
                        break;
                    case INFO:
                        messagess << colors::LightFore.gray << time << colors::Fore.green << " [INFO]" << layersPath << ' ' << colors::LightFore.red << message << colors::Fore.reset;
                        messagess_colorless << time << " [INFO]" << layersPath << ' ' << message;
                        break;
                    case WARNING:
                        messagess << colors::LightFore.gray << time << colors::LightFore.cyan << " [WARNING]" << layersPath << ' ' << colors::LightFore.red << message << colors::Fore.reset;
                        messagess_colorless << time << " [WARNING]" << layersPath << ' ' << message;
                        break;
                    case ERROR:
                        messagess << colors::LightFore.gray << time << colors::Fore.red << " [ERROR]" << layersPath << ' ' << colors::LightFore.red << message << colors::Fore.reset;
                        messagess_colorless << time << " [ERROR]" << layersPath << ' ' << message;
                        break;
                    case FATAL:
                        messagess << colors::LightFore.gray << time << colors::Fore.purple << " [FATAL]" << layersPath << ' ' << colors::LightFore.red << message << colors::Fore.reset;
                        messagess_colorless << time << " [FATAL]" << layersPath << ' ' << message;
                        break;
                    case INITED:
                        messagess << colors::LightFore.gray << time << colors::LightFore.yellow << " [INITED]" << layersPath << ' ' << colors::LightFore.red << message << colors::Fore.reset;
                        messagess_colorless << time << " [INITED]" << layersPath << ' ' << message;
                        break;
                    case CALLING:
                        messagess << colors::LightFore.gray << time << colors::LightFore.purple << " [CALLING]" << layersPath << ' ' << colors::LightFore.red << message << colors::Fore.reset;
                        messagess_colorless << time << " [CALLING]" << layersPath << ' ' << message;
                        break;
                    case NEW_LAYER:
                        messagess << std::endl << colors::LightFore.gray << time << colors::Fore.blue << " [NEW_LAYER]" << layersPath << ' ' << colors::LightFore.red << message << colors::Fore.reset;
                        messagess_colorless << std::endl << time << " [NEW_LAYER]" << layersPath << ' ' << message;
                        break;
                    case LAYER_POPPED:
                        messagess << std::endl << colors::LightFore.gray << time << colors::Fore.blue << " [LAYER_POPPED]" << layersPath << ' ' << colors::LightFore.red << message << colors::Fore.reset;
                        messagess_colorless << std::endl << time << " [LAYER_POPPED]" << layersPath << ' ' << message;
                        break;
                }

                switch(lgtype){
                    case CONSOLE_LOGGING:
                        std::cout << messagess.str() << std::endl;
                        break;
                    case FILE_LOGGING:
                        if(!logFilePath.empty()){
                            std::ofstream logFile(logFilePath, std::ios_base::app);
                            if(logFile.is_open()){
                                logFile << messagess_colorless.str() << std::endl;
                                logFile.close();
                            }
                        }
                        break;
                    case BOTH:
                        std::cout << messagess.str() << std::endl;
                        if(!logFilePath.empty()){
                            std::ofstream logFile(logFilePath, std::ios_base::app);
                            if(logFile.is_open()){
                                logFile << messagess_colorless.str() << std::endl;
                                logFile.close();
                            }
                        }
                        break;
                }
            }

            void activate(bool addDate = true, bool resetLogFile = true){
                isActive = true;
                init(addDate, resetLogFile);
            }

            void deactivate(){
                isActive = false;
            }

            Logger(
                LOGGING_TYPE lgtype = CONSOLE_LOGGING,
                std::string logFilePath = "",
                bool resetLogFile = true,
                bool logLayers = false,
                bool addDate = true,
                bool isActive = true
            ):
                lgtype(lgtype), 
                logFilePath(logFilePath),
                logLayers(logLayers),
                isActive(isActive)
            {
                if(isActive){
                    init(addDate, resetLogFile);
                }
            }

            ~Logger(){}

            private:

                void init(bool addDate, bool resetLogFile){
                    system("clear");
                    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

                    char buf[100] = {0};
                    std::strftime(buf, sizeof(buf), "%Y-%m-%d", std::localtime(&now));
                    startDate = std::string(buf);

                    if(addDate){
                        this->logFilePath += "\ " + startDate;
                        log(INITED, "-------------------------------->");
                        log(INFO, "Log started on " + startDate);
                    } else {
                        log(INITED, "-------------------------------->");
                        log(INFO, "Log started");
                    }

                    if(resetLogFile && (lgtype == BOTH || lgtype == FILE_LOGGING)){
                        std::ofstream logFile(this->logFilePath);
                        logFile << "\n";
                        logFile.close();
                    } else {
                        std::ofstream logFile(this->logFilePath, std::ios::app);
                        logFile << "\n";
                        logFile.close();
                    }
                }
    };
}