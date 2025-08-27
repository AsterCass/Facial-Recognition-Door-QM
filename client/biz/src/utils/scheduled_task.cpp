#include "utils/scheduled_task.h"

#include <airstrip_thread_pool.h>
#include <iomanip>
#include <thread>
#include <sstream>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "airstrip_command.h"
#include "airstrip_log.h"
#include "config/config.h"
#include "airstrip_program_options.h"
#include "api/api.h"
#include "camera/camera_frame.h"
#include "db/card_db.h"
#include "db/face_db.h"
#include "db/open_record_db.h"
#include "enums/general_enums.h"
#include "nfc/nfc_tool.h"
#include "ui/main_router.h"
#include "ui/components/main_component_header.h"
#include "utils/card_recognition.h"
#include "utils/face_recognition.h"
#include "utils/global_data_manager.h"
#include <boost/filesystem.hpp>
#include <utils/general_utils.h>

int doorOpenSec = 0;
int messageLabelSec = 0;
int64_t lastShowFaceRegisterTime = 0;

using namespace std;
using namespace airstrip;
using namespace boost::gregorian;
namespace fs = boost::filesystem;

// Every (30 * 60 * (taskIvCnt + executionTime)) sec
void updatePersistentData() {
    static int count = 1740;
    if (count++ < 1800) return;
    count = 1;
    // Every 22 hour
    static time_t lastTime = 0;
    const auto now = chrono::system_clock::now();
    const auto time = chrono::system_clock::to_time_t(now);
    if (time - lastTime < 22 * 60 * 60) {
        return;
    }
    // Operation
    lastTime = time;
    logPrintln("Update persistent data", INFO, __FUNCTION__);
    // Delete db data
    deleteYearRecordDB();
    // Delete face file
    {
        const std::time_t cutoff = time - (5 * 24 * 60 * 60);
        const fs::directory_iterator end_iter;
        const string faceLogDic = g_appWorkDir + "log-face/";
        for (fs::directory_iterator iter(faceLogDic); iter != end_iter; ++iter) {
            if (is_regular_file(iter->status())) {
                if (iter->path().extension() == ".jpg") {
                    const std::time_t fileTime = last_write_time(iter->path());
                    if (fileTime < cutoff) {
                        logPrintln("Delete file: " + iter->path().string(),
                                   INFO, __FUNCTION__);
                        fs::remove(iter->path());
                    }
                }
            }
        }
    }
    // Delete dump file
    {
        const std::time_t cutoff = time - (30 * 24 * 60 * 60);
        const fs::directory_iterator end_iter;
        const string faceLogDic = g_appWorkDir + "dump/";
        for (fs::directory_iterator iter(faceLogDic); iter != end_iter; ++iter) {
            if (is_regular_file(iter->status())) {
                if (iter->path().extension() == ".dmp") {
                    const std::time_t fileTime = last_write_time(iter->path());
                    if (fileTime < cutoff) {
                        logPrintln("Delete file: " + iter->path().string(),
                                   INFO, __FUNCTION__);
                        fs::remove(iter->path());
                    }
                }
            }
        }
    }
    // Backup data
    if (g_needBackup) {
        logPrintln("Start backup data thread");
        static_cast<ThreadPool *>(g_mainThreadPool)->enqueue([] {
            logPrintln("Start backup data");
#ifndef WIN32
            execScript(g_appWorkDir + "script/linux/backup.sh ");
#endif
            logPrintln("Got backup data");
#ifndef WIN32
            const auto fileBase64 = generalUtils::fileToBase64(g_appWorkDir + "frd.bk.tar.gz");
            dataBackupUp(fileBase64);
#endif
            logPrintln("Backup data finish");
        });
    }
    // Delete system tmp file
    {
#ifndef WIN32
        const std::time_t cutoff = time - (5 * 24 * 60 * 60);
        const fs::directory_iterator end_iter;
        const string dic = "/data/";
        for (fs::directory_iterator iter(dic); iter != end_iter; ++iter) {
            if (is_regular_file(iter->status())) {
                if (iter->path().filename().string().substr(0, 5) == "core-") {
                    const std::time_t fileTime = last_write_time(iter->path());
                    if (fileTime < cutoff) {
                        logPrintln("Delete file: " + iter->path().string(),
                                   INFO, __FUNCTION__);
                        fs::remove(iter->path());
                    }
                }
            }
        }
#endif
    }
}


