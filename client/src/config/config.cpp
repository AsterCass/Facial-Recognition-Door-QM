#include "config/config.h"
#include "enums/general_enums.h"

using namespace std;

// variable

int taskUpdateIpIntervalCount = 1;
int currentLogLevel = DEBUG;
QStackedWidget *stackedWidget = nullptr;

// input variable

int taskPullTaskIntervalCount = 20;
string appEnv = ENV_PROD;
int logPrintType = CONSOLE;
