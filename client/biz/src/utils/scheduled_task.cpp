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

int doorOpenSec = 0;
int messageLabelSec = 0;

using namespace std;
using namespace airstrip;
using namespace boost::gregorian;

// Every (30 * 60 * (taskIvCnt + executionTime)) sec
void updatePersistentData() {
    static int count = 1800;
    if (count++ < 1800) return;
    count = 1;
    // Every 22 hour
    static auto lastTime = chrono::system_clock::from_time_t(0);
    const auto currentTime = chrono::system_clock::now();
    if (chrono::duration_cast<std::chrono::hours>(currentTime - lastTime).count() < 22) {
        return;
    }
    // Operation
    lastTime = currentTime;
    logPrintln("Update persistent data", INFO, __FUNCTION__);
    // Delete db data
    deleteYearRecordDB();
    // Delete log
    // Delete face file
    // Backup data
    // Delete system tmp file
}


// Every (30 * 60 * (taskIvCnt + executionTime)) sec
void uploadAppData() {
    static int count = 1800;
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
    MainComponentHeader::getInstance()->updateTimeText(string(oss.str()));

    if (g_appWorkDir.empty()) {
        return;
    }

    // Wired
    {
        static string currentWiredIp;
#ifdef WIN32
        const string wiredIp = execScript(g_appWorkDir + "script/win/get_wired_ip.ps1");
#else
        const string wiredIp = execScript(g_appWorkDir + "script/linux/get_wired_ip.sh");
#endif
        if (wiredIp != currentWiredIp) {
            currentWiredIp = wiredIp;
            MainComponentHeader::getInstance()->updateWiredStatus(!wiredIp.empty());
        }
    }

    // Wireless
    {
        static string currentWirelessIp;
#ifdef WIN32
        const string wirelessIp = execScript(g_appWorkDir + "script/win/get_wireless_ip.ps1");
#else
        const string wirelessIp = execScript(g_appWorkDir + "script/linux/get_wireless_ip.sh");
#endif
        if (wirelessIp != currentWirelessIp) {
            currentWirelessIp = wirelessIp;
            MainComponentHeader::getInstance()->updateWirelessStatus(!wirelessIp.empty());
        }
    }

    // 4g
    {
        static string current4gIp;
#ifdef WIN32
        const string fourGIp;
#else
        const string fourGIp = execScript(g_appWorkDir + "script/linux/get_4g_ip.sh");
#endif
        if (fourGIp != current4gIp) {
            current4gIp = fourGIp;
            MainComponentHeader::getInstance()->update4GStatus(!fourGIp.empty());
        }
    }

    // Cloud
    {
        MainComponentHeader::getInstance()->updateServerStatus(linkedServer());
    }
}

// Every (5 * (taskIvCnt + executionTime)) sec
void checkTaskAndExecute() {
    // todo 这里考虑扔到子线程里面去执行，否则会非常大延长executionTime的时间
    static int count = 1;
    if (count++ < 5) return;
    count = 1;
    checkTask();
}

// Every (5 * (taskIvCnt + executionTime)) sec
void gotoManagement() {
    static int count = 5;
    if (count++ < 5) return;
    count = 1;
    if (g_tryGoManagementCount >= 5) {
        g_tryGoManagementCount = 0;
        logPrintln("Go to management...", INFO, __FUNCTION__);
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
    if (!cardInfo.userId.empty()) {
        OpenRecordInfo recordInfo = {};
        recordInfo.userId = cardInfo.userId;
        recordInfo.openMode = IcCardOpen;
        recordInfo.openResult = 0;
        recordInfo.openTime = chrono::system_clock::to_time_t(chrono::system_clock::now());
        recordInfo.cardNo = cardInfo.cardNo;
        recordInfo.cardType = cardInfo.cardType;
        ScheduledTask::commonOpenDoor(recordInfo);
        playWav(AuthSuccess);
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
}


void onceTaskAfter() {
    // To home
    if (g_stackedWidget != nullptr) {
        g_stackedWidget->setCurrentIndex(MAIN_PAGE_HOME);
    }
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

    // ...
}

[[noreturn]] void taskExecutor(const chrono::milliseconds interval) {
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

void ScheduledTask::sendFaceRegRes(const FaceUserInfo &userInfo) {
    static auto lastTime = chrono::system_clock::from_time_t(0);
    static bool lastPass = false;
    const auto currentTime = chrono::system_clock::now();

    if (lastPass && chrono::duration_cast<std::chrono::seconds>(currentTime - lastTime).count() < 5) {
        logPrintln("Already pass last", DEBUG, __FUNCTION__);
        return;
    }

    if (!userInfo.userId.empty()) {
        OpenRecordInfo recordInfo{};
        recordInfo.userId = userInfo.userId;
        recordInfo.openMode = FaceOpen;
        recordInfo.openResult = 0;
        recordInfo.openTime = chrono::system_clock::to_time_t(currentTime);
        recordInfo.faceId = userInfo.faceId;
        commonOpenDoor(recordInfo);
        playWav(AuthSuccess);

        lastPass = true;
    } else {
        static auto lastFailTime = chrono::system_clock::from_time_t(0);
        if ((currentTime - lastFailTime).count() > 2) {
            if (chrono::duration_cast<std::chrono::seconds>(currentTime - lastFailTime).count() < 5) {
                CameraFrame::getInstance()->negativeMessage();
                messageLabelSec = 1;
                playWav(AuthFail);
            } else {
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
