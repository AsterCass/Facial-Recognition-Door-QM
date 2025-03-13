#include "api/api.h"

#include <iostream>
#include <boost/json.hpp>

#include "airstrip_command.h"
#include "airstrip_http.h"
#include "config/config.h"
#include <boost/uuid/detail/md5.hpp>
#include <boost/algorithm/hex.hpp>

#include "airstrip_log.h"
#include "utils/face_recognition.h"
#include "utils/general_utils.h"

using namespace std;

string snCode = "";
string token = "";

// local
string getSn() {
#ifdef  WIN32
    return "Q_M_R_Z_PRO_002";
#else
    if (snCode.empty()) {
        snCode = airstrip::execCommand("linux_sn_read | tail -n 1");
    }
    return snCode;
#endif
}

string getSign() {
    const string data = getSn() + g_signId;
    boost::uuids::detail::md5 hash;
    boost::uuids::detail::md5::digest_type digest;

    hash.process_bytes(data.data(), data.size());
    hash.get_digest(digest);

    std::string result;
    boost::algorithm::hex(reinterpret_cast<char *>(&digest),
                          reinterpret_cast<char *>(&digest) + sizeof(digest),
                          std::back_inserter(result));
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

void openDoor() {
}

void closeDoor() {
}

void playWav(const string &voiceTemplate) {
}

void playWav(PlayWavType type) {
}


// http

void login() {
    token = "";
    const auto now = chrono::system_clock::now();
    const auto sec = chrono::duration_cast<chrono::seconds>(
        now.time_since_epoch()).count();

    boost::json::object loginJson;
    loginJson["deviceId"] = getSn();
    loginJson["deviceSign"] = getSign();
    loginJson["deviceVersion"] = APP_VERSION;
    loginJson["timestamp"] = sec;

#ifdef  WIN32
    const string certPath;
#else
    const string certPath = "/etc/ssl/certs/ca-certificates.crt";
#endif
    const string bodyStr = serialize(loginJson);
    logPrintln("Api login body string = " + bodyStr, airstrip::INFO, __FUNCTION__);
    const auto ret = airstrip::AirstripHttp::sendRequest(
        g_serverAddress + "/api/v1/doorGuard/zFang/device/login",
        airstrip::RequestMethod::POST,
        {},
        bodyStr,
        10,
        certPath
    );

    if (ret.success) {
        logPrintln("Api login ret = " + ret.body, airstrip::INFO, __FUNCTION__);
        auto parsed = boost::json::parse(ret.body);
        if (HTTP_CODE_OK == parsed.at("code").as_int64()) {
            auto data = parsed.at("data").as_object();
            const auto deviceToken = data.at("deviceToken").as_string().c_str();
            token = deviceToken;
            logPrintln("Api login finish", airstrip::INFO, __FUNCTION__);
        } else {
            logPrintln("Api login failed in server", airstrip::WARN, __FUNCTION__);
        }
    } else {
        logPrintln("Api login failed in local", airstrip::WARN, __FUNCTION__);
    }
}


void checkTask() {
    if (token.empty()) {
        // todo re login
        return;
    }

    const auto now = chrono::system_clock::now();
    const auto sec = chrono::duration_cast<chrono::seconds>(
        now.time_since_epoch()).count();

    boost::json::object taskListJson;
    taskListJson["deviceId"] = getSn();
    taskListJson["deviceToken"] = token;
    taskListJson["timestamp"] = sec;


#ifdef  WIN32
    const string certPath;
#else
    const string certPath = "/etc/ssl/certs/ca-certificates.crt";
#endif
    const string bodyStr = serialize(taskListJson);
    logPrintln("Api check task body string = " + bodyStr, airstrip::DEBUG, __FUNCTION__);
    const auto ret = airstrip::AirstripHttp::sendRequest(
        g_serverAddress + "/api/v1/doorGuard/zFang/device/taskList",
        airstrip::RequestMethod::POST,
        {},
        bodyStr,
        10,
        certPath
    );

    if (ret.success) {
        logPrintln("Api task body ret = " + ret.body, airstrip::DEBUG, __FUNCTION__);
        auto parsed = boost::json::parse(ret.body);
        if (HTTP_CODE_OK == parsed.at("code").as_int64()) {
            const auto taskList = parsed.at("data").as_array();
            if (taskList.empty()) {
                logPrintln("Api not task deal with", airstrip::DEBUG, __FUNCTION__);
                return;
            }
            for (const auto &task: taskList) {
                bool isSuccess = false;
                auto taskJson = task.as_object();
                const auto dataType = taskJson.at("dataType").as_int64();
                switch (dataType) {
                    case Face: {
                        auto taskData = taskJson.at("taskData").as_object();
                        auto action = taskData.at("action").as_int64();
                        auto taskId = taskData.at("taskId").as_string().c_str();
                        auto userId = taskData.at("userId").as_string().c_str();
                        auto startTime = taskData.at("startTime").as_int64();
                        auto endTime = taskData.at("endTime").as_int64();

                        ostringstream oss;
                        oss << "Api get taskId: " << taskId << " userId: " << userId << " action: " << action
                                << " startTime: " << startTime << " endTime: " << endTime;
                        cout << oss.str() << endl;
                        logPrintln(oss.str(), airstrip::INFO, __FUNCTION__);
                        try {
                            if (Add == action) {
                                auto picBase64String = taskData.at("facePhoto").as_string().c_str();
                                auto pic = generalUtils::base64ToMat(picBase64String);
                                FaceUserInfo info = {};
                                info.userId = userId;;
                                info.startTime = startTime;
                                info.endTime = endTime;
                                info.isEnable = true;
                                isSuccess = faceInsert(pic, info);
                            } else if (Remove == action) {
                                FaceUserInfo info = {};
                                info.userId = userId;;
                                isSuccess = faceDelete(info);
                            } else if (Modify == action) {
                                auto picBase64String = taskData.at("facePhoto").as_string().c_str();
                                auto pic = generalUtils::base64ToMat(picBase64String);
                                FaceUserInfo info = {};
                                info.userId = userId;;
                                info.startTime = startTime;
                                info.endTime = endTime;
                                info.isEnable = true;
                                isSuccess = faceUpdate(pic, info);
                            }
                        } catch (const exception &e) {
                            ostringstream errMsg;
                            errMsg << e.what();
                            logPrintln("Face operation error " + errMsg.str()
                                       , airstrip::ERROR, __FUNCTION__);
                        }
                        break;
                    }
                    case Card: {
                        break;
                    }
                    case Disable: {
                        break;
                    }
                    case Voice: {
                        break;
                    }
                    case Open: {
                        break;
                    }
                    default: {
                    }
                }
            }
        } else {
            logPrintln("Api check task failed in server", airstrip::WARN, __FUNCTION__);
        }
    } else {
        logPrintln("Api check task failed in local", airstrip::WARN, __FUNCTION__);
    }
}

void faceGrant() {
}

void appUpdate() {
}

void uploadOpenRecord() {
}

void taskFinish() {
}

void dataBackupUp() {
}

void dataBackupDown() {
}
