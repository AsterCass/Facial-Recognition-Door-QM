#ifndef AIRSTRIP_PROGRAM_OPTIONS_H
#define AIRSTRIP_PROGRAM_OPTIONS_H

#include <airstrip_enums.h>
#include <string>
#include <map>
#include <vector>

namespace airstrip {
    typedef struct AirstripProgramOptionLoadedOption {
        std::vector<std::string> originData;
        ProgramOptionInputValueType dataType = STRING;
    } AirstripProgramOptionLoadedOption;

    typedef struct AirstripProgramOptionDetail {
        bool needContinue = true; // Is need to run program, for example --version, --help isn't
        bool needInput = false; // Is need param
        std::string optionDesc;
        std::string outputValue; // Only for needContinue equals false, kill program and output this value
        ProgramOptionInputValueType valueType = STRING; // Only for needInput equals true
        std::vector<std::string> defaultValue; // Only for needInput equals true
    } AirstripProgramOptionDetail;

    typedef struct AirstripProgramOptions {
        bool allowEmptyOpt = true;
        std::string emptyOptError = "Please enter options"; // Only for allowEmptyOpt equals false
        std::map<std::string, AirstripProgramOptionDetail> options; // Opts and their property
    } AirstripProgramOptions;

    void enableProgramOptions(const AirstripProgramOptions &optSetting, int argc, char **argv);

    bool getProgramOptions(const std::string &optionName, void *optionValue);
}

#endif // AIRSTRIP_PROGRAM_OPTIONS_H
