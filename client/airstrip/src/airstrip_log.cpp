#include "airstrip_log.h"
#include "airstrip_configs.h"
#include <chrono>
#include <iomanip>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include <sstream>
#include <thread>
#include <boost/filesystem.hpp>

using namespace std;
namespace fs = boost::filesystem;

namespace airstrip {
    bool startClearThread = false;

    void clearHistoryLog() {
        const auto now = chrono::system_clock::now();
        const auto time = chrono::system_clock::to_time_t(now);
        const std::time_t cutoff = time - (logReDays * 24 * 60 * 60);

        const fs::directory_iterator end_iter;
        for (fs::directory_iterator iter(logPrintPath); iter != end_iter; ++iter) {
            if (is_regular_file(iter->status())) {
                if (iter->path().extension() == ".txt" &&
                    iter->path().stem().extension() == ".log") {
                    const std::time_t fileTime = last_write_time(iter->path());
                    if (fileTime < cutoff) {
                        fs::remove(iter->path());
                    }
                }
            }
        }
    }

    void clearHistoryLogThreadStart() {
        if (logPrintPath.empty()) {
            return;
        }
	thread cleanup_thread([&] {
            while (true) {
                this_thread::sleep_for(chrono::hours(12));
                clearHistoryLog();
            }
        });
        cleanup_thread.detach();
    }

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
            const string logFileName = logPrintPath + oss.str().substr(0, 10).append(".log.txt");
            ofstream logFile(logFileName, ios_base::app);
            if (logFile.is_open()) {
                logFile << outputStr << endl;
            } else {
                cerr << "Error opening file for logging." << endl;
            }
        }

        // Log clear
        if (!startClearThread) {
            startClearThread = true;
            clearHistoryLogThreadStart();
        }
    }
}
