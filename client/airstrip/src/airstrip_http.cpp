#include "airstrip_http.h"

#include <airstrip_log.h>
#include <iostream>
#include <boost/process.hpp>


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
        cmd << R"(curl -w "\n%{http_code}" -X )" << methodToString(method);

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
                return Response{false, 0, "", ""};
            }

            // 去除末尾换行符
            if (!result.empty() && result.back() == '\n') {
                result.pop_back();
            }

            // Parse http code
            const size_t pos = result.find_last_of('\n');
            if (pos == string::npos) {
                logPrintln("Invalid response format", WARN, __FUNCTION__);
                return Response{false, 0, "", "Invalid response format"};
            }

            const string body = result.substr(0, pos);
            return Response{true, stoi(result.substr(pos + 1)), body, ""};
        } catch (const std::exception &e) {
            stringstream errorEx;
            errorEx << "Exception while executing command: " << e.what();
            logPrintln(errorEx.str(), WARN, __FUNCTION__);
            return Response{false, 0, "", ""};
        }
    }
}
