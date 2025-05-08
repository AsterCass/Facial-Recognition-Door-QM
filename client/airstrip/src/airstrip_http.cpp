#include "airstrip_http.h"

#include <airstrip_log.h>
#include <iostream>
#include <boost/process.hpp>
#include <fstream>


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
        const std::string curl_cmd = buildCurlCommand(url, method, params, bodyJson, timeoutSec, certPath);
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

        // url
        string fullUrl = url;
        if (!params.empty()) {
            fullUrl += "?";
            int count = 0;
            const auto total = params.size();
            for (const auto &param: params) {
                fullUrl += param.first + "=" + param.second;
                if (count < total - 1) {
                    fullUrl += "&";
                }
                ++count;
            }
        }

        cmd << " \"" << fullUrl << "\"";

        // body 并检查是否过长
        if (!bodyJson.empty()) {
            if (bodyJson.length() > 100000) {
                // 设置一个合理的阈值
                // 创建临时文件存储请求体
                boost::filesystem::path tempFilePath = boost::filesystem::temp_directory_path() /
                                                       boost::filesystem::unique_path("curl_data_%%%%-%%%%-%%%%-%%%%");
                string tempFileName = tempFilePath.string();

                try {
                    std::ofstream tempFile(tempFileName);
                    if (!tempFile) {
                        logPrintln("Failed to create temp file for request body: " + tempFileName, WARN, __FUNCTION__);
                        // 如果创建临时文件失败，回退到常规方法（可能会失败）
                        cmd << R"( -H "Content-Type: application/json" -d ")" << escapeQuotes(bodyJson) << R"(")";
                    } else {
                        tempFile << bodyJson;
                        tempFile.close();

                        // 使用文件作为请求体
                        cmd << R"( -H "Content-Type: application/json" --data @)" << tempFileName;

                        // 记录临时文件信息以便在执行后清理
                        cmd << " #TEMP_FILE:" << tempFileName;
                    }
                } catch (const std::exception &e) {
                    logPrintln("Exception creating temp file: " + string(e.what()), WARN, __FUNCTION__);
                    // 回退到常规方法
                    cmd << R"( -H "Content-Type: application/json" -d ")" << escapeQuotes(bodyJson) << R"(")";
                }
            } else {
                // 如果请求体不过长，使用常规方法
                cmd << R"( -H "Content-Type: application/json" -d ")" << escapeQuotes(bodyJson) << R"(")";
            }
        }


        return cmd.str();
    }


    Response AirstripHttp::executeCurlCommand(const string &cmd) {
        namespace bp = boost::process;

        try {
            string result, error;
            bp::ipstream out_stream, err_stream;

            // 检查命令中是否包含临时文件标记
            size_t tempFilePos = cmd.find("#TEMP_FILE:");
            string tempFileName;
            string actualCmd = cmd;

            if (tempFilePos != string::npos) {
                // 提取临时文件名
                tempFileName = cmd.substr(tempFilePos + 11); // 11 是 "#TEMP_FILE:" 的长度
                // 移除命令中的临时文件标记
                actualCmd = cmd.substr(0, tempFilePos);
            }

            // 执行命令
            bp::child c(actualCmd, bp::std_out > out_stream, bp::std_err > err_stream);

            string line;
            while (out_stream && getline(out_stream, line))
                result += line + "\n";

            string err_line;
            while (err_stream && getline(err_stream, err_line))
                error += err_line + "\n";

            c.wait();
            const int exit_code = c.exit_code();

            // 如果存在临时文件，清理它
            if (!tempFileName.empty()) {
                try {
                    boost::filesystem::remove(tempFileName);
                } catch (const std::exception &e) {
                    logPrintln("Failed to remove temp file: " + tempFileName + ", error: " + e.what(), WARN,
                               __FUNCTION__);
                }
            }

            if (exit_code != 0) {
                const string errorMsg = "Command " + actualCmd +
                                        " execution failed with status: " + to_string(exit_code) +
                                        " Error is : " + error;
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
