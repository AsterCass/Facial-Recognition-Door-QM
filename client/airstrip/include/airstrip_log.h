#ifndef AIRSTRIP_LOG_H
#define AIRSTRIP_LOG_H

#include <string>
#include "airstrip_enums.h"

namespace airstrip {
    void logPrintln(const std::string &message, LogLevel level = INFO, const std::string &functionName = "");
}

#endif // AIRSTRIP_LOG_H
