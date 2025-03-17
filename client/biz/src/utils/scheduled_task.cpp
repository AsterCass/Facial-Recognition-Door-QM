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
#include "enums/general_enums.h"
#include "nfc/nfc_tool.h"
#include "ui/main_router.h"
#include "ui/components/main_component_header.h"
#include "utils/card_recognition.h"
#include "utils/face_recognition.h"

int doorOpenSec = 0;

using namespace std;
using namespace airstrip;
using namespace boost::gregorian;;

// Every (10 * (taskIvCnt + executionTime)) sec
void updateUIMainComponentHeader(const std::string &appWorkDir) {
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

    if (appWorkDir.empty()) {
        return;
    }

    // Wired
    {
        static string currentWiredIp;
#ifdef WIN32
        const string wiredIp = execScript(appWorkDir + "script/win/get_wired_ip.ps1");
#else
        const string wiredIp = execScript(appWorkDir + "script/linux/get_wired_ip.sh");
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
        const string wirelessIp = execScript(appWorkDir + "script/win/get_wireless_ip.ps1");
#else
        const string wirelessIp = execScript(appWorkDir + "script/linux/get_wireless_ip.sh");
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
        const string fourGIp = execScript(appWorkDir + "script/linux/get_4g_ip.sh");
#endif
        if (fourGIp != current4gIp) {
            current4gIp = fourGIp;
            MainComponentHeader::getInstance()->update4GStatus(!fourGIp.empty());
        }
    }

    // Cloud
    {
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
        openDoor();
        doorOpenSec = 1;
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
void getFaceRegReport() {
    static int facePassIv = 5;
    if (facePassIv++ < 5) return;

    const auto res = reportFaceRecognition();
    if (res.faceId < 0) {
        playWav(AuthFail);
    } else if (res.faceId > 0) {
        openDoor();
        doorOpenSec = 1;
        facePassIv = 0;
        playWav(AuthSuccess);
    } else {
    }
}


void onceTaskBefore() {
    static int count = 1;
    if (count > 1) return;
    ++count;

    // Init Camera
    CameraFrame::getInstance()->start();

    // Init Face
    initFaceRecognition();

    // Login
    login();

    // Init Db
    initCardDB();
    initFaceDB();
    loadCardDb();
    loadFaceDb();
}


void onceTaskAfter() {
    static int count = 1;
    if (count > 1) return;
    ++count;

    // To home
    if (g_stackedWidget != nullptr) {
        g_stackedWidget->setCurrentIndex(MAIN_PAGE_HOME);
    } else {
        --count;
    }
}

[[noreturn]] void taskExecutor(const chrono::milliseconds interval) {
    std::string appWorkDir;
    getProgramOptions(PRO_OPT_APP_WORK_DIR, &appWorkDir);

    while (true) {
        // Once Task
        onceTaskBefore();
        // Task updateUIMainComponentHeader
        updateUIMainComponentHeader(appWorkDir);
        // Try go to hided management
        gotoManagement();
        // Try to get nfc code
        getNfcCode();
        // Try to get task list
        checkTaskAndExecute();
        // Try to get face
        getFaceRegReport();
        // Auto close door
        doorAutoClose();

        //...

        // Once Task
        onceTaskAfter();

        // Interval
        this_thread::sleep_for(interval);
    }
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
