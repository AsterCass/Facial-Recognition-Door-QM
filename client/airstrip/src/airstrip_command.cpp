#include "airstrip_command.h"

#include <array>
#include "airstrip_log.h"

using namespace std;

namespace airstrip {
    string execScript(const string &address) {
        array<char, 128> buffer{};
        string result;
        string body;
#ifdef WIN32
        const string script = "powershell.exe -ExecutionPolicy Bypass -File " + address;
        FILE *pipe = popen(script.c_str(), "r");
#else
        const string script = "sh " + address;
        FILE *pipe = popen(script.c_str(), "r");
#endif

        if (!pipe) {
            logPrintln("Failed to execute script " + address, ERROR, __FUNCTION__);
        }
        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            result += buffer.data();
        }
        const int status = pclose(pipe);
        if (status != 0) {
            const string error = "Script " + address + " execution failed with status: " + to_string(status);
            logPrintln(error, ERROR, __FUNCTION__);
        }
        const size_t pos = result.find_last_of('\n');
        if (pos == string::npos) {
            logPrintln("Invalid response format script " + address, ERROR, __FUNCTION__);
        } else {
            body = result.substr(0, pos);
        }
        return body;
    }


    string execCommand(const string &cmd) {
        array<char, 128> buffer{};
        string result;
        string body;
        FILE *pipe = popen(cmd.c_str(), "r");
        if (!pipe) {
            logPrintln("Failed to execute command " + cmd, ERROR, __FUNCTION__);
        }
        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            result += buffer.data();
        }
        const int status = pclose(pipe);
        if (status != 0) {
            const string error = "Command " + cmd + " execution failed with status: " + to_string(status);
            logPrintln(error, ERROR, __FUNCTION__);
        }
        const size_t pos = result.find_last_of('\n');
        if (pos == string::npos) {
            logPrintln("Invalid response format command " + cmd, ERROR, __FUNCTION__);
        } else {
            body = result.substr(0, pos);
        }
        return body;
    }
}
