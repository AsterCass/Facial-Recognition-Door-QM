#include <QApplication>
#include "ui/main_router.h"
#include "utils/general_utils.h"

int main(int argc, char *argv[]) {
    // Input arg
    if (const auto ret = generalUtils::loadArguments(argc, argv)) {
        return ret;
    }

    // Init application
    QApplication app(argc, argv);

    // Page router
    const auto router = MainRouter::getInstance();
    router->show();

    // Finish
    generalUtils::logPrintln(INFO, __FUNCTION__, "Application started");

    return QApplication::exec();
}
