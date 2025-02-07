#include "airstrip_log.h"
#include "airstrip_configs.h"
#include <chrono>
#include <iomanip>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <sstream>

using namespace std;

namespace airstrip {
    void logPrintln(const string &message, LogLevel level, const string &functionName) {
        // Time formate
        const auto now = chrono::system_clock::now();
        const auto time = chrono::system_clock::to_time_t(now);
        const auto msec = chrono::duration_cast<chrono::milliseconds>(
                              now.time_since_epoch()).count() % 1000;
        ostringstream oss;
        string outputStr;
        oss << put_time(localtime(&time), "%Y-%m-%d %H:%M:%S") << "." << msec;

        if (static_cast<int>(level) >= logPrintLevel) {
            switch (level) {
                case TRACE:
                    outputStr.append("[TRACE][").append(oss.str())
                            .append("][").append(functionName).append("] ").append(message);
                    break;
                case DEBUG:
                    outputStr.append("[DEBUG][").append(oss.str())
                            .append("][").append(functionName).append("] ").append(message);
                    break;
                case INFO:
                    outputStr.append("[INFO][").append(oss.str())
                            .append("][").append(functionName).append("] ").append(message);
                    break;
                case WARN:
                    outputStr.append("[WARN][").append(oss.str())
                            .append("][").append(functionName).append("] ").append(message);
                    break;
                case ERROR:
                    outputStr.append("[ERROR][").append(oss.str())
                            .append("][").append(functionName).append("] ").append(message);
                    break;
                case CRITICAL:
                    outputStr.append("[CRITICAL][").append(oss.str())
                            .append("][").append(functionName).append("] ").append(message);
                    break;
            }
        }

        if (outputStr.empty()) {
            return;
        }

        if (logPrintPath.empty()) {
            cout << outputStr << endl;
        } else {
            const string logFileName = logPrintPath + oss.str().substr(0, 10).append(".txt");
            std::ofstream logFile(logFileName, std::ios_base::app);
            if (logFile.is_open()) {
                logFile << outputStr << std::endl;
            } else {
                std::cerr << "Error opening file for logging." << std::endl;
            }
        }
    }
}