// Every (30 * 60 * (taskIvCnt + executionTime)) sec
void uploadAppData() {
    static int count = 1680;
    if (count++ < 1800) return;
    count = 1;

    logPrintln("Start upload app data", INFO, __FUNCTION__);

    // Upload open record
    const auto data = getAllRecordNotUpload();
    if (!data.empty()) {
        const auto uploadRet = uploadOpenRecord(data);
        if (uploadRet) {
            vector<int64_t> recordIds = {};
            for (const auto &record: data) {
                recordIds.emplace_back(record.openRecordId);
            }
            uploadedOpenRecordDB(recordIds);
        } else {
            logPrintln("Upload open record fail", WARN, __FUNCTION__);
        }
    }

    // else
}

// Every (60 * (taskIvCnt + executionTime)) sec
void updateCommonAppData() {
    static int count = 60;
    if (count++ < 60) return;
    count = 1;
    logPrintln("Start update common app data", DEBUG, __FUNCTION__);
    if (g_lightOnlyCheck) {
#ifndef WIN32
        closeLight();
#endif
    }
}


// Every (10 * (taskIvCnt + executionTime)) sec
void updateUIMainComponentHeader() {
    static int count = 10;
    if (count++ < 10) return;
    count = 1;

    logPrintln("Start update time", DEBUG, __FUNCTION__);

    const auto now = chrono::system_clock::now();
    const auto time = chrono::system_clock::to_time_t(now);
    const auto today = day_clock::local_day();
    const auto day = today.day_of_week();
    const auto &weekStr = CHINESE_WEEK[day];

    ostringstream oss;
    oss << put_time(localtime(&time), "%Y.%m.%d %H:%M ") << weekStr;
    GlobalDataManager::getInstance()->updateHeaderTime(string(oss.str()));

    if (g_appWorkDir.empty()) {
        return;
    }

    // Wired
    logPrintln("Start update wired", DEBUG, __FUNCTION__); {
#ifdef WIN32
        g_wiredIp = execScript(g_appWorkDir + "script/win/get_wired_ip.ps1");
#else
        g_wiredIp = execScript(g_appWorkDir + "script/linux/get_wired_ip.sh");
#endif
        GlobalDataManager::getInstance()->updateHeaderWired(g_wiredIp);
    }

    // Wireless
    logPrintln("Start update wireless", DEBUG, __FUNCTION__); {
#ifdef WIN32
        g_wirelessIp = execScript(g_appWorkDir + "script/win/get_wireless_ip.ps1");
#else
         g_wirelessIp = execScript(g_appWorkDir + "script/linux/get_wireless_ip.sh");
#endif
        GlobalDataManager::getInstance()->updateHeaderWireless(g_wirelessIp);
    }

    // 4g
    logPrintln("Start update 4g", DEBUG, __FUNCTION__); {
#ifndef WIN32
        g_fourGIp = execScript(g_appWorkDir + "script/linux/get_4g_ip.sh");
#endif
        GlobalDataManager::getInstance()->updateHeaderFourG(g_fourGIp);
    }

    // Cloud
    logPrintln("Start update cloud", DEBUG, __FUNCTION__); {
        GlobalDataManager::getInstance()->updateHeaderServer(linkedServer());
    }

    // Auto close face register
    logPrintln("Start update register", DEBUG, __FUNCTION__); {
        if (lastShowFaceRegisterTime > 0 && time - lastShowFaceRegisterTime > 60) {
            MainRouter::getInstance()->hideFaceRegister();
            lastShowFaceRegisterTime = 0;
        }
    }

    // Connect
    logPrintln("Start update connect", DEBUG, __FUNCTION__); {
        // dns
        static int resetDns = 1;
        static std::string dns1 = "";
        static std::string dns2 = "";
        if (resetDns++ > 0 && (dns1 != g_netDns1 || dns2 != g_netDns2)) {
            resetDns = 0;
            logPrintln("Reset dns " + g_netDns1 + " " + g_netDns2, INFO, __FUNCTION__);
            dns1 = g_netDns1;
            dns2 = g_netDns2;
#ifndef WIN32
            execCommandNoReturn("sh " + g_appWorkDir + "script/linux/reset_dns.sh " + g_netDns1 + " " + g_netDns2);
#endif
        }

        //wired
        static int resetDhcp = 1;
        static int netWiredDhcp = -1;
        static std::string netWiredIp;
        static std::string netWiredMask;
        static std::string netWiredGateway;
        if (resetDhcp++ > 0 && (netWiredDhcp != g_netWiredDhcp || netWiredIp != g_netWiredIp ||
                                netWiredMask != g_netWiredMask || netWiredGateway != g_netWiredGateway)) {
            resetDhcp = 0;
            netWiredDhcp = g_netWiredDhcp;
            netWiredIp = g_netWiredIp;
            netWiredMask = g_netWiredMask;
            netWiredGateway = g_netWiredGateway;
            logPrintln("Reset wired dhcp " + std::to_string(g_netWiredDhcp), INFO, __FUNCTION__);
            if (g_netWiredDhcp) {
#ifndef WIN32
                execCommandNoReturn("sh " + g_appWorkDir + "script/linux/reset_wired.sh dhcp");
#endif
            } else {
#ifndef WIN32
                std::ostringstream oss;
                oss << "sh " << g_appWorkDir + "script/linux/reset_wired.sh static " << g_netWiredIp << " ";
                oss << std::to_string(generalUtils::subnetMaskToCIDR(g_netWiredMask)) << " "
                        << g_netWiredGateway << " " << g_netDns1 << " " << g_netDns2;
                logPrintln("Reset wired static " + oss.str(), INFO, __FUNCTION__);
                execCommandNoReturn(oss.str());
#endif
            }
        }

        // wireless
        static int reconnectCount = 1;
        if (g_netWirelessEnable && reconnectCount++ > 0) {
            reconnectCount = 0;
            static string lastPasswd = g_netWirelessPasswd;
            static string lastSSid = g_netWirelessSsid;
            if (g_wirelessIp.empty() || lastSSid != g_netWirelessSsid || lastPasswd != g_netWirelessPasswd) {
                logPrintln("Connect to wifi ...", INFO, __FUNCTION__);
                lastSSid = g_netWirelessSsid;
                lastPasswd = g_netWirelessPasswd;
#ifndef WIN32
                execCommandNoReturn("sh " + g_appWorkDir + "script/linux/reset_wifi.sh on '" +
                            g_netWirelessSsid + "' '" + g_netWirelessPasswd + "'");
#endif
            }
        } else if (!g_netWirelessEnable && !g_wirelessIp.empty()) {
            logPrintln("Disconnect wifi ...", INFO, __FUNCTION__);
#ifndef WIN32
            execCommandNoReturn("sh " + g_appWorkDir + "script/linux/reset_wifi.sh off");
#endif
        }

        // 4g
        static int reconnectCountFourG = 3;
        if (g_netFourEnable && ++reconnectCountFourG > 3) {
            reconnectCountFourG = 0;
            if (g_fourGIp.empty()) {
                logPrintln("Connect to 4g ...", INFO, __FUNCTION__);
#ifndef WIN32
                execCommandNoReturn("sh " + g_appWorkDir + "script/linux/reset_4g.sh on");
#endif
            }
        } else if (!g_netFourEnable && !g_fourGIp.empty()) {
            logPrintln("Disconnect 4g ...", INFO, __FUNCTION__);
#ifndef WIN32
            execCommandNoReturn("sh " + g_appWorkDir + "script/linux/reset_4g.sh off");
#endif
        }
    }


    logPrintln("Start bar label update finish", DEBUG, __FUNCTION__);
}

