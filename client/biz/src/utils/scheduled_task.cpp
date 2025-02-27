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
#include "ui/components/main_component_header.h"

using namespace std;
using namespace airstrip;
using namespace boost::gregorian;;

// Every (2 * taskIvCnt + executionTime) sec
void updateUIMainComponentHeader(const std::string &appWorkDir) {
    static int count = 2;
    if (count++ < 2) return;
    count = 1;
    const auto now = chrono::system_clock::now();
    const auto time = chrono::system_clock::to_time_t(now);
    const auto today = day_clock::local_day();
    const auto day = today.day_of_week();
    const auto weekStr = CHINESE_WEEK[day];

    ostringstream oss;
    oss << put_time(localtime(&time), "%Y.%m.%d %H:%M ") << weekStr;
    MainComponentHeader::getInstance()->updateTimeText(string(oss.str()));

    if (appWorkDir.size() <= 0) {
        return;
    }

    // Wired
    {
        static string currentWiredIp = "";
#ifdef WIN32
        const string wiredIp = execScript(appWorkDir + "script/win/get_wired_ip.ps1");
#else
        const string wirelessIp = execScript(appWorkDir + "script/linux/get_wired_ip.sh");
#endif
        if (wiredIp != currentWiredIp) {
            currentWiredIp = wiredIp;
            MainComponentHeader::getInstance()->updateWiredStatus(wiredIp.size() > 0);
        }
    }

    // Wireless
    {
        static string currentWirelessIp = "";
#ifdef WIN32
        const string wirelessIp = execScript(appWorkDir + "script/win/get_wireless_ip.ps1");
#else
        const string wirelessIp = execScript(appWorkDir + "script/linux/get_wireless_ip.sh");
#endif
        if (wirelessIp != currentWirelessIp) {
            currentWirelessIp = wirelessIp;
            MainComponentHeader::getInstance()->updateWirelessStatus(wirelessIp.size() > 0);
        }
    }

    // 4g
    {
        static string current4gIp = "";
#ifdef WIN32
        const string fourGIp = "";
#else
        const string wirelessIp = execScript(appWorkDir + "script/linux/get_4g_ip.sh");
#endif
        if (fourGIp != current4gIp) {
            current4gIp = fourGIp;
            MainComponentHeader::getInstance()->update4GStatus(fourGIp.size() > 0);
        }
    }

    // Cloud
    {
    }
}

void gotoManagement() {
    if (tryGoManagementCount >= 5) {
        tryGoManagementCount = 0;
        logPrintln("Go to management...", INFO, __FUNCTION__);
    } else {
        tryGoManagementCount = 0;
    }
}


[[noreturn]] void taskExecutor(const chrono::milliseconds interval) {
    std::string appWorkDir;
    getProgramOptions(PRO_OPT_APP_WORK_DIR, &appWorkDir);

    while (true) {
        // Task updateUIMainComponentHeader
        updateUIMainComponentHeader(appWorkDir);
        // Try go to hided management
        gotoManagement();
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
