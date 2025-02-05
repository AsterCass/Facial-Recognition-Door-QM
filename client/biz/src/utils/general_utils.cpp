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
            string arg = argv[i];
            if (arg.substr(0, 2) == "--"
            ) {
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

    // int loadArguments(int argc, char **argv) {
    //     auto args = parseArguments(argc, argv);
    //
    //     // Load env
    //     if (args.find("appEnv") != args.end()) {
    //         appEnv = args["appEnv"];
    //     }
    //     if (appEnv == ENV_DEV) {
    //         currentLogLevel = DEBUG;
    //     } else if (appEnv == ENV_PROD) {
    //         currentLogLevel = INFO;
    //     } else {
    //         currentLogLevel = TRACE;
    //     }
    //     cout << "Loaded arguments [appEnv] - [" << appEnv << "]" << endl;
    //     cout << "Loaded arguments [currentLogLevel] - [" << currentLogLevel << "]" << endl;
    //
    //     // Load print type
    //     if (args.find("logPrintType") != args.end()) {
    //         try {
    //             logPrintType = stoi(args.at("logPrintType"));
    //         } catch (...) {
    //             cerr << "Error parsing arguments logPrintType" << endl;
    //             return EXIT_FAILURE;
    //         }
    //     }
    //     cout << "Loaded arguments [logPrintType] - [" << logPrintType << "]" << endl;
    //
    //     // Load taskPullTaskIntervalCount
    //     if (args.find("taskPullTaskIntervalCount") != args.end()) {
    //         try {
    //             taskPullTaskIntervalCount = stoi(args.at("taskPullTaskIntervalCount"));
    //         } catch (...) {
    //             cerr << "Error parsing arguments taskPullTaskIntervalCount" << endl;
    //             return EXIT_FAILURE;
    //         }
    //     }
    //     cout << "Loaded arguments [taskPullTaskIntervalCount] - [" << taskPullTaskIntervalCount << "]" << endl;
    //
    //     // Else
    //
    //     return EXIT_SUCCESS;
    // }

}
