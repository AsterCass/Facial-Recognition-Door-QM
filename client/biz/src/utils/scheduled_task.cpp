#include "utils/scheduled_task.h"
#include <thread>
#include "utils/general_utils.h"
#include "config/config.h"
#include "airstrip_log.h"



using namespace std;
using namespace airstrip;

void task1() {
    logPrintln(INFO, __FUNCTION__, "Task 1 Finish");
}

void task2() {
    logPrintln(INFO, __FUNCTION__, "Task 2 Finish");
}


[[noreturn]] void taskExecutor(const chrono::milliseconds interval) {
    while (true) {
        // Task 1
        task1();
        // Task 2
        task2();
        // Interval
        this_thread::sleep_for(interval);
    }
}

ScheduledTask::ScheduledTask() {
    taskFuture = std::async(std::launch::async, taskExecutor,
                            std::chrono::seconds(GENERAL_TASK_INTERVAL_SECONDS));
}


ScheduledTask::~ScheduledTask() = default;
