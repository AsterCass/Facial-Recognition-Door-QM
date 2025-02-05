#include "config/config.h"
#include "enums/general_enums.h"

using namespace std;

// variable

int taskUpdateIpIntervalCount = 1;
QStackedWidget *stackedWidget = nullptr;
void *mainThreadPool = nullptr;

// input variable

int taskPullTaskIntervalCount = 20;
string appEnv = ENV_PROD;
