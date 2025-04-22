#include "utils/scheduled_task.h"

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
    // todo 这里需要压缩，比较消耗时间，最好放到线程池中执行
    static int count = 1800;
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
                        fs::remove(iter->path());
                    }
                }
            }
        }
    }
    // Backup data
    {
    }
    // Delete system tmp file
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
    {
#ifdef WIN32
        const string wiredIp = execScript(g_appWorkDir + "script/win/get_wired_ip.ps1");
#else
        const string wiredIp = execScript(g_appWorkDir + "script/linux/get_wired_ip.sh");
#endif
        GlobalDataManager::getInstance()->updateHeaderWired(wiredIp);
    }

    // Wireless
    string wirelessIp; {
#ifdef WIN32
        wirelessIp = execScript(g_appWorkDir + "script/win/get_wireless_ip.ps1");
#else
         wirelessIp = execScript(g_appWorkDir + "script/linux/get_wireless_ip.sh");
#endif
        GlobalDataManager::getInstance()->updateHeaderWireless(wirelessIp);
    }

    // 4g
    string fourGIp; {
#ifndef WIN32
        fourGIp = execScript(g_appWorkDir + "script/linux/get_4g_ip.sh");
#endif
        GlobalDataManager::getInstance()->updateHeaderFourG(fourGIp);
    }

    // Cloud
    {
        GlobalDataManager::getInstance()->updateHeaderServer(linkedServer());
    }

    // Auto close face register
    {
        if (lastShowFaceRegisterTime > 0 && time - lastShowFaceRegisterTime > 60) {
            MainRouter::getInstance()->hideFaceRegister();
            lastShowFaceRegisterTime = 0;
        }
    }

    // Connect
    {
        static int reconnectCount = 1;
        // wireless
        if (g_netModel == 2 && reconnectCount++ > 0) {
            reconnectCount = 0;
            static string lastPasswd = g_wifiPasswd;
            static string lastSSid = g_wifiAccount;
            if (wirelessIp.empty() || lastSSid != g_wifiAccount || lastPasswd != g_wifiPasswd) {
                logPrintln("Connect to wifi ...", INFO, __FUNCTION__);
                lastSSid = g_wifiAccount;
                lastPasswd = g_wifiPasswd;
#ifndef WIN32
                execCommandNoReturn("sh " + g_appWorkDir + "script/linux/reset_wifi.sh on '" +
                            g_wifiAccount + "' '" + g_wifiPasswd + "'");
#endif
            }
        } else if (g_netModel != 2 && !wirelessIp.empty()) {
            logPrintln("Disconnect wifi ...", INFO, __FUNCTION__);
#ifndef WIN32
            execCommandNoReturn("sh " + g_appWorkDir + "script/linux/reset_wifi.sh off");
#endif
        }

        // 4g
        static int reconnectCountFourG = 3;
        if (g_netModel == 3 && ++reconnectCountFourG > 3) {
            reconnectCountFourG = 0;
            if (fourGIp.empty()) {
                logPrintln("Connect to 4g ...", INFO, __FUNCTION__);
#ifndef WIN32
                execCommandNoReturn("sh " + g_appWorkDir + "script/linux/reset_4g.sh on");
#endif
            }
        } else if (g_netModel != 3 && !fourGIp.empty()) {
            logPrintln("Disconnect 4g ...", INFO, __FUNCTION__);
#ifndef WIN32
            execCommandNoReturn("sh " + g_appWorkDir + "script/linux/reset_4g.sh off");
#endif
        }
    }
}

// Every (5 * (taskIvCnt + executionTime)) sec
void checkTaskAndExecute() {
    // todo 这里考虑扔到子线程里面去执行，否则会非常大延长executionTime的时间
    static int count = 1;
    if (count++ < 5) return;
    count = 1;

    const auto now = chrono::system_clock::now();
    const auto time = chrono::system_clock::to_time_t(now);
    static time_t lastTime = 0;
    if (time - lastTime > g_taskIvSec) {
        lastTime = time;
        checkTask();
    }
}

