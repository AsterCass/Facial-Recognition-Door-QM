#include "api/api.h"

#include <airstrip_thread_pool.h>
#include <iostream>
#include <thread>
#include <boost/json.hpp>

#include "airstrip_command.h"
#include "airstrip_http.h"
#include "config/config.h"
#include <boost/uuid/detail/md5.hpp>
#include <boost/algorithm/hex.hpp>

#include "airstrip_log.h"
#include "airstrip_program_options.h"
#include "utils/card_recognition.h"
#include "utils/face_recognition.h"
#include "utils/general_utils.h"
#include "utils/scheduled_task.h"

#ifndef Q_OS_WIN
#include <fcntl.h>
#include <sys/ioctl.h>

#define TELPO_IOF_MAGIC 't'

#define TELPO_IOCTL_MAX485_RD _IOW(TELPO_IOF_MAGIC, 0x51, int)
#define TELPO_IOCTL_IR_LED_POWER _IOW(TELPO_IOF_MAGIC, 0x52, int)
#define TELPO_IOCTL_RELAY _IOW(TELPO_IOF_MAGIC, 0x53, int)
#define TELPO_IOCTL_RS232_PWR _IOW(TELPO_IOF_MAGIC, 0x54, int)
#define TELPO_IOCTL_NFC_PWR _IOW(TELPO_IOF_MAGIC, 0x55, int)
#define TELPO_IOCTL_WG26 _IOW(TELPO_IOF_MAGIC, 0x26, int)
#define TELPO_IOCTL_WG34 _IOW(TELPO_IOF_MAGIC, 0x34, int)
#endif

using namespace std;

string snCode;
string token;
string staticSoundsDir;
mutex lockPlayWav;
deque<int> playWavQueue = {};
int fdForDoor = -1;
unsigned char doorKeyValues[6];

static time_t lastModifyIrLedTime = 0;

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

    string result;
    boost::algorithm::hex(reinterpret_cast<char *>(&digest),
                          reinterpret_cast<char *>(&digest) + sizeof(digest),
                          back_inserter(result));
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

int checkDoorKey() {
#ifndef WIN32
    if (-1 == fdForDoor) {
        fdForDoor = open("/dev/telpo_gpio", O_RDWR);
    }
    if (fdForDoor < 0) {
        logPrintln("Failed to open device", airstrip::ERROR, __FUNCTION__);
        return -1;
    }
    memset(doorKeyValues, 0, sizeof(doorKeyValues));
    int ret = read(fdForDoor, doorKeyValues, sizeof(doorKeyValues));
    if (ret != sizeof(doorKeyValues)) {
        logPrintln("Failed to read data", airstrip::ERROR, __FUNCTION__);
        return -1;
    }
    return doorKeyValues[2];

#endif
    logPrintln("Check door key ", airstrip::DEBUG, __FUNCTION__);
    return -1;
}

void closeIrLed() {
    logPrintln("Close ir led pre", airstrip::INFO, __FUNCTION__);
    auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    if (now - lastModifyIrLedTime < 1) {
        return;
    }
    lastModifyIrLedTime = now;
    logPrintln("Close ir led start", airstrip::INFO, __FUNCTION__);
#ifndef WIN32
    if (-1 == fdForDoor) {
        fdForDoor = open("/dev/telpo_gpio", O_RDWR);
    }
    if (fdForDoor < 0) {
        logPrintln("Failed to open device", airstrip::ERROR, __FUNCTION__);
        return;
    }
    auto openRet = ioctl(fdForDoor,TELPO_IOCTL_IR_LED_POWER,0);
    logPrintln("Close ir led " + to_string(openRet), airstrip::INFO, __FUNCTION__);
#endif
    g_enableIrLed = 0;
    logPrintln("Close ir led ", airstrip::INFO, __FUNCTION__);
}

