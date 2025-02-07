#include "airstrip_program_options.h"

#include <airstrip_configs.h>
#include <airstrip_enums.h>
#include <airstrip_log.h>
#include <boost/program_options.hpp>
#include <iostream>

namespace po = boost::program_options;
using namespace std;

namespace airstrip {
    map<string, AirstripProgramOptionLoadedOption> programOptions;

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
            desc.add_options()("logReDays", po::value<string>(),
                               "Set log files retention days, only work when logPath isn't empty"
                               "\nDefault value is 10");
            for (auto &opt: optSetting.options) {
                if (opt.second.needInput) {
                    desc.add_options()(
                        opt.first.c_str(),
                        po::value<vector<string> >()->multitoken(),
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
            {
                if (vm.count("help")) {
                    cout << desc << endl;
                    exit(0);
                }
            }
            // Log level
            {
                if (vm.count("logLevel")) {
                    try {
                        logPrintLevel = stoi(vm["logLevel"].as<string>());
                    } catch (...) {
                        logPrintLevel = DEBUG;
                    }
                } else {
                    logPrintLevel = DEBUG;
                }
                programOptions["logLevel"].dataType = INTEGER;
                programOptions["logLevel"].originData = {to_string(logPrintLevel)};
            }
            // Log path
            {
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
                programOptions["logLevel"].dataType = STRING;
                programOptions["logPrintPath"].originData = {logPrintPath};
            }
            // Log remain
            {
                if (vm.count("logReDays")) {
                    try {
                        logReDays = stoi(vm["logReDays"].as<string>());
                    } catch (...) {
                        logReDays = 10;
                    }
                } else {
                    logReDays = 10;
                }
                programOptions["logReDays"].dataType = INTEGER;
                programOptions["logReDays"].originData = {to_string(logReDays)};
            }

            // User custom
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
                        programOptions[optStd].dataType = opt.second.valueType;
                        programOptions[optStd].originData = vm[optStd].as<vector<string> >();
                    }
                } else {
                    if (opt.second.needInput) {
                        programOptions[optStd].dataType = opt.second.valueType;
                        programOptions[optStd].originData = opt.second.defaultValue;
                    }
                }
            }
            if (!haveInput && !optSetting.allowEmptyOpt) {
                cout << optSetting.emptyOptError << endl;
                exit(0);
            }

            // Print and Check
            for (const auto &loadedOpt: programOptions) {
                string outputData;
                bool getOptionRet;
                switch (loadedOpt.second.dataType) {
                    case STRING: {
                        string data;
                        getOptionRet = getProgramOptions(loadedOpt.first, &data);
                        outputData = data;
                        break;
                    }
                    case STRING_VECTOR: {
                        vector<string> data;
                        getOptionRet = getProgramOptions(loadedOpt.first, &data);
                        ostringstream values;
                        for (const auto &str: data) {
                            values << str << " ";
                        }
                        outputData = values.str();
                        break;
                    }
                    case INTEGER: {
                        int data;
                        getOptionRet = getProgramOptions(loadedOpt.first, &data);
                        outputData = to_string(data);
                        break;
                    }
                    case INTEGER_VECTOR: {
                        vector<int> data;
                        getOptionRet = getProgramOptions(loadedOpt.first, &data);
                        ostringstream values;
                        for (const auto &str: data) {
                            values << str << " ";
                        }
                        outputData = values.str();
                        break;
                    }
                    case DOUBLE: {
                        double data;
                        getOptionRet = getProgramOptions(loadedOpt.first, &data);
                        outputData = to_string(data);
                        break;
                    }
                    case DOUBLE_VECTOR: {
                        vector<double> data;
                        getOptionRet = getProgramOptions(loadedOpt.first, &data);
                        ostringstream values;
                        for (const auto &str: data) {
                            values << str << " ";
                        }
                        outputData = values.str();
                        break;
                    }
                }
                if (getOptionRet) {
                    logPrintln("Loaded " + loadedOpt.first + " = " + outputData,
                               INFO, __FUNCTION__);
                } else {
                    logPrintln("Load " + loadedOpt.first + " error, please check input",
                               CRITICAL, __FUNCTION__);
                    exit(1);
                }
            }
        } catch (const exception &e) {
            cout << "Failed to parse options: " << e.what() << endl;
        }
    }

    bool getProgramOptions(const string &optionName, void *optionValue) {
        if (programOptions.find(optionName) == programOptions.end()) {
            return false;
        }
        const auto loadedOpt = programOptions[optionName];

        try {
            switch (loadedOpt.dataType) {
                case STRING: {
                    auto *data = static_cast<string *>(optionValue);
                    *data = loadedOpt.originData.at(0);
                    break;
                }
                case STRING_VECTOR: {
                    auto *data = static_cast<vector<string> *>(optionValue);
                    for (const auto &cell: loadedOpt.originData) {
                        data->push_back(cell);
                    }
                    break;
                }
                case INTEGER: {
                    auto *data = static_cast<int *>(optionValue);
                    *data = stoi(loadedOpt.originData.at(0));
                    break;
                }
                case INTEGER_VECTOR: {
                    auto *data = static_cast<vector<int> *>(optionValue);
                    for (const auto &cell: loadedOpt.originData) {
                        data->push_back(stoi(cell));
                    }
                    break;
                }
                case DOUBLE: {
                    auto *data = static_cast<double *>(optionValue);
                    *data = stod(loadedOpt.originData.at(0));
                    break;
                }
                case DOUBLE_VECTOR: {
                    auto *data = static_cast<vector<double> *>(optionValue);
                    for (const auto &cell: loadedOpt.originData) {
                        data->push_back(stod(cell));
                    }
                    break;
                }
                default:
                    return false;
            }
        } catch (...) {
            return false;
        }
        return true;
    }
}
