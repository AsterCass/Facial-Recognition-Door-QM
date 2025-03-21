#include <QApplication>
#include <airstrip_log.h>
#include <airstrip_program_options.h>
#include <enums/general_enums.h>
#include "airstrip_db.h"
#include "airstrip_thread_pool.h"
#include "utils/scheduled_task.h"
#include "ui/main_router.h"
#include "config/config.h"
#include <boost/json.hpp>

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
        taskIvCnt.optionDesc = "Set common task interval sec count. \nDefault values is 1";
        taskIvCnt.defaultValue = {std::to_string(1)};
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
    airstrip::getProgramOptions(PRO_OPT_APP_WORK_DIR, &g_appWorkDir);
    if (g_appWorkDir.empty()) {
        logPrintln("App work dir not found", airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }
#ifndef WIN32
    google_breakpad::MinidumpDescriptor descriptor(g_appWorkDir + "dump");
    google_breakpad::ExceptionHandler eh(descriptor, NULL, dumpCallback, NULL, true, -1);
#endif

    // Db
    {
        g_commonDb.initDb(g_appWorkDir + PRO_DB_ADDRESS);

        // update common_backend_config set config_value_json="localhost:5525" where config_name="serverAddress";
        std::string serverAddress = g_commonDb.getConfig(PRO_DB_COMMON_KEY_SERVER_ADD);
        if (serverAddress.empty()) {
            g_commonDb.upsertConfig(PRO_DB_COMMON_KEY_SERVER_ADD, "localhost:5525");
        }
        g_serverAddress = serverAddress;

        // update common_backend_config set config_value_json="123456" where config_name="managementPassword";
        std::string managementPassword = g_commonDb.getConfig(PRO_DB_COMMON_KEY_MANA_PASS);
        if (managementPassword.empty()) {
            g_commonDb.upsertConfig(PRO_DB_COMMON_KEY_MANA_PASS, "123456");
        }
        g_managementPassword = managementPassword;

        // update common_backend_config set config_value_json="abcdefg" where config_name="signId";
        std::string signId = g_commonDb.getConfig(PRO_DB_SIGN_ID);
        if (signId.empty()) {
            g_commonDb.upsertConfig(PRO_DB_SIGN_ID, "abcdefg");
        }
        g_signId = signId;

        boost::json::object commonDataJson;
        commonDataJson[PRO_DB_FACE_THRESHOLD] = 0.46;
        commonDataJson[PRO_DB_VOL_NUM] = 50;
        commonDataJson[PRO_DB_FACE_DISTANCE] = 2;
        commonDataJson[PRO_DB_NET_MODEL] = 1;
        commonDataJson[PRO_DB_WIFI_ACCOUNT] = "";
        commonDataJson[PRO_DB_WIFI_PASSWD] = "";
        commonDataJson[PRO_DB_ENABLE_FACE_SPOOF] = 1;
        commonDataJson[PRO_DB_ENABLE_LIGHT_ONLY_CHECK] = 0;

        std::string commonData = g_commonDb.getConfig(PRO_DB_COMMON_DATA);
        if (commonData.empty()) {
            std::string commonDataJsonJson = serialize(commonDataJson);
            g_commonDb.upsertConfig(PRO_DB_COMMON_DATA, commonDataJsonJson);
        }
        g_commonData = commonData;
        logPrintln("Db finish", airstrip::INFO, __FUNCTION__);
    }

    // Init application
    QApplication app(argc, argv);

    // Init thread poll
    g_mainThreadPool = airstrip::ThreadPool::getInstance(3);

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