void openIrLed() {
    logPrintln("Open ir led pre", airstrip::INFO, __FUNCTION__);
    auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    if (now - lastModifyIrLedTime < 1) {
        return;
    }
    lastModifyIrLedTime = now;
    logPrintln("Open ir led start", airstrip::INFO, __FUNCTION__);
#ifndef WIN32
    if (-1 == fdForDoor) {
        fdForDoor = open("/dev/telpo_gpio", O_RDWR);
    }
    if (fdForDoor < 0) {
        logPrintln("Failed to open device", airstrip::ERROR, __FUNCTION__);
        return;
    }
    auto openRet = ioctl(fdForDoor,TELPO_IOCTL_IR_LED_POWER,1);
    logPrintln("Open ir led " + to_string(openRet), airstrip::INFO, __FUNCTION__);
#endif
    g_enableIrLed = 1;
    logPrintln("Open ir led ", airstrip::INFO, __FUNCTION__);
}

void openDoor() {
#ifndef WIN32
    if (-1 == fdForDoor) {
        fdForDoor = open("/dev/telpo_gpio", O_RDWR);
    }
    if (fdForDoor < 0) {
        logPrintln("Failed to open device", airstrip::ERROR, __FUNCTION__);
        return;
    }
    auto openRet = ioctl(fdForDoor, TELPO_IOCTL_RELAY, 1);
    logPrintln("Open door ret " + to_string(openRet), airstrip::DEBUG, __FUNCTION__);
#endif
    logPrintln("Open door ", airstrip::INFO, __FUNCTION__);
}

void closeDoor() {
#ifndef WIN32
    if (-1 == fdForDoor) {
        fdForDoor = open("/dev/telpo_gpio", O_RDWR);
    }
    if (fdForDoor < 0) {
        logPrintln("Failed to open device", airstrip::ERROR, __FUNCTION__);
        return;
    }
    auto openRet = ioctl(fdForDoor, TELPO_IOCTL_RELAY, 0);
    logPrintln("Close door ret " + to_string(openRet), airstrip::DEBUG, __FUNCTION__);
#endif
    logPrintln("Close door ", airstrip::INFO, __FUNCTION__);
}

void playWav(const string &voiceTemplate) {
    if (!g_voice) {
        return;
    }
    if (!voiceTemplate.empty()) {
        try {
            const int rental = stoi(voiceTemplate.substr(1, 1));
            switch (rental) {
                case 1:
                    playWav(Rental_1);
                    break;
                case 2:
                    playWav(Rental_2);
                    break;
                case 3:
                    playWav(Rental_2);
                    break;
                default:
                    break;
            }

            const int charge = stoi(voiceTemplate.substr(2, 1));
            switch (charge) {
                case 1:
                    playWav(Charge_1);
                    break;
                case 2:
                    playWav(Charge_2);
                    break;
                case 3:
                    playWav(Charge_3);
                    break;
                case 4:
                    playWav(Charge_4);
                    break;
                case 5:
                    playWav(Charge_5);
                    break;
                default:
                    break;
            }
        } catch (const exception &e) {
            ostringstream errMsg;
            errMsg << e.what();
            logPrintln("Play wav operation error " + errMsg.str()
                       , airstrip::ERROR, __FUNCTION__);
        }
    }
}

void playWav(const PlayWavType type) {
    if (!g_voice) {
        return;
    }
    if (staticSoundsDir.empty()) {
        staticSoundsDir = g_appWorkDir + "static/sounds/";
    }

    // push to queue
    if (playWavQueue.size() >= 2) {
        playWavQueue.clear();
    }
    playWavQueue.push_back(type);


    // get last item play
    std::lock_guard<std::mutex> lock(lockPlayWav);
    if (playWavQueue.empty()) {
        airstrip::logPrintln("No wav to play");
        return;
    }
    const auto toPlayType = playWavQueue.front();
    playWavQueue.pop_front();

    ostringstream playWavMsg;
    playWavMsg << "aplay " << staticSoundsDir;
    switch (toPlayType) {
        case Di:
            playWavMsg << "check_di.wav";
            break;
        case AuthSuccess:
            playWavMsg << "check_success.wav";
            break;
        case AuthFail:
            playWavMsg << "check_fail.wav";
            break;
        case AuthFailFirst:
            playWavMsg << "check_fail_first.wav";
            break;
        case Disabled:
            playWavMsg << "check_disabled.wav";
            break;
        case Expired:
            playWavMsg << "check_expired.wav";
            break;
        case Charge_1:
            playWavMsg << "charge_1.wav";
            break;
        case Charge_2:
            playWavMsg << "charge_2.wav";
            break;
        case Charge_3:
            playWavMsg << "charge_3.wav";
            break;
        case Charge_4:
            playWavMsg << "charge_4.wav";
            break;
        case Charge_5:
            playWavMsg << "charge_5.wav";
            break;
        case Rental_1:
            playWavMsg << "rental_1.wav";
            break;
        case Rental_2:
            playWavMsg << "rental_2.wav";
            break;
        default: {
            logPrintln("Play wav not found type = " + to_string(toPlayType),
                       airstrip::ERROR, __FUNCTION__);
        }
    }
#ifdef WIN32
    std::this_thread::sleep_for(std::chrono::seconds(2));
#else
    airstrip::execCommand(playWavMsg.str());
#endif
    logPrintln("Play wav " + playWavMsg.str() + " queue size = " + to_string(playWavQueue.size())
               , airstrip::INFO, __FUNCTION__);
}

