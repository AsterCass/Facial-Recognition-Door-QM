#include <QApplication>
#include "config/config.h"
#include "utils/thread_pool.h"
#include "ui/main_router.h"
#include "utils/general_utils.h"


void printMessage(const std::string &message) {
}

int main(int argc, char *argv[]) {
    // Input arg
    if (const auto ret = generalUtils::loadArguments(argc, argv)) {
        return ret;
    }

    // Init application
    QApplication app(argc, argv);

    // Init thread poll
    // mainThreadPool = ThreadPool::getInstance(3);
    // for (int i = 0; i < 10; ++i) {
    //     static_cast<ThreadPool *>(mainThreadPool)->enqueue([i] {
    //         std::this_thread::sleep_for(std::chrono::seconds(1));
    //         generalUtils::logPrintln(INFO, "TASK", std::to_string(i));
    //     });
    // }

    // Page router
    const auto router = MainRouter::getInstance();
    router->show();

    // Finish
    generalUtils::logPrintln(INFO, __FUNCTION__, "Application started");

    return QApplication::exec();
}
