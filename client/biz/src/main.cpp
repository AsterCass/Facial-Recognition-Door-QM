#include <QApplication>
#include <airstrip_log.h>
#include <airstrip_program_options.h>
#include "utils/scheduled_task.h"
#include "ui/main_router.h"

int main(int argc, char *argv[]) {
    // // Input arg
    // if (const auto ret = generalUtils::loadArguments(argc, argv)) {
    //     return ret;
    // }

    // Input args

    airstrip::AirstripProgramOptions optSetting;
    airstrip::enableProgramOptions(optSetting, argc, argv);

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
    airstrip::logPrintln(INFO, __FUNCTION__, "Application started");

    return QApplication::exec();
}