bool linkedServer() {
    return !token.empty();
}


// http

void login() {
    try {
        token = "";
        const auto now = chrono::system_clock::now();
        const auto sec = chrono::duration_cast<chrono::seconds>(
            now.time_since_epoch()).count();

        boost::json::object loginJson;
        loginJson["deviceId"] = getSn();
        loginJson["deviceSign"] = getSign();
        loginJson["deviceVersion"] = APP_VERSION;
        loginJson["timestamp"] = sec;

        const string bodyStr = serialize(loginJson);
        logPrintln("Api login body string = " + bodyStr, airstrip::INFO, __FUNCTION__);
        const auto ret = airstrip::AirstripHttp::sendRequest(
            g_serverAddress + "/api/v1/doorGuard/zFang/device/login",
            airstrip::RequestMethod::POST,
            {},
            bodyStr,
            5,
            CERT_PATH
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
    } catch (const exception &e) {
        ostringstream errMsg;
        errMsg << e.what();
        logPrintln("Login error " + errMsg.str()
                   , airstrip::ERROR, __FUNCTION__);
    }
}


void checkTask() {
    // todo 加锁

    if (token.empty()) {
        login();
        if (token.empty()) {
            return;
        }
    }

    const auto now = chrono::system_clock::now();
    const auto sec = chrono::duration_cast<chrono::seconds>(
        now.time_since_epoch()).count();

    boost::json::object taskListJson;
    taskListJson["deviceId"] = getSn();
    taskListJson["deviceToken"] = token;
    taskListJson["timestamp"] = sec;

    const string bodyStr = serialize(taskListJson);
    logPrintln("Api check task body string = " + bodyStr, airstrip::DEBUG, __FUNCTION__);


    airstrip::Response ret = airstrip::AirstripHttp::sendRequest(
        g_serverAddress + "/api/v1/doorGuard/zFang/device/taskList",
        airstrip::RequestMethod::POST,
        {},
        bodyStr,
        10,
        CERT_PATH
    );

    try {
        if (ret.success) {
            logPrintln("Api task body ret = " + ret.body, airstrip::DEBUG, __FUNCTION__);
            auto parsed = boost::json::parse(ret.body);
            if (HTTP_CODE_OK == parsed.at("code").as_int64()) {
                const auto taskList = parsed.at("data").as_array();
                if (taskList.empty()) {
                    logPrintln("Api not task deal with", airstrip::DEBUG, __FUNCTION__);
                    return;
                }

                map<string, int> taskStatusMap = {};

                for (const auto &task: taskList) {
                    string taskId;
                    bool isSuccess = false;
                    auto taskJson = task.as_object();
                    const auto dataType = taskJson.at("dataType").as_int64();
                    switch (dataType) {
                        case Face: {
                            auto taskData = taskJson.at("taskData").as_object();
                            taskId = taskData.at("taskId").as_string().c_str();
                            auto action = taskData.at("action").as_int64();
                            auto userId = taskData.at("userId").as_string().c_str();
                            auto startTime = taskData.at("startTime").as_int64();
                            auto endTime = taskData.at("endTime").as_int64();

                            ostringstream oss;
                            oss << "Api face get taskId: " << taskId << " userId: " << userId << " action: " <<
                                    action
                                    << " startTime: " << startTime << " endTime: " << endTime;
                            cout << oss.str() << endl;
                            logPrintln(oss.str(), airstrip::INFO, __FUNCTION__);

                            if (Add == action) {
                                auto picBase64String = taskData.at("facePhoto").as_string().c_str();
                                auto pic = generalUtils::base64ToMat(picBase64String);
                                if (!pic.empty()) {
                                    FaceUserInfo info = {};
                                    info.userId = userId;
                                    info.isEnable = true;
                                    info.startTime = startTime;
                                    info.endTime = endTime;
                                    isSuccess = faceInsert(pic, info);
                                }
                            } else if (Remove == action) {
                                FaceUserInfo info = {};
                                info.userId = userId;;
                                isSuccess = faceDelete(info);
                            } else if (Modify == action) {
                                auto picBase64String = taskData.at("facePhoto").as_string().c_str();
                                auto pic = generalUtils::base64ToMat(picBase64String);
                                FaceUserInfo info = {};
                                info.userId = userId;
                                info.isEnable = true;
                                info.startTime = startTime;
                                info.endTime = endTime;
                                isSuccess = faceUpdate(pic, info);
                            }

                            break;
                        }
                        case Card: {
                            auto taskData = taskJson.at("taskData").as_object();
                            taskId = taskData.at("taskId").as_string().c_str();
                            auto action = taskData.at("action").as_int64();
                            auto userId = taskData.at("userId").as_string().c_str();
                            auto startTime = taskData.at("startTime").as_int64();
                            auto endTime = taskData.at("endTime").as_int64();
                            auto cardType = taskData.at("cardType").as_int64();
                            auto cardNo = taskData.at("cardNo").as_string().c_str();

                            ostringstream oss;
                            oss << "Api card get taskId: " << taskId << " userId: " << userId << " action: " <<
                                    action
                                    << " startTime: " << startTime << " endTime: " << endTime << " cardNo: " <<
                                    cardNo;
                            cout << oss.str() << endl;
                            logPrintln(oss.str(), airstrip::INFO, __FUNCTION__);

                            if (Add == action) {
                                CardUserInfo info = {};
                                info.userId = userId;
                                info.isEnable = true;
                                info.cardNo = cardNo;
                                info.cardType = static_cast<int>(cardType);
                                info.startTime = startTime;
                                info.endTime = endTime;
                                isSuccess = cardInsert(info);
                            } else if (Remove == action) {
                                CardUserInfo info = {};
                                info.userId = userId;;
                                isSuccess = cardDelete(info);
                            } else if (Modify == action) {
                                CardUserInfo info = {};
                                info.userId = userId;
                                info.isEnable = true;
                                info.cardNo = cardNo;
                                info.cardType = static_cast<int>(cardType);
                                info.startTime = startTime;
                                info.endTime = endTime;
                                isSuccess = cardUpdate(info);
                            }
                            break;
                        }
                        case Disable: {
                            auto taskData = taskJson.at("taskData").as_object();
                            taskId = taskData.at("taskId").as_string().c_str();
                            auto action = taskData.at("action").as_int64();
                            auto userId = taskData.at("userId").as_string().c_str();
                            auto keyType = taskData.at("keyType").as_int64();
                            auto keyId = taskData.at("keyId").as_string().c_str();

                            ostringstream oss;
                            oss << "Api disable get taskId: " << taskId << " userId: " << userId << " action: " <<
                                    action
                                    << " keyType: " << keyType << " keyId: " << keyId;
                            cout << oss.str() << endl;
                            logPrintln(oss.str(), airstrip::INFO, __FUNCTION__);

                            if (keyType == 0) {
                                isSuccess = cardDisable(userId, keyId, action ? 0 : 1);
                                isSuccess &= faceDisable(userId, action ? 0 : 1);
                            } else if (keyType == 1) {
                                isSuccess = faceDisable(userId, action ? 0 : 1);
                            } else if (keyType == 3) {
                                isSuccess = cardDisable(userId, keyId, action ? 0 : 1);
                            } else {
                                isSuccess = false;
                            }

                            break;
                        }
                        case Voice: {
                            auto taskData = taskJson.at("taskData").as_object();
                            taskId = taskData.at("taskId").as_string().c_str();
                            auto userId = taskData.at("userId").as_string().c_str();
                            auto voiceTmp = taskData.at("voiceFeature").as_string().c_str();
                            ostringstream oss;
                            oss << "Api voice get taskId: " << taskId << " userId: " << userId
                                    << " voiceFeature: " << voiceTmp;
                            cout << oss.str() << endl;
                            logPrintln(oss.str(), airstrip::INFO, __FUNCTION__);

                            isSuccess = cardVoiceTemplate(userId, voiceTmp);
                            isSuccess &= faceVoiceTemplate(userId, voiceTmp);

                            break;
                        }
                        case Open: {
                            auto taskData = taskJson.at("taskData").as_object();
                            taskId = taskData.at("taskId").as_string().c_str();
                            auto userId = taskData.at("userId").as_string().c_str();

                            ostringstream oss;
                            oss << "Api open get taskId: " << taskId << " userId: " << userId;
                            cout << oss.str() << endl;
                            logPrintln(oss.str(), airstrip::INFO, __FUNCTION__);

                            OpenRecordInfo recordInfo = {};
                            recordInfo.userId = userId;
                            recordInfo.openMode = RemoteOpen;
                            recordInfo.openResult = 0;
                            recordInfo.openTime = sec;
                            isSuccess = ScheduledTask::commonOpenDoor(recordInfo);
                            break;
                        }
                        default: {
                        }
                    }

                    logPrintln("Api task " + taskId + " ret " + to_string(isSuccess),
                               airstrip::INFO, __FUNCTION__);
                    taskStatusMap[taskId] = isSuccess ? 0 : 1;
                    taskFinish(taskStatusMap);
                }
            } else {
                token = "";
                logPrintln("Api check task failed in server", airstrip::WARN, __FUNCTION__);
            }
        } else {
            token = "";
            logPrintln("Api check task failed in local", airstrip::WARN, __FUNCTION__);
        }
    } catch (const exception &e) {
        token = "";
        ostringstream errMsg;
        errMsg << e.what();
        logPrintln("Task rest text " + ret.body, airstrip::ERROR, __FUNCTION__);
        logPrintln("Task execute error " + errMsg.str(), airstrip::ERROR, __FUNCTION__);
    }
}

bool faceGrant(const cv::Mat &frame, const std::string &userPhone) {
    if (token.empty()) {
        login();
        if (token.empty()) {
            return false;
        }
    }

    // Compress
    cv::Size newSize(frame.cols / 2, frame.rows / 2);
    cv::Mat resizedImage;
    resize(frame, resizedImage, newSize, 0, 0, cv::INTER_AREA);
    std::vector<uchar> buffer;
    std::vector<int> compressionParams;
    compressionParams.push_back(cv::IMWRITE_JPEG_QUALITY);
    compressionParams.push_back(50);
    imencode(".jpg", resizedImage, buffer, compressionParams);
    cv::Mat compressedImage = imdecode(buffer, cv::IMREAD_COLOR);

    // Build
    boost::json::object faceGrantJson;
    faceGrantJson["deviceId"] = getSn();
    faceGrantJson["deviceToken"] = token;
    faceGrantJson["userPhone"] = userPhone;
    faceGrantJson["facePhoto"] = generalUtils::matToBase64(compressedImage);

    const string bodyStr = serialize(faceGrantJson);
    logPrintln("Api face grant request string = " + bodyStr, airstrip::DEBUG, __FUNCTION__);

    bool faceGrantRet = false;

    try {
        const auto ret = airstrip::AirstripHttp::sendRequest(
            g_serverAddress + "/api/v1/doorGuard/zFang/device/authGrant",
            airstrip::RequestMethod::POST,
            {},
            bodyStr,
            30,
            CERT_PATH
        );

        if (ret.success) {
            logPrintln("Api face grant ret = " + ret.body, airstrip::DEBUG, __FUNCTION__);
            auto parsed = boost::json::parse(ret.body);

            if (HTTP_CODE_OK == parsed.at("code").as_int64()) {
                const auto userData = parsed.at("data").as_object();
                const auto userId = userData.at("userId").as_string().c_str();
                const auto startTime = userData.at("startTime").as_int64();
                const auto endTime = userData.at("endTime").as_int64();

                ostringstream oss;
                oss << "Add user : " << userId << " startTime: " << startTime << " endTime: " << endTime;
                cout << oss.str() << endl;
                logPrintln(oss.str(), airstrip::INFO, __FUNCTION__);

                FaceUserInfo info = {};
                info.userId = userId;
                info.startTime = startTime;
                info.endTime = endTime;
                info.isEnable = true;
                faceGrantRet = faceInsert(frame, info);
            } else {
                logPrintln("Api face grant failed in server", airstrip::WARN, __FUNCTION__);
            }
        } else {
            token = "";
            logPrintln("Api face grant failed in local", airstrip::WARN, __FUNCTION__);
        }
    } catch (const std::exception &e) {
        ostringstream errMsg;
        errMsg << e.what();
        logPrintln("Api face grant failed exception: " + errMsg.str()
                   , airstrip::ERROR, __FUNCTION__);
    }


    return faceGrantRet;
}

AppUpdateNotification appUpdate() {
    if (token.empty()) {
        login();
        if (token.empty()) {
            return {};
        }
    }

    AppUpdateNotification notification = {};

    try {
        boost::json::object retObj;
        retObj["deviceId"] = getSn();
        retObj["deviceToken"] = token;
        retObj["deviceVersion"] = APP_VERSION;
        const string bodyStr = serialize(retObj);
        logPrintln("Api app update body string = " + bodyStr,
                   airstrip::DEBUG, __FUNCTION__);

        const auto ret = airstrip::AirstripHttp::sendRequest(
            g_serverAddress + "/api/v1/doorGuard/zFang/device/pluginUpgrade",
            airstrip::RequestMethod::POST,
            {},
            bodyStr,
            5,
            CERT_PATH
        );
        if (ret.success) {
            logPrintln("Ret body ret = " + ret.body,
                       airstrip::DEBUG, __FUNCTION__);
            auto parsed = boost::json::parse(ret.body);
            if (HTTP_CODE_OK == parsed.at("code").as_int64()) {
                notification.isSuccessful = true;
                const auto app = parsed.at("data").as_object();
                const auto url = app.at("deviceVersionUrl").as_string().c_str();
                const auto version = app.at("deviceVersion").as_string();
                g_latestVersion = version.c_str();
                if (version != APP_VERSION) {
                    notification.isNeedUpdate = true;
                    notification.updateVersion = version.c_str();
                    notification.updateUrl = url;
                }
            } else {
                token = "";
            }
        } else {
            token = "";
            logPrintln("Ret failed in local",
                       airstrip::WARN, __FUNCTION__);
        }
    } catch (const exception &e) {
        ostringstream errMsg;
        errMsg << e.what();
        logPrintln("Update operation error " + errMsg.str()
                   , airstrip::ERROR, __FUNCTION__);
    }

    return notification;
}

bool uploadOpenRecord(const std::vector<OpenRecordInfo> &records) {
    if (token.empty()) {
        login();
        if (token.empty()) {
            return false;
        }
    }
    bool uploadSuccessful = false;
    try {
        const auto now = chrono::system_clock::now();
        const auto time = chrono::system_clock::to_time_t(now);

        boost::json::object retObj;
        retObj["deviceId"] = getSn();
        retObj["deviceToken"] = token;
        boost::json::array recordListJsonObj;
        for (const auto &record: records) {
            boost::json::object recordJsonObj;
            recordJsonObj["userId"] = record.userId;
            recordJsonObj["cardNo"] = record.cardNo;
            recordJsonObj["cardType"] = record.cardType;
            recordJsonObj["openMode"] = record.openMode;
            recordJsonObj["openTime"] = record.openTime;
            recordJsonObj["openResult"] = record.openResult;
            recordJsonObj["uploadTime"] = time;
            recordListJsonObj.push_back(recordJsonObj);
        }
        retObj["list"] = recordListJsonObj;

        const string bodyStr = serialize(retObj);
        logPrintln("Request body string = " + bodyStr,
                   airstrip::DEBUG, __FUNCTION__);
        const auto ret = airstrip::AirstripHttp::sendRequest(
            g_serverAddress + "/api/v1/doorGuard/zFang/device/openDoorRecordList",
            airstrip::RequestMethod::POST,
            {},
            bodyStr,
            10,
            CERT_PATH
        );
        if (ret.success) {
            logPrintln("Ret body ret = " + ret.body,
                       airstrip::DEBUG, __FUNCTION__);
            auto parsed = boost::json::parse(ret.body);
            if (HTTP_CODE_OK == parsed.at("code").as_int64()) {
                uploadSuccessful = true;
            } else {
                token = "";
            }
        } else {
            token = "";
            logPrintln("Ret failed in local",
                       airstrip::WARN, __FUNCTION__);
        }
    } catch (const exception &e) {
        ostringstream errMsg;
        errMsg << e.what();
        logPrintln("Upload open record operation error " + errMsg.str()
                   , airstrip::ERROR, __FUNCTION__);
    }
    return uploadSuccessful;
}

void taskFinish(const map<string, int> &taskStatusMap) {
    if (token.empty()) {
        login();
        if (token.empty()) {
            return;
        }
    }
    try {
        boost::json::object retObj;
        retObj["deviceId"] = getSn();
        retObj["deviceToken"] = token;
        boost::json::array tasks;
        for (const auto &taskStatus: taskStatusMap) {
            boost::json::object taskObj;
            taskObj["taskId"] = taskStatus.first;
            taskObj["taskStatus"] = taskStatus.second;
            tasks.push_back(taskObj);
        }
        retObj["list"] = tasks;

        const string bodyStr = serialize(retObj);
        logPrintln("Api check task ret body string = " + bodyStr,
                   airstrip::DEBUG, __FUNCTION__);
        const auto ret = airstrip::AirstripHttp::sendRequest(
            g_serverAddress + "/api/v1/doorGuard/zFang/device/taskStatus",
            airstrip::RequestMethod::POST,
            {},
            bodyStr,
            10,
            CERT_PATH
        );
        if (ret.success) {
            logPrintln("Api task ret body ret = " + ret.body,
                       airstrip::DEBUG, __FUNCTION__);
        } else {
            token = "";
            logPrintln("Api check task ret failed in local",
                       airstrip::WARN, __FUNCTION__);
        }
    } catch (const exception &e) {
        ostringstream errMsg;
        errMsg << e.what();
        logPrintln("Face operation error " + errMsg.str()
                   , airstrip::ERROR, __FUNCTION__);
    }
}

bool dataBackupUp(const std::string &fileBase64) {
    if (token.empty()) {
        login();
        if (token.empty()) {
            return false;
        }
    }

    try {
        boost::json::object retObj;
        retObj["deviceId"] = getSn();
        retObj["deviceToken"] = token;
        retObj["backupBase64"] = fileBase64;

        const string bodyStr = serialize(retObj);
        // logPrintln("Data backup body string = " + bodyStr, airstrip::DEBUG, __FUNCTION__);
        const auto ret = airstrip::AirstripHttp::sendRequest(
            g_serverAddress + "/api/v1/doorGuard/zFang/device/uploadBackup",
            airstrip::RequestMethod::POST,
            {},
            bodyStr,
            20,
            CERT_PATH
        );
        if (ret.success) {
            logPrintln("Data backup ret = " + ret.body,
                       airstrip::DEBUG, __FUNCTION__);
        } else {
            token = "";
            logPrintln("Data backup failed in local",
                       airstrip::WARN, __FUNCTION__);
            return false;
        }
    } catch (const exception &e) {
        ostringstream errMsg;
        errMsg << e.what();
        logPrintln("Data backup error " + errMsg.str()
                   , airstrip::ERROR, __FUNCTION__);
        return false;
    }

    return true;
}
