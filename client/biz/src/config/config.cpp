#include "config/config.h"

using namespace std;

// variable
void *mainThreadPool = nullptr;

QStackedWidget *stackedWidget = nullptr;

int tryGoManagementCount = 0;

airstrip::CommonBackendConfigDbManager commonDb;

std::deque<int> routerQueue = {};
