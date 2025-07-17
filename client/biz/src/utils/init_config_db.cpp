#include "utils/init_config_db.h"
#include <string>
#include <config/config.h>
#include <sstream>
#include <airstrip_log.h>

template<typename T>
T convertFromString(const std::string &value);

// 特化：int
template<>
int convertFromString<int>(const std::string &value) {
    return std::stoi(value);
}

// 特化：double
template<>
double convertFromString<double>(const std::string &value) {
    return std::stod(value);
}

// 特化：std::string
template<>
std::string convertFromString<std::string>(const std::string &value) {
    return value;
}

template<typename T>
void initConfigValue(const std::string &key, const std::string &defaultStr, T &targetVar) {
    std::string valueStr = g_commonDb.getConfig(key);
    if (valueStr.empty()) {
        g_commonDb.upsertConfig(key, defaultStr);
        valueStr = defaultStr;
    }

    try {
        targetVar = convertFromString<T>(valueStr);
    } catch (const std::exception &e) {
        std::ostringstream errMsg;
        errMsg << "Load config [" << key << "] error: " << e.what();
        logPrintln(errMsg.str(), airstrip::ERROR, __FUNCTION__);
        targetVar = convertFromString<T>(defaultStr);
    }

    std::ostringstream stdMsg;
    stdMsg << "Loaded config [" << key << "] value = [" << targetVar << "]";
    logPrintln(stdMsg.str(), airstrip::INFO, __FUNCTION__);
}


void initConfigDb() {
    // str
    initConfigValue<std::string>(
        PRO_DB_COMMON_KEY_SERVER_ADD, "localhost:5525", g_serverAddress);
    initConfigValue<std::string>(
        PRO_DB_COMMON_KEY_MANA_PASS, "123456", g_managementPassword);
    initConfigValue<std::string>(
        PRO_DB_SIGN_ID, "abcdefg", g_signId);
    initConfigValue<std::string>(
        PRO_DB_WIFI_ACCOUNT, "", g_wifiAccount);
    initConfigValue<std::string>(
        PRO_DB_WIFI_PASSWD, "", g_wifiPasswd);

    // double
    initConfigValue<double>(PRO_DB_FACE_THRESHOLD, "0.48", g_faceThreshold);
    initConfigValue<double>(PRO_DB_FACE_THRESHOLD_NIG, "0.42", g_faceThresholdNight);
    initConfigValue<double>(PRO_DB_ENABLE_LIGHT_THRESHOLD, "200", g_lightThreshold);
    initConfigValue<double>(PRO_DB_ENABLE_DARK_THRESHOLD, "70", g_darkThreshold);
    initConfigValue<double>(PRO_DB_ENABLE_LIGHT_RATIO, "0.5", g_lightRatio);
    initConfigValue<double>(PRO_DB_ENABLE_DARK_RATIO, "0.3", g_darkRatio);

    // int
    initConfigValue<int>(PRO_DB_VOL_NUM, "50", g_volNum);
    initConfigValue<int>(PRO_DB_FACE_DISTANCE, "2", g_faceDistance);
    initConfigValue<int>(PRO_DB_NET_MODEL, "1", g_netModel);
    initConfigValue<int>(PRO_DB_ENABLE_FACE_SPOOF, "1", g_enableFaceSpoof);
    initConfigValue<int>(PRO_DB_ENABLE_LIGHT_ONLY_CHECK, "0", g_lightOnlyCheck);
    initConfigValue<int>(PRO_DB_FACE_REG_COUNT, "3", g_faceRegCount);
    initConfigValue<int>(PRO_DB_TASK_IV_SEC, "600", g_taskIvSec);
    initConfigValue<int>(PRO_DB_SHOW_CONF_USER, "0", g_showConfUser);
    initConfigValue<int>(PRO_DB_CAM_AUTO_LIGHT, "1", g_camAutoLight);
    initConfigValue<int>(PRO_DB_CAM_EXPOSE, "800", g_camExpose);
    initConfigValue<int>(PRO_DB_CAM_GAIN, "64", g_camGain);
    initConfigValue<int>(PRO_DB_CAM_LIGHT, "0", g_camLight);
    initConfigValue<int>(PRO_DB_FULL_FACE_COMPARE, "0", g_fullFaceCompare);
    initConfigValue<int>(PRO_DB_FACE_REG_IV_SEC, "0", g_faceRegIvSec);
    initConfigValue<int>(PRO_DB_FACE_REG_CORE_IV_MILL_SEC, "500", g_faceRegCoreIvMillSec);
    initConfigValue<int>(PRO_DB_NEED_BACKUP, "0", g_needBackup);
    initConfigValue<int>(PRO_DB_AUTO_CLOSE_DOOR_SEC, "5", g_autoCloseDoorSec);
    initConfigValue<int>(PRO_DB_ALLOW_CARD_OPEN, "1", g_allowCardOpen);
    initConfigValue<int>(PRO_DB_ALLOW_FACE_OPEN, "1", g_allowFaceOpen);
    initConfigValue<int>(PRO_DB_VOICE, "1", g_voice);
}
