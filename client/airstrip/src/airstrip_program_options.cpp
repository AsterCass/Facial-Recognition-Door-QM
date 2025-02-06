#include "airstrip_program_options.h"

#include <airstrip_configs.h>
#include <airstrip_enums.h>
#include <airstrip_log.h>
#include <boost/program_options.hpp>
#include <iostream>

namespace po = boost::program_options;
using namespace std;

namespace airstrip {
    std::map<std::string, std::vector<std::string> > programOptions;

    void enableProgramOptions(
        const AirstripProgramOptions &optSetting, int argc, char **argv) {
        try {
            // Build
            po::options_description desc("Allowed options");
            desc.add_options()("help,h", "Produce help message");
            desc.add_options()("logLevel", po::value<string>(),
                               "Set log output level, 0-5, 0 is TRACE, 5 is CRITICAL. "
                               "\nDefault value is 1 means DEBUG");
            desc.add_options()("logPath", po::value<string>(),
                               "Set log file output directory, if logPath is empty output to console."
                               "\nDefault value is ''");
            for (auto &opt: optSetting.options) {
                if (opt.second.needInput) {
                    desc.add_options()(
                        opt.first.c_str(),
                        po::value<vector<string> >(),
                        opt.second.optionDesc.c_str());
                } else {
                    desc.add_options()(
                        opt.first.c_str(),
                        opt.second.optionDesc.c_str());
                }
            }
            po::variables_map vm;
            store(parse_command_line(argc, argv, desc), vm);
            notify(vm);

            // Read
            if (vm.count("help")) {
                cout << desc << endl;
                exit(0);
            }

            if (vm.count("logLevel")) {
                try {
                    logPrintLevel = stoi(vm["logLevel"].as<string>());
                } catch (...) {
                    logPrintLevel = DEBUG;
                }
            } else {
                logPrintLevel = DEBUG;
            }
            programOptions["logLevel"] = {std::to_string(logPrintLevel)};

            if (vm.count("logPath")) {
                const auto inputLogPath = vm["logPath"].as<string>();
                if (!inputLogPath.empty() && inputLogPath.back() != '/') {
                    logPrintPath = inputLogPath + "/";
                } else {
                    logPrintPath = inputLogPath;
                }
            } else {
                logPrintPath = "";
            }
            programOptions["logPrintPath"] = {logPrintPath};

            bool haveInput = false;

            for (auto &opt: optSetting.options) {
                const size_t pos = opt.first.find(",") == string::npos ? opt.first.size() : opt.first.find(",");
                auto optStd = opt.first.substr(0, pos);
                if (vm.count(optStd)) {
                    haveInput = true;
                    if (!opt.second.needContinue) {
                        cout << opt.second.outputValue << endl;
                        exit(0);
                    }
                    if (opt.second.needInput) {
                        programOptions[optStd] = vm[optStd].as<vector<string> >();
                    }
                } else {
                    if (opt.second.needInput) {
                        programOptions[optStd] = opt.second.defaultValue;
                    }
                }
            }
            if (!haveInput && !optSetting.allowEmptyOpt) {
                cout << optSetting.emptyOptError << endl;
                exit(0);
            }

            // Print
            for (auto &loadedOpt: programOptions) {
                std::ostringstream values;
                for (const auto &str: loadedOpt.second) {
                    values << str << " ";
                }
                const size_t pos = loadedOpt.first.find(",") == string::npos
                                       ? loadedOpt.first.size()
                                       : loadedOpt.first.find(",");
                auto optStd = loadedOpt.first.substr(0, pos);
                logPrintln(INFO, __FUNCTION__,
                           "Loaded " + optStd + " = " + values.str());
            }
        } catch (const exception &e) {
            cout << "Failed to parse options: " << e.what() << endl;
        }
    }
}