// Every (5 * (taskIvCnt + executionTime)) sec
void checkTaskAndExecute() {
    static int count = 1;
    if (count++ < 5) return;
    count = 1;

    logPrintln("Start task execute", DEBUG, __FUNCTION__);
    const auto now = chrono::system_clock::now();
    const auto time = chrono::system_clock::to_time_t(now);
    static time_t lastTime = 0;
    if (time - lastTime > g_taskIvSec) {
        lastTime = time;
        static bool inTasking = false;
        if (!inTasking) {
            inTasking = true;
            static_cast<ThreadPool *>(g_mainThreadPool)->enqueue([] {
                checkTask();
                inTasking = false;
            });
        }
    }
}

// Every (4 * (taskIvCnt + executionTime)) sec
void gotoManagement() {
    static int count = 4;
    if (count++ < 4) return;
    count = 1;
    if (g_tryGoManagementCount >= 5) {
        g_tryGoManagementCount = 0;
        logPrintln("Start update page", DEBUG, __FUNCTION__);
        MainRouter::getInstance()->addPage(MAIN_PAGE_SETTING_LOGIN);
    } else {
        g_tryGoManagementCount = 0;
    }
}

// Every (taskIvCnt + executionTime) sec
void getNfcCode() {
    if (!g_allowCardOpen) {
        return;
    }
    const NfcCardData ret = getCardData();
    if (!ret.isExist) {
        return;
    }
    playWav(Di);
    logPrintln("Nfc card detected " + to_string(ret.cardType) + " " + ret.cardNo,
               INFO, __FUNCTION__);
    const auto cardInfo = cardRecognition(ret.cardNo);

    const auto now = chrono::system_clock::now();
    const auto currentTimeSec = chrono::system_clock::to_time_t(now);

    if (!cardInfo.userId.empty()) {
        if (!cardInfo.isEnable) {
            CameraFrame::getInstance()->negativeMessage();
            messageLabelSec = 1;
            playWav(Disabled);
            playWav(cardInfo.voiceTemplate);
        } else if (currentTimeSec < cardInfo.startTime || currentTimeSec > cardInfo.endTime) {
            CameraFrame::getInstance()->negativeMessage();
            messageLabelSec = 1;
            playWav(Expired);
            playWav(cardInfo.voiceTemplate);
        } else {
            OpenRecordInfo recordInfo = {};
            recordInfo.userId = cardInfo.userId;
            if (cardInfo.cardType == ICCard) {
                recordInfo.openMode = IcCardOpen;
            } else if (cardInfo.cardType == IdCard) {
                recordInfo.openMode = IdCardOpen;
            } else {
                recordInfo.openMode = 0;
            }
            recordInfo.openResult = 0;
            recordInfo.openTime = chrono::system_clock::to_time_t(chrono::system_clock::now());
            recordInfo.cardNo = cardInfo.cardNo;
            recordInfo.cardType = cardInfo.cardType;
            ScheduledTask::commonOpenDoor(recordInfo);
            playWav(AuthSuccess);
        }
    }
}

