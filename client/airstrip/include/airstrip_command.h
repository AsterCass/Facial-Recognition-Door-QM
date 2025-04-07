#ifndef AIRSTRIP_COMMAND_H
#define AIRSTRIP_COMMAND_H
#include <string>

namespace airstrip {
    std::string execScript(const std::string &address);

    std::string execCommand(const std::string &cmd);

    void execCommandNoReturn(const std::string &cmd);
}

#endif //AIRSTRIP_COMMAND_H
