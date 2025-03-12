#include "api/api.h"

#include <boost/json.hpp>

#include "airstrip_command.h"
#include "airstrip_http.h"
#include "config/config.h"
#include <boost/uuid/detail/md5.hpp>
#include <boost/algorithm/hex.hpp>

#include "airstrip_log.h"

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
}
