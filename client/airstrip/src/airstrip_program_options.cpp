#include "airstrip_program_options.h"
#include <boost/program_options.hpp>
#include <iostream>

namespace po = boost::program_options;
using namespace std;

namespace airstrip {


    std::map<std::string, std::string> enableProgramOptions(
        const AirstripProgramOptions& optSetting, int argc, char **argv)
    {
        std::map<std::string, std::string> retOptions = {};
//        if(!optSetting.allowEmptyOpt && optSetting.options.empty()) {
//            cout << optSetting.emptyOptError << endl;
//            exit(0);
//        }

        po::options_description desc("Allowed options");
        desc.add_options()
     ("help,h", "Produce help message") // -h 或 --help
     ("input,i", po::value<std::string>(), "Input file") // -i 或 --input
     ("output,o", po::value<std::string>(), "Output file") // -o 或 --output
     ("verbose,v", "Enable verbose output");
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        try {

            for(auto &opt: optSetting.options) {

            }


        } catch (const std::exception &e) {
          cout << "Failed to parse options: " << e.what() << endl;
        }
        return retOptions;
    }


}