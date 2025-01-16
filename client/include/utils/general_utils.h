#ifndef GENERAL_UTILS_H
#define GENERAL_UTILS_H

#include <string>
#include "enums/general_enums.h"


namespace asterGeneralUtils {
    // Parse input arguments
    int loadArguments(int argc, char **argv);

    // Println log
    void logPrintln(LogLevel level, const std::string &functionName, const std::string &message);
}


#endif //GENERAL_UTILS_H
