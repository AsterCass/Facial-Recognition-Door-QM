#ifndef AIRSTRIP_LOG_H
#define AIRSTRIP_LOG_H

#include <string>
#include "airstrip_enums.h"

namespace airstrip {


    void logPrintln(LogLevel level, const std::string &functionName, const std::string &message);


}

#endif // AIRSTRIP_LOG_H