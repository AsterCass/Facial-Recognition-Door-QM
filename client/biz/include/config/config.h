#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <deque>
#include <QStackedWidget>
#include <airstrip_db.h>

// const
constexpr auto APP_VERSION = "3.0.0";
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
constexpr auto PRO_DB_COMMON_DATA = "commonData";
constexpr auto PRO_DB_FACE_THRESHOLD = "faceThreshold";
constexpr auto PRO_DB_VOL_NUM = "volNum";
constexpr auto PRO_DB_ENABLE_FACE_SPOOF = "enableFaceSpoof";

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
    {1121, 400, 80},
};

// variable
extern void *g_mainThreadPool;

extern QStackedWidget *g_stackedWidget;

extern int g_tryGoManagementCount;

extern airstrip::CommonBackendConfigDbManager g_commonDb;

extern std::deque<int> g_routerQueue;

extern std::string g_appWorkDir;
extern std::string g_serverAddress;
extern std::string g_managementPassword;
extern std::string g_signId;
extern std::string g_commonData;


#endif //CONFIG_H