// Every (taskIvCnt + executionTime) sec
void doorAutoClose() {
    if (doorOpenSec <= 0) return;
    if (++doorOpenSec > g_autoCloseDoorSec) {
        logPrintln("Start close door", DEBUG, __FUNCTION__);
        closeDoor();
        doorOpenSec = -1;
    }
}

// Every (taskIvCnt + executionTime) sec
void messageLabelHide() {
    if (messageLabelSec <= 0) return;
    if (++messageLabelSec > 3) {
        logPrintln("Start hide Label", DEBUG, __FUNCTION__);
        CameraFrame::getInstance()->hideAllMessage();
        messageLabelSec = -1;
    }
}

void onceTaskBefore() {
    // Init Camera
    CameraFrame::getInstance()->start();

    // Init Face
    initFaceRecognition();

    // Login
    login();

    // Check latest version
    appUpdate();

    // Init Db
    initCardDB();
    initFaceDB();
    initOpenRecordDB();
    loadCardDb();
    loadFaceDb();
}


void onceTaskAfter() {
    // To home
    MainRouter::getInstance()->addPage(MAIN_PAGE_HOME);
}

void repeatOperation() {
    // Task updateUIMainComponentHeader
    updateUIMainComponentHeader();
    // Try go to hided management
    gotoManagement();
    // Try to get nfc code
    getNfcCode();
    // Try to get task list
    checkTaskAndExecute();
    // Auto close door
    doorAutoClose();
    // Auto hide message
    messageLabelHide();
    // Try to deal with persistent data
    updatePersistentData();
    // Upload app data
    uploadAppData();
    // Try to update common data
    updateCommonAppData();

    // ...
}

[[noreturn]] void taskExecutor(const chrono::milliseconds interval) {
    //std::this_thread::sleep_for(std::chrono::seconds(10));
    onceTaskBefore();
    repeatOperation();
    onceTaskAfter();

    while (true) {
        // Operation
        repeatOperation();
        // Interval
        this_thread::sleep_for(interval);
    }
}

