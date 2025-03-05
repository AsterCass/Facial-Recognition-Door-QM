#include <QApplication>
#include <airstrip_log.h>
#include <airstrip_program_options.h>
#include <enums/general_enums.h>
#include "airstrip_db.h"
#include "airstrip_thread_pool.h"
#include "utils/scheduled_task.h"
#include "ui/main_router.h"
#include "config/config.h"
#ifndef WIN32
#include "client/linux/handler/exception_handler.h"
static bool dumpCallback(const google_breakpad::MinidumpDescriptor& descriptor,
void* context, bool succeeded) {
    printf("Dump path: %s\n", descriptor.path());
    return succeeded;
}
#endif

int main(int argc, char *argv[]) {
    // Input args
    airstrip::AirstripProgramOptions optSetting; {
        // Version
        airstrip::AirstripProgramOptionDetail version;
        version.needContinue = false;
        version.needInput = false;
        version.optionDesc = "Produce version";
        version.outputValue = APP_VERSION;
        version.valueType = airstrip::STRING;
        optSetting.options[std::string(PRO_OPT_VERSION) + ",v"] = version;
        // Env
        airstrip::AirstripProgramOptionDetail env;
        env.needContinue = true;
        env.needInput = true;
        env.optionDesc = "Set produce environment, 0 or 1, 1 means dev, 0 means prod. \nDefault values is 1";
        env.defaultValue = {std::to_string(DEV)};
        env.valueType = airstrip::INTEGER;
        optSetting.options[std::string(PRO_OPT_ENV)] = env;
        // Task interval Count
        airstrip::AirstripProgramOptionDetail taskIvCnt;
        taskIvCnt.needContinue = true;
        taskIvCnt.needInput = true;
        taskIvCnt.optionDesc = "Set common task interval sec count. \nDefault values is 5";
        taskIvCnt.defaultValue = {std::to_string(5)};
        taskIvCnt.valueType = airstrip::INTEGER;
        optSetting.options[std::string(PRO_OPT_TASK_IN_CNT)] = taskIvCnt;
        // App width and height
        airstrip::AirstripProgramOptionDetail appWidth;
        airstrip::AirstripProgramOptionDetail appHeight;
        appWidth.needContinue = true;
        appWidth.needInput = true;
        appWidth.optionDesc = "Set app width. \nDefault values is 400";
        appWidth.defaultValue = {std::to_string(400)};
        appWidth.valueType = airstrip::INTEGER;
        optSetting.options[std::string(PRO_OPT_APP_WIDTH)] = appWidth;
        appHeight.needContinue = true;
        appHeight.needInput = true;
        appHeight.optionDesc = "Set app width. \nDefault values is 640";
        appHeight.defaultValue = {std::to_string(640)};
        appHeight.valueType = airstrip::INTEGER;
        optSetting.options[std::string(PRO_OPT_APP_HEIGHT)] = appHeight;
        // App work dir
        airstrip::AirstripProgramOptionDetail appWorkDir;
        appWorkDir.needContinue = true;
        appWorkDir.needInput = true;
        appWorkDir.optionDesc = "Set application work directory, for get static resources or script etc."
                "\n Default value is '../assets/data/frd/'";
        appWorkDir.defaultValue = {"../assets/data/frd/"};
        appWorkDir.valueType = airstrip::STRING;
        optSetting.options[std::string(PRO_OPT_APP_WORK_DIR)] = appWorkDir;
        // Other
    }
    enableProgramOptions(optSetting, argc, argv);

    // Dump
    std::string appWorkDir;
    airstrip::getProgramOptions(PRO_OPT_APP_WORK_DIR, &appWorkDir);
#ifndef WIN32
    google_breakpad::MinidumpDescriptor descriptor(appWorkDir + "dump/");
    google_breakpad::ExceptionHandler eh(descriptor, NULL, dumpCallback, NULL, true, -1);
#endif

    // Init application
    QApplication app(argc, argv);

    // Init thread poll
    mainThreadPool = airstrip::ThreadPool::getInstance(3);

    // Db
    if (!appWorkDir.empty()) {
        commonDb.initDb(appWorkDir + PRO_DB_ADDRESS);
        std::string serverAddress = commonDb.getConfig(PRO_DB_COMMON_KEY_SERVER_ADD);
        if (serverAddress.empty()) {
            commonDb.upsertConfig(PRO_DB_COMMON_KEY_SERVER_ADD, "localhost:5525");
        }
        std::string managementPassword = commonDb.getConfig(PRO_DB_COMMON_KEY_MANA_PASS);
        if (managementPassword.empty()) {
            commonDb.upsertConfig(PRO_DB_COMMON_KEY_MANA_PASS, "123456");
        }
        logPrintln("Db finish", airstrip::INFO, __FUNCTION__);
    }

    // Page router
    const auto router = MainRouter::getInstance();
    router->setWindowFlags(Qt::WindowStaysOnTopHint);
    router->show();
    logPrintln("UI finish", airstrip::INFO, __FUNCTION__);

    // Init Schedule task
    ScheduledTask::getInstance();
    logPrintln("ScheduledTask finish", airstrip::INFO, __FUNCTION__);

    // Finish
    logPrintln("Application started", airstrip::INFO, __FUNCTION__);


    return QApplication::exec();
}
