#ifndef AIRSTRIP_PROGRAM_OPTIONS_H
#define AIRSTRIP_PROGRAM_OPTIONS_H

#include <string>
#include <map>

namespace airstrip {

    typedef struct AirstripProgramOptionDetail {
      bool needContinue = true; // Is need to run program, for example --version, --help isn't
      bool needInput = false; // Is need param
      bool inputMultiple = false; // Only for needInput equals true, Is had multiple input value
      std::string optionDesc;
      std::string outputValue; // Only for needContinue equals false, kill program and output this value
    } AirstripProgramOptionDetail;

    typedef struct AirstripProgramOptions {
      bool allowEmptyOpt = true;
      std::string emptyOptError = "Please enter options"; // Only for allowEmptyOpt equals false
      std::map<std::string, AirstripProgramOptionDetail> options; // Opts and their property
    } AirstripProgramOptions;

    std::map<std::string, std::string> enableProgramOptions(
        const AirstripProgramOptions& optSetting, int argc, char **argv);

}

#endif // AIRSTRIP_PROGRAM_OPTIONS_H