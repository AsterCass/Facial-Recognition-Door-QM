#include "airstrip_command.h"

#include <array>
#include <sstream>
#include "airstrip_log.h"
#include <boost/process.hpp>

using namespace std;

namespace bp = boost::process;

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
            logPrintln("Failed to execute script " + address, WARN, __FUNCTION__);
        }
        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            result += buffer.data();
        }
        const int status = pclose(pipe);
        if (status != 0) {
            const string error = "Script " + address + " execution failed with status: "
                                 + to_string(status) + " Error is : " + result;
            logPrintln(error, WARN, __FUNCTION__);
        }
        const size_t pos = result.find_last_of('\n');
        if (pos == string::npos) {
            logPrintln("Invalid response format script " + address, WARN, __FUNCTION__);
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
            logPrintln("Failed to execute command " + cmd, WARN, __FUNCTION__);
        }
        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            result += buffer.data();
        }
        const int status = pclose(pipe);
        if (status != 0) {
            const string error = "Command " + cmd + " execution failed with status: "
                                 + to_string(status) + " Error is : " + result;
            logPrintln(error, WARN, __FUNCTION__);
        }
        const size_t pos = result.find_last_of('\n');
        if (pos == string::npos) {
            body = result;
        } else {
            body = result.substr(0, pos);
        }
        return body;
    }

    void execCommandNoReturn(const string &cmd) {
        system(cmd.c_str());
    }

    std::string execCommandBoost(const std::string &cmd) {
        namespace bp = boost::process;

        try {
            string result, error;
            bp::ipstream out_stream, err_stream;

            bp::child c(cmd, bp::std_out > out_stream, bp::std_err > err_stream);

            string line;
            while (out_stream && getline(out_stream, line))
                result += line + "\n";

            string err_line;
            while (err_stream && getline(err_stream, err_line))
                error += err_line + "\n";

            c.wait();
            const int exit_code = c.exit_code();

            if (exit_code != 0) {
                const string errorMsg = "Command " + cmd + " execution failed with status: "
                                        + to_string(c.exit_code()) + " Error is : " + error;
                logPrintln(errorMsg, WARN, __FUNCTION__);
                return "";
            }

            // 去除末尾换行符
            if (!result.empty() && result.back() == '\n') {
                result.pop_back();
            }

            return result;
        } catch (const std::exception &e) {
            stringstream errorEx;
            errorEx << "Exception while executing command: " << cmd << " , Error = " << e.what();
            logPrintln(errorEx.str(), WARN, __FUNCTION__);
            return "";
        }
    }
}
