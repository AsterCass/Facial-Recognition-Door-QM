#ifndef CONFIG_H
#define CONFIG_H

#include <functional>
#include <string>
#include <QStackedWidget>

// const
constexpr auto APP_VERSION = "1.0.0";
constexpr std::array<std::pair<const char *, const char *>, 7> WEEK_CN_MAP = {
    {
        {"Sunday", "星期日"},
        {"Monday", "星期一"},
        {"Tuesday", "星期二"},
        {"Wednesday", "星期三"},
        {"Thursday", "星期四"},
        {"Friday", "星期五"},
        {"Saturday", "星期六"},
    }
};
constexpr int HTTP_CODE_OK = 200;
constexpr int GENERAL_TASK_INTERVAL_SECONDS = 5;
constexpr auto ENV_DEV = "dev";
constexpr auto ENV_PROD = "prod";


// variable
extern int taskUpdateIpIntervalCount;
extern QStackedWidget *stackedWidget;
extern void *mainThreadPool;


// input args

extern int taskPullTaskIntervalCount;
extern std::string appEnv;


#endif //CONFIG_H
