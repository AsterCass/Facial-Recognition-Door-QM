#include "config/config.h"
#include "enums/general_enums.h"

using namespace std;

// variable
void *mainThreadPool = nullptr;

QStackedWidget *stackedWidget = nullptr;

int tryGoManagementCount = 0;

airstrip::CommonBackendConfigDbManager commonDb;