void ScheduledTask::sendFaceRegRes(const FaceUserInfo &userInfo, const cv::Mat &frame, float confidence) {
    static time_t lastTime = 0;
    static bool lastPass = false;
    static int consecutiveFailCount = 0;
    static int consecutiveErrorCount = 0;
    const auto currentTime = chrono::system_clock::now();
    const auto currentTimeSec = chrono::system_clock::to_time_t(currentTime);

    logPrintln("For  user info " + to_string(confidence), DEBUG, __FUNCTION__);

    if (lastPass && currentTimeSec - lastTime < 5) {
        logPrintln("Already pass last", DEBUG, __FUNCTION__);
        return;
    }

    if (!userInfo.userId.empty()) {
        if (!userInfo.isEnable) {
            CameraFrame::getInstance()->negativeMessage();
            messageLabelSec = 1;
            playWav(Disabled);
            playWav(userInfo.voiceTemplate);
        } else if (currentTimeSec < userInfo.startTime || currentTimeSec > userInfo.endTime) {
            CameraFrame::getInstance()->negativeMessage();
            messageLabelSec = 1;
            playWav(Expired);
            playWav(userInfo.voiceTemplate);
        } else {
            ostringstream oss;
            oss << g_appWorkDir << "log-face/" <<
                    put_time(localtime(&currentTimeSec), "%Y-%m-%d-%H-%M-%S")
                    << "-" << userInfo.userId << ".jpg";
            imwrite(oss.str(), generalUtils::matCompress(frame));
            OpenRecordInfo recordInfo = {};
            recordInfo.userId = userInfo.userId;
            recordInfo.openMode = FaceOpen;
            recordInfo.openResult = 0;
            recordInfo.openTime = currentTimeSec;
            recordInfo.faceId = userInfo.faceId;
            commonOpenDoor(recordInfo, userInfo.userId, confidence);
            consecutiveFailCount = 0;
            consecutiveErrorCount = 0;
            playWav(AuthSuccess);
            playWav(userInfo.voiceTemplate);
            // todo save frame
            if (g_lightOnlyCheck) {
#ifndef WIN32
                closeLight();
#endif
            }
        }
        lastPass = true;
    } else {
        static time_t lastFailTime = 0;
        if (currentTimeSec - lastFailTime > g_faceRegIvSec) {
            if (currentTimeSec - lastFailTime < 10) {
                if (consecutiveFailCount == 0) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                } else if (consecutiveFailCount == 1) {
                    playWav(AuthFailFirst);
                } else {
                    ostringstream oss;
                    oss << g_appWorkDir << "log-face/" <<
                            put_time(localtime(&currentTimeSec), "%Y-%m-%d-%H-%M-%S") << "-Fail" << ".jpg";
                    imwrite(oss.str(), generalUtils::matCompress(frame));
                    CameraFrame::getInstance()->negativeMessage();
                    messageLabelSec = 1;
                    ++consecutiveErrorCount;
                    playWav(AuthFail);
                    if (consecutiveErrorCount >= g_faceRegCount) {
                        consecutiveErrorCount = 0;
                        MainRouter::getInstance()->showFaceRegister(frame);
                        lastShowFaceRegisterTime = currentTimeSec;
                    }
                }
                ++consecutiveFailCount;
                //todo 如果任务时间间隔大于1分钟 则异步调获取任务接口，防止刚刚下发
            } else {
                consecutiveFailCount = 0;
            }
            lastFailTime = currentTimeSec;
        }
        lastPass = false;
    }
    lastTime = currentTimeSec;
}

bool ScheduledTask::commonOpenDoor(const OpenRecordInfo &openRecordInfo, const std::string &userId,
                                   float confidence) {
    openDoor();
    doorOpenSec = 1;
    if (g_showConfUser) {
        const string preUserId = userId.substr(0, std::min(userId.size(), static_cast<size_t>(5)));
        std::ostringstream confidenceOss;
        confidenceOss << std::fixed << std::setprecision(3) << confidence;
        const string extraStr = preUserId + " " + confidenceOss.str();
        CameraFrame::getInstance()->positiveMessage(extraStr);
    } else {
        CameraFrame::getInstance()->positiveMessage();
    }
    messageLabelSec = 1;
    return insertOpenRecordDB(openRecordInfo);
}

ScheduledTask::ScheduledTask() {
    int taskIvCnt = 0;
    getProgramOptions(PRO_OPT_TASK_IN_CNT, &taskIvCnt);
    if (0 == taskIvCnt) {
        return;
    }
    taskFuture = std::async(std::launch::async, taskExecutor,
                            std::chrono::seconds(taskIvCnt));
}


ScheduledTask::~ScheduledTask() = default;