// Every (4 * (taskIvCnt + executionTime)) sec
void gotoManagement() {
    static int count = 4;
    if (count++ < 4) return;
    count = 1;
    if (g_tryGoManagementCount >= 5) {
        g_tryGoManagementCount = 0;
        MainRouter::getInstance()->addPage(MAIN_PAGE_SETTING_LOGIN);
    } else {
        g_tryGoManagementCount = 0;
    }
}

// Every (taskIvCnt + executionTime) sec
void getNfcCode() {
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
    if (++doorOpenSec > 5) {
        closeDoor();
        doorOpenSec = -1;
    }
}

// Every (taskIvCnt + executionTime) sec
void messageLabelHide() {
    if (messageLabelSec <= 0) return;
    if (++messageLabelSec > 3) {
        CameraFrame::getInstance()->hideAllMessage();
        messageLabelSec = -1;
    }
}

void faceTest() {
    try {
        const std::string directory = g_appWorkDir + "test/";
        if (!fs::exists(directory) || !fs::is_directory(directory)) {
            logPrintln("Test directory not exist", WARN, __FUNCTION__);
        }
        for (const auto &entry: fs::recursive_directory_iterator(directory)) {
            if (is_regular_file(entry.path())) {
                string extension = entry.path().extension().string();
                transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
                if (extension == ".jpg") {
                    const auto absolutePath = absolute(entry.path()).string();
                    logPrintln("Test image start" + absolutePath, INFO, __FUNCTION__);
                    const auto imageTest = cv::imread(absolutePath);
                    faceRecognition(imageTest, cv::Rect(0, 0, imageTest.cols, imageTest.rows));
                }
            }
        }
    } catch (const exception &e) {
        ostringstream errMsg;
        errMsg << e.what();
        logPrintln("Test face error" + errMsg.str(), airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }
}

void onceTaskBefore() {
    // Init Camera
    CameraFrame::getInstance()->start();

    // Init Face
    initFaceRecognition();

    // Login
    login();

    // Init Db
    initCardDB();
    initFaceDB();
    initOpenRecordDB();
    loadCardDb();
    loadFaceDb();




    // Face test
    faceTest();
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

void ScheduledTask::sendFaceRegRes(const FaceUserInfo &userInfo, const cv::Mat &frame) {
    static auto lastTime = chrono::system_clock::from_time_t(0);
    static bool lastPass = false;
    static int consecutiveFailCount = 0;
    const auto currentTime = chrono::system_clock::now();
    const auto currentTimeSec = chrono::system_clock::to_time_t(currentTime);

    if (lastPass && chrono::duration_cast<std::chrono::seconds>(currentTime - lastTime).count() < 5) {
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
            recordInfo.openTime = chrono::system_clock::to_time_t(currentTime);
            recordInfo.faceId = userInfo.faceId;
            commonOpenDoor(recordInfo);
            consecutiveFailCount = 0;
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
        static auto lastFailTime = chrono::system_clock::from_time_t(0);
        if ((currentTime - lastFailTime).count() > 3) {
            if (chrono::duration_cast<std::chrono::seconds>(currentTime - lastFailTime).count() < 5) {
                ostringstream oss;
                oss << g_appWorkDir << "log-face/" <<
                        put_time(localtime(&currentTimeSec), "%Y-%m-%d-%H-%M-%S") << "-Fail" << ".jpg";
                imwrite(oss.str(), generalUtils::matCompress(frame));
                CameraFrame::getInstance()->negativeMessage();
                messageLabelSec = 1;
                ++consecutiveFailCount;
                playWav(AuthFail);
                if (consecutiveFailCount >= g_faceRegCount) {
                    consecutiveFailCount = 0;
                    MainRouter::getInstance()->showFaceRegister(frame);
                    lastShowFaceRegisterTime = currentTimeSec;
                }
                //todo 如果任务时间间隔大于1分钟 则异步调获取任务接口，防止刚刚下发
            } else {
                consecutiveFailCount = 0;
                playWav(AuthFailFirst);
            }
            lastFailTime = currentTime;
        }
        lastPass = false;
    }
    lastTime = currentTime;
}

bool ScheduledTask::commonOpenDoor(const OpenRecordInfo &openRecordInfo) {
    openDoor();
    doorOpenSec = 1;
    CameraFrame::getInstance()->positiveMessage();
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
