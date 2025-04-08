#include "airstrip_http.h"

#include <airstrip_log.h>
#include <iostream>
#include <array>


using namespace std;


namespace airstrip {
    std::string escapeQuotes(const std::string &input) {
        std::string result;
        for (const char c: input) {
            if (c == '"' || c == '\\' || c == '`' || c == '$') {
                result += '\\';
            }
            result += c;
        }
        return result;
    }

    Response AirstripHttp::sendRequest(
        const std::string &url,
        RequestMethod method,
        const std::map<std::string, std::string> &params,
        const std::string &bodyJson,
        int timeoutSec,
        const std::string &certPath
    ) {
        std::string curl_cmd = buildCurlCommand(url, method, params, bodyJson, timeoutSec, certPath);
        return executeCurlCommand(curl_cmd);
    }


    string AirstripHttp::buildCurlCommand(
        const string &url,
        RequestMethod method,
        const map<string, string> &params,
        const string &bodyJson,
        int timeoutSec,
        const string &certPath
    ) {
        stringstream cmd;
        // base
        cmd << R"(curl -s -w "\n%{http_code}" -X )" << methodToString(method);

        // timeout time
        cmd << " --max-time " << timeoutSec;

        // ssl
        if (!certPath.empty()) {
            cmd << " --cacert " << certPath;
        }

        // request body
        if (!bodyJson.empty()) {
            cmd << R"( -H "Content-Type: application/json" -d ")" << escapeQuotes(bodyJson) << R"(")";
        }

        // url
        string fullUrl = url;
        if (!params.empty()) {
            fullUrl += "?";
            int count = 0;
            const auto total = params.size();
            for (const auto &param: params) {
                fullUrl += param.first + "=" + param.second;
                if (count < total) {
                    fullUrl += "&";
                }
                ++count;
            }
        }

        cmd << " \"" << fullUrl << "\"";

        return cmd.str();
    }


    Response AirstripHttp::executeCurlCommand(const string &cmd) {
        array<char, 128> buffer{};
        string result;

        // Execute command
        FILE *pipe = popen(cmd.c_str(), "r");
        if (!pipe) {
            logPrintln("Failed to execute command", ERROR, __FUNCTION__);
            return Response{false, 0, "", "Failed to execute command"};
        }

        // Read output
        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            result += buffer.data();
        }

        // Get command status
        const int status = pclose(pipe);
        if (status != 0) {
            const string error = "Command execution failed with status: " +
                                 to_string(status) + " Error is : " + result;
            logPrintln(error, ERROR, __FUNCTION__);
            return Response{false, 0, "", error};
        }

        // Parse http code
        const size_t pos = result.find_last_of('\n');
        if (pos == string::npos) {
            logPrintln("Invalid response format", ERROR, __FUNCTION__);
            return Response{false, 0, "", "Invalid response format"};
        }

        const string body = result.substr(0, pos);
        return Response{true, stoi(result.substr(pos + 1)), body, ""};
    }
}
