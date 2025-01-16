#include <QApplication>
#include "utils/general_utils.h"

int main(int argc, char *argv[]) {
    // Input arg
    if (const auto ret = asterGeneralUtils::loadArguments(argc, argv)) {
        return ret;
    }

    // Init application
    QApplication app(argc, argv);

    asterGeneralUtils::logPrintln(DEBUG, __FUNCTION__, "Hello World!");

    return QApplication::exec();
}
