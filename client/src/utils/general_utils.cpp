#include "utils/general_utils.h"

#include <chrono>
#include <iomanip>
#include <string>
#include <map>
#include <iostream>
#include "config/config.h"
#include <fstream>
#include <ctime>
#include <sstream>

using namespace std;

namespace generalUtils {
    map<string, string> parseArguments(int argc, char **argv) {
        map<string, string> args;
        for (int i = 1; i < argc; ++i) {
            if (string arg = argv[i]; arg.substr(0, 2) == "--") {
                string key = arg.substr(2);
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    args[key] = argv[i + 1];
                    ++i;
                } else {
                    args[key] = "";
                }
            }
        }
        return args;
    }

    int loadArguments(int argc, char **argv) {
        auto args = parseArguments(argc, argv);

        // Load env
        if (args.find("appEnv") != args.end()) {
            appEnv = args["appEnv"];
        }
        if (appEnv == ENV_DEV) {
            currentLogLevel = DEBUG;
        } else if (appEnv == ENV_PROD) {
            currentLogLevel = INFO;
        } else {
            currentLogLevel = TRACE;
        }
        cout << "Loaded arguments [appEnv] - [" << appEnv << "]" << endl;
        cout << "Loaded arguments [currentLogLevel] - [" << currentLogLevel << "]" << endl;

        // Load print type
        if (args.find("logPrintType") != args.end()) {
            try {
                logPrintType = stoi(args.at("logPrintType"));
            } catch (...) {
                cerr << "Error parsing arguments logPrintType" << endl;
                return EXIT_FAILURE;
            }
        }
        cout << "Loaded arguments [logPrintType] - [" << logPrintType << "]" << endl;

        // Load taskPullTaskIntervalCount
        if (args.find("taskPullTaskIntervalCount") != args.end()) {
            try {
                taskPullTaskIntervalCount = stoi(args.at("taskPullTaskIntervalCount"));
            } catch (...) {
                cerr << "Error parsing arguments taskPullTaskIntervalCount" << endl;
                return EXIT_FAILURE;
            }
        }
        cout << "Loaded arguments [taskPullTaskIntervalCount] - [" << taskPullTaskIntervalCount << "]" << endl;

        // Else

        return EXIT_SUCCESS;
    }


    void logPrintln(LogLevel level, const string &functionName, const string &message) {
        // Time formate
        const auto now = chrono::system_clock::now();
        const auto time = chrono::system_clock::to_time_t(now);
        const auto msec = chrono::duration_cast<chrono::milliseconds>(
                              now.time_since_epoch()).count() % 1000;
        ostringstream oss;
        string outputStr;
        oss << put_time(localtime(&time), "%Y-%m-%d %H:%M:%S") << "." << msec;

        if (static_cast<int>(level) >= currentLogLevel) {
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

        if (logPrintType == CONSOLE) {
            cout << outputStr << endl;
        } else if (logPrintType == OFILE) {
            const string logFileName = oss.str().substr(0, 10).append(".txt");
            if (std::ofstream logFile(logFileName, std::ios_base::app); logFile.is_open()) {
                logFile << outputStr << std::endl;
            } else {
                std::cerr << "Error opening file for logging." << std::endl;
            }
        }
    }
}
