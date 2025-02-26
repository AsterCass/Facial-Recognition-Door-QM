#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <QStackedWidget>

// const
constexpr auto APP_VERSION = "1.0.0";
constexpr int HTTP_CODE_OK = 200;
constexpr auto ENV_DEV = "dev";
constexpr auto ENV_PROD = "prod";

constexpr auto PRO_OPT_VERSION = "version";
constexpr auto PRO_OPT_ENV = "env";
constexpr auto PRO_OPT_TASK_IN_CNT = "taskIvCnt";
constexpr auto PRO_OPT_APP_WIDTH = "width";
constexpr auto PRO_OPT_APP_HEIGHT = "height";
constexpr auto PRO_OPT_APP_WORK_DIR = "appWorkDir";

const std::vector<std::string> CHINESE_WEEK = {
    "星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六", "星期日",
};

// variable
extern void *mainThreadPool;

extern QStackedWidget *stackedWidget;

extern int taskUpdateIpIntervalCount;


#endif //CONFIG_H
