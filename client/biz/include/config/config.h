#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <QStackedWidget>

// const
constexpr auto APP_VERSION = "1.0.0";
constexpr int HTTP_CODE_OK = 200;
constexpr int GENERAL_TASK_INTERVAL_SECONDS = 5;
constexpr auto ENV_DEV = "dev";
constexpr auto ENV_PROD = "prod";

constexpr auto PRO_OPT_VERSION = "version";
constexpr auto PRO_OPT_ENV = "env";
constexpr auto PRO_OPT_TASK_IN_CNT = "taskIvCnt";
constexpr auto PRO_OPT_APP_WIDTH = "width";
constexpr auto PRO_OPT_APP_HEIGHT = "height";


// variable
extern int taskUpdateIpIntervalCount;
extern QStackedWidget *stackedWidget;
extern void *mainThreadPool;


#endif //CONFIG_H
