#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <QStackedWidget>
#include <airstrip_db.h>

// const
constexpr auto APP_VERSION = "3.1.0";
constexpr int HTTP_CODE_OK = 200;
constexpr auto ENV_DEV = "dev";
constexpr auto ENV_PROD = "prod";

constexpr auto PRO_OPT_VERSION = "version";
constexpr auto PRO_OPT_ENV = "env";
constexpr auto PRO_OPT_TASK_IN_CNT = "taskIvCnt";
constexpr auto PRO_OPT_APP_WIDTH = "width";
constexpr auto PRO_OPT_APP_HEIGHT = "height";
constexpr auto PRO_OPT_APP_WORK_DIR = "appWorkDir";

constexpr auto PRO_DB_ADDRESS = "db/common.db";
constexpr auto PRO_DB_COMMON_KEY_SERVER_ADD = "serverAddress";
constexpr auto PRO_DB_COMMON_KEY_MANA_PASS = "managementPassword";
constexpr auto PRO_DB_SIGN_ID = "signId";
constexpr auto PRO_DB_FACE_THRESHOLD = "faceThreshold";
constexpr auto PRO_DB_FACE_THRESHOLD_NIG = "faceThresholdNight";
constexpr auto PRO_DB_VOL_NUM = "volNum";
constexpr auto PRO_DB_FACE_DISTANCE = "faceDistance";
constexpr auto PRO_DB_NET_MODEL = "netModel";
constexpr auto PRO_DB_WIFI_ACCOUNT = "wifiAccount";
constexpr auto PRO_DB_WIFI_PASSWD = "wifiPasswd";
constexpr auto PRO_DB_ENABLE_FACE_SPOOF = "enableFaceSpoof";
constexpr auto PRO_DB_ENABLE_LIGHT_ONLY_CHECK = "lightOnlyCheck";
constexpr auto PRO_DB_ENABLE_LIGHT_THRESHOLD = "lightThreshold";
constexpr auto PRO_DB_ENABLE_DARK_THRESHOLD = "darkThreshold";
constexpr auto PRO_DB_ENABLE_LIGHT_RATIO = "lightRatio";
constexpr auto PRO_DB_ENABLE_DARK_RATIO = "darkRatio";
constexpr auto PRO_DB_FACE_REG_COUNT = "faceRegCount";
constexpr auto PRO_DB_TASK_IV_SEC = "taskIvSec";
constexpr auto PRO_DB_SHOW_CONF_USER = "showConfUser";
constexpr auto PRO_DB_CAM_AUTO_LIGHT = "camAutoLight";
constexpr auto PRO_DB_CAM_EXPOSE = "camExpose";
constexpr auto PRO_DB_CAM_GAIN = "camGain";
constexpr auto PRO_DB_CAM_LIGHT = "camLight";
constexpr auto PRO_DB_FULL_FACE_COMPARE = "fullFaceCompare";
constexpr auto PRO_DB_FACE_REG_IV_SEC = "faceRegIvSec";
constexpr auto PRO_DB_FACE_REG_CORE_IV_MILL_SEC = "faceRegCoreIvMillSec";


const std::vector<std::string> CHINESE_WEEK = {
    "星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六", "星期日",
};

constexpr auto IR_SCALE = 0.2;

#ifdef WIN32
constexpr auto MINIMUM_FONT_SIZE = 7;
constexpr auto MINI_FONT_SIZE = 8;
constexpr auto MEDIUM_FONT_SIZE = 24;
constexpr auto MAX_FONT_SIZE = 36;
constexpr auto MINI_ICON_SIZE = 20;
#else
constexpr auto MINIMUM_FONT_SIZE = 5;
constexpr auto MINI_FONT_SIZE = 6;
constexpr auto MEDIUM_FONT_SIZE = 12;
constexpr auto MAX_FONT_SIZE = 18;
constexpr auto MINI_ICON_SIZE = 40;
#endif


#ifdef  WIN32
constexpr auto CERT_PATH = "";
#else
constexpr auto CERT_PATH = "/etc/ssl/certs/ca-certificates.crt";
#endif

const std::vector<std::vector<int> > EXPOSE_AND_GAIN_PARAM = {
    {10, 64, 0},
    {40, 64, 0},
    {70, 64, 0},
    {100, 64, 0},
    {200, 64, 0},
    {300, 64, 0},
    {400, 64, 0},
    {500, 64, 0},
    {600, 64, 0},
    {700, 64, 0},
    {800, 64, 0},
    {900, 64, 0},
    {1000, 64, 0},
    {1121, 64, 0},
    {1121, 85, 0},
    {1121, 150, 0},
    {1121, 200, 0},
    {1121, 300, 0},
    {1121, 200, 40},
    {1121, 200, 80},
    {1121, 200, 160},
};

// variable
extern void *g_mainThreadPool;

extern int g_tryGoManagementCount;

extern bool g_closeFaceRecognition;

extern bool g_onFaceRegisterProcess;

extern int g_currentLightLevel;

extern std::string g_prepareUpdateUrl;
extern std::string g_prepareUpdateVersion;

extern airstrip::CommonBackendConfigDbManager g_commonDb;

extern std::string g_appWorkDir;
extern std::string g_serverAddress;
extern std::string g_managementPassword;
extern std::string g_signId;
extern double g_faceThreshold;
extern double g_faceThresholdNight;
extern int g_volNum;
extern int g_faceDistance;
extern int g_netModel;
extern std::string g_wifiAccount;
extern std::string g_wifiPasswd;
extern int g_enableFaceSpoof;
extern int g_lightOnlyCheck;
extern double g_lightThreshold;
extern double g_darkThreshold;
extern double g_lightRatio;
extern double g_darkRatio;
extern int g_faceRegCount;
extern int g_taskIvSec;
extern int g_showConfUser;
extern int g_camAutoLight;
extern int g_camExpose;
extern int g_camGain;
extern int g_camLight;
extern int g_fullFaceCompare;
extern int g_faceRegIvSec;
extern int g_faceRegCoreIvMillSec;

// function

inline bool currentIsNight() {
    return EXPOSE_AND_GAIN_PARAM.at(g_currentLightLevel).at(2) != 0;
}


#endif //CONFIG_H
