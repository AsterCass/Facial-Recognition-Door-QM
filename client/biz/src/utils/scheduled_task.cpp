#include "utils/scheduled_task.h"

#include <iomanip>
#include <thread>
#include <sstream>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "airstrip_log.h"
#include "config/config.h"
#include "airstrip_program_options.h"
#include "ui/components/main_component_header.h"

using namespace std;
using namespace airstrip;
using namespace boost::gregorian;;

// Every (2 * taskIvCnt + executionTime) sec
void updateUIMainComponentHeader() {
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
}


[[noreturn]] void taskExecutor(const chrono::milliseconds interval) {
    while (true) {
        // Task updateUIMainComponentHeader
        updateUIMainComponentHeader();
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
