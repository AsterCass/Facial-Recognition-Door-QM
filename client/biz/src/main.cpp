#include <QApplication>
#include <airstrip_log.h>
#include <airstrip_program_options.h>
#include <enums/general_enums.h>
#include "utils/scheduled_task.h"
#include "ui/main_router.h"
#include "config/config.h"=

int main(int argc, char *argv[]) {
    // Input args
    airstrip::AirstripProgramOptions optSetting; {
        // Version
        airstrip::AirstripProgramOptionDetail version;
        version.needContinue = false;
        version.needInput = false;
        version.optionDesc = "Produce version";
        version.outputValue = APP_VERSION;
        version.valueType = STRING;
        optSetting.options[std::string(PRO_OPT_VERSION) + ",v"] = version;
        // Env
        airstrip::AirstripProgramOptionDetail env;
        env.needContinue = true;
        env.needInput = true;
        env.optionDesc = "Set produce environment, 0 or 1, 1 means dev, 0 means prod. \nDefault values is 1";
        env.defaultValue = {std::to_string(DEV)};
        env.valueType = INTEGER;
        optSetting.options[std::string(PRO_OPT_ENV)] = env;
        // Task interval Count
        airstrip::AirstripProgramOptionDetail taskIvCnt;
        taskIvCnt.needContinue = true;
        taskIvCnt.needInput = true;
        taskIvCnt.optionDesc = "Set common task interval sec count. \nDefault values is 5";
        taskIvCnt.defaultValue = {std::to_string(5)};
        taskIvCnt.valueType = INTEGER;
        optSetting.options[std::string(PRO_OPT_TASK_IN_CNT)] = taskIvCnt;
    }
    enableProgramOptions(optSetting, argc, argv);

    // Init application
    QApplication app(argc, argv);

    // Init thread poll
    // mainThreadPool = airstrip::ThreadPool::getInstance(3);
    // for (int i = 0; i < 10; ++i) {
    //     static_cast<airstrip::ThreadPool *>(mainThreadPool)->enqueue([i] {
    //         std::this_thread::sleep_for(std::chrono::seconds(1));
    //         airstrip::logPrintln(INFO, "TASK", std::to_string(i));
    //     });
    // }

    // Init Schedule task
    ScheduledTask::getInstance();

    // Page router
    const auto router = MainRouter::getInstance();
    router->show();

    // Finish
    airstrip::logPrintln("Application started", INFO, __FUNCTION__);

    return QApplication::exec();
}
