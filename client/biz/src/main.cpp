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
#include <boost/exception/info.hpp>
#include <sstream>

#ifndef WIN32
#include "airstrip_command.h"
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
        // update common set v="localhost:5525" where k="serverAddress";
        {
            std::string serverAddress = g_commonDb.getConfig(PRO_DB_COMMON_KEY_SERVER_ADD);
            if (serverAddress.empty()) {
                g_commonDb.upsertConfig(PRO_DB_COMMON_KEY_SERVER_ADD, "localhost:5525");
                serverAddress = "localhost:5525";
            }
            g_serverAddress = serverAddress;
        }
        // update common set v="123456" where k="managementPassword";
        {
            std::string managementPassword = g_commonDb.getConfig(PRO_DB_COMMON_KEY_MANA_PASS);
            if (managementPassword.empty()) {
                g_commonDb.upsertConfig(PRO_DB_COMMON_KEY_MANA_PASS, "123456");
                managementPassword = "123456";
            }
            g_managementPassword = managementPassword;
        }
        // update common set v="abcdefg" where k="signId";
        {
            std::string signId = g_commonDb.getConfig(PRO_DB_SIGN_ID);
            if (signId.empty()) {
                g_commonDb.upsertConfig(PRO_DB_SIGN_ID, "abcdefg");
                signId = "abcdefg";
            }
            g_signId = signId;
        }
        // update common set v="0.48" where k="faceThreshold";
        {
            std::string faceThreshold = g_commonDb.getConfig(PRO_DB_FACE_THRESHOLD);
            if (faceThreshold.empty()) {
                g_commonDb.upsertConfig(PRO_DB_FACE_THRESHOLD, "0.48");
                faceThreshold = "0.48";
            }
            try {
                g_faceThreshold = stod(faceThreshold);
            } catch (const std::exception &e) {
                std::ostringstream errMsg;
                errMsg << "Load data faceThreshold error " << e.what();
                logPrintln(errMsg.str(), airstrip::ERROR, __FUNCTION__);
                g_faceThreshold = 0.48;
            }
        }
        // update common set v="0.42" where k="faceThresholdNight";
        {
            std::string faceThresholdNight = g_commonDb.getConfig(PRO_DB_FACE_THRESHOLD_NIG);
            if (faceThresholdNight.empty()) {
                g_commonDb.upsertConfig(PRO_DB_FACE_THRESHOLD_NIG, "0.42");
                faceThresholdNight = "0.42";
            }
            try {
                g_faceThresholdNight = stod(faceThresholdNight);
            } catch (const std::exception &e) {
                std::ostringstream errMsg;
                errMsg << "Load data faceThresholdNight error " << e.what();
                logPrintln(errMsg.str(), airstrip::ERROR, __FUNCTION__);
                g_faceThresholdNight = 0.42;
            }
        }
        // update common set v="50" where k="volNum";
        {
            std::string volNum = g_commonDb.getConfig(PRO_DB_VOL_NUM);
            if (volNum.empty()) {
                g_commonDb.upsertConfig(PRO_DB_VOL_NUM, "50");
                volNum = "50";
            }
            try {
                g_volNum = stoi(volNum);
            } catch (const std::exception &e) {
                std::ostringstream errMsg;
                errMsg << "Load data volNum error " << e.what();
                logPrintln(errMsg.str(), airstrip::ERROR, __FUNCTION__);
                g_volNum = 50;
            }
        }
        // update common set v="2" where k="faceDistance";
        {
            std::string faceDistance = g_commonDb.getConfig(PRO_DB_FACE_DISTANCE);
            if (faceDistance.empty()) {
                g_commonDb.upsertConfig(PRO_DB_FACE_DISTANCE, "2");
                faceDistance = "2";
            }
            try {
                g_faceDistance = stoi(faceDistance);
            } catch (const std::exception &e) {
                std::ostringstream errMsg;
                errMsg << "Load data faceDistance error " << e.what();
                logPrintln(errMsg.str(), airstrip::ERROR, __FUNCTION__);
                g_faceDistance = 2;
            }
        }
        // update common set v="1" where k="netModel";
        {
            std::string netModel = g_commonDb.getConfig(PRO_DB_NET_MODEL);
            if (netModel.empty()) {
                g_commonDb.upsertConfig(PRO_DB_NET_MODEL, "1");
                netModel = "1";
            }
            try {
                g_netModel = stoi(netModel);
            } catch (const std::exception &e) {
                std::ostringstream errMsg;
                errMsg << "Load data netModel error " << e.what();
                logPrintln(errMsg.str(), airstrip::ERROR, __FUNCTION__);
                g_netModel = 1;
            }
        }
        // update common set v="" where k="wifiAccount";
        {
            std::string wifiAccount = g_commonDb.getConfig(PRO_DB_WIFI_ACCOUNT);
            if (wifiAccount.empty()) {
                g_commonDb.upsertConfig(PRO_DB_WIFI_ACCOUNT, "");
                wifiAccount = "";
            }
            g_wifiAccount = wifiAccount;
        }
        // update common set v="" where k="wifiPasswd";
        {
            std::string wifiPasswd = g_commonDb.getConfig(PRO_DB_WIFI_PASSWD);
            if (wifiPasswd.empty()) {
                g_commonDb.upsertConfig(PRO_DB_WIFI_PASSWD, "");
                wifiPasswd = "";
            }
            g_wifiPasswd = wifiPasswd;
        }
        // update common set v="1" where k="enableFaceSpoof";
        {
            std::string enableFaceSpoof = g_commonDb.getConfig(PRO_DB_ENABLE_FACE_SPOOF);
            if (enableFaceSpoof.empty()) {
                g_commonDb.upsertConfig(PRO_DB_ENABLE_FACE_SPOOF, "1");
                enableFaceSpoof = "1";
            }
            try {
                g_enableFaceSpoof = stoi(enableFaceSpoof);
            } catch (const std::exception &e) {
                std::ostringstream errMsg;
                errMsg << "Load data enableFaceSpoof error " << e.what();
                logPrintln(errMsg.str(), airstrip::ERROR, __FUNCTION__);
                g_enableFaceSpoof = 1;
            }
        }
        // update common set v="0" where k="lightOnlyCheck";
        {
            std::string lightOnlyCheck = g_commonDb.getConfig(PRO_DB_ENABLE_LIGHT_ONLY_CHECK);
            if (lightOnlyCheck.empty()) {
                g_commonDb.upsertConfig(PRO_DB_ENABLE_LIGHT_ONLY_CHECK, "0");
                lightOnlyCheck = "0";
            }
            try {
                g_lightOnlyCheck = stoi(lightOnlyCheck);
            } catch (const std::exception &e) {
                std::ostringstream errMsg;
                errMsg << "Load data lightOnlyCheck error " << e.what();
                logPrintln(errMsg.str(), airstrip::ERROR, __FUNCTION__);
                g_lightOnlyCheck = 0;
            }
        }
        // update common set v="200" where k="lightThreshold";
        {
            std::string lightThreshold = g_commonDb.getConfig(PRO_DB_ENABLE_LIGHT_THRESHOLD);
            if (lightThreshold.empty()) {
                g_commonDb.upsertConfig(PRO_DB_ENABLE_LIGHT_THRESHOLD, "200");
                lightThreshold = "200";
            }
            try {
                g_lightThreshold = stod(lightThreshold);
            } catch (const std::exception &e) {
                std::ostringstream errMsg;
                errMsg << "Load data lightThreshold error " << e.what();
                logPrintln(errMsg.str(), airstrip::ERROR, __FUNCTION__);
                g_lightThreshold = 200.0;
            }
        }
        // update common set v="70" where k="darkThreshold";
        {
            std::string darkThreshold = g_commonDb.getConfig(PRO_DB_ENABLE_DARK_THRESHOLD);
            if (darkThreshold.empty()) {
                g_commonDb.upsertConfig(PRO_DB_ENABLE_DARK_THRESHOLD, "70");
                darkThreshold = "70";
            }
            try {
                g_darkThreshold = stod(darkThreshold);
            } catch (const std::exception &e) {
                std::ostringstream errMsg;
                errMsg << "Load data darkThreshold error " << e.what();
                logPrintln(errMsg.str(), airstrip::ERROR, __FUNCTION__);
                g_darkThreshold = 70.0;
            }
        }
        // update common set v="0.5" where k="lightRatio";
        {
            std::string lightRatio = g_commonDb.getConfig(PRO_DB_ENABLE_LIGHT_RATIO);
            if (lightRatio.empty()) {
                g_commonDb.upsertConfig(PRO_DB_ENABLE_LIGHT_RATIO, "0.5");
                lightRatio = "0.5";
            }
            try {
                g_lightRatio = stod(lightRatio);
            } catch (const std::exception &e) {
                std::ostringstream errMsg;
                errMsg << "Load data lightRatio error " << e.what();
                logPrintln(errMsg.str(), airstrip::ERROR, __FUNCTION__);
                g_lightRatio = 0.5;
            }
        }
        // update common set v="0.3" where k="darkRatio";
        {
            std::string darkRatio = g_commonDb.getConfig(PRO_DB_ENABLE_DARK_RATIO);
            if (darkRatio.empty()) {
                g_commonDb.upsertConfig(PRO_DB_ENABLE_DARK_RATIO, "0.3");
                darkRatio = "0.3";
            }
            try {
                g_darkRatio = stod(darkRatio);
            } catch (const std::exception &e) {
                std::ostringstream errMsg;
                errMsg << "Load data darkRatio error " << e.what();
                logPrintln(errMsg.str(), airstrip::ERROR, __FUNCTION__);
                g_darkRatio = 0.3;
            }
        }
        // update common set v="3" where k="faceRegCount";
        {
            std::string faceRegCount = g_commonDb.getConfig(PRO_DB_FACE_REG_COUNT);
            if (faceRegCount.empty()) {
                g_commonDb.upsertConfig(PRO_DB_FACE_REG_COUNT, "3");
                faceRegCount = "3";
            }
            try {
                g_faceRegCount = stoi(faceRegCount);
            } catch (const std::exception &e) {
                std::ostringstream errMsg;
                errMsg << "Load data faceRegCount error " << e.what();
                logPrintln(errMsg.str(), airstrip::ERROR, __FUNCTION__);
                g_faceRegCount = 3;
            }
        }
        // update common set v="600" where k="taskIvSec";
        {
            std::string taskIvSec = g_commonDb.getConfig(PRO_DB_TASK_IV_SEC);
            if (taskIvSec.empty()) {
                g_commonDb.upsertConfig(PRO_DB_TASK_IV_SEC, "600");
                taskIvSec = "600";
            }
            try {
                g_taskIvSec = stoi(taskIvSec);
            } catch (const std::exception &e) {
                std::ostringstream errMsg;
                errMsg << "Load data taskIvSec error " << e.what();
                logPrintln(errMsg.str(), airstrip::ERROR, __FUNCTION__);
                g_taskIvSec = 600;
            }
        }
        // update common set v="0" where k="showConfUser";
        {
            std::string showConfUser = g_commonDb.getConfig(PRO_DB_SHOW_CONF_USER);
            if (showConfUser.empty()) {
                g_commonDb.upsertConfig(PRO_DB_SHOW_CONF_USER, "0");
                showConfUser = "0";
            }
            try {
                g_showConfUser = stoi(showConfUser);
            } catch (const std::exception &e) {
                std::ostringstream errMsg;
                errMsg << "Load data showConfUser error " << e.what();
                logPrintln(errMsg.str(), airstrip::ERROR, __FUNCTION__);
                g_showConfUser = 0;
            }
        }

        logPrintln("Db finish", airstrip::INFO, __FUNCTION__);
    }

    // Pre script
#ifndef WIN32
    airstrip::execScript(g_appWorkDir + "script/linux/reset_vol.sh " + std::to_string(g_volNum));
#endif

    // Init application
    QApplication app(argc, argv);

    // Init thread poll
    g_mainThreadPool = airstrip::ThreadPool::getInstance(8);

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
