#include "config/config.h"

using namespace std;

// variable
void *g_mainThreadPool = nullptr;

int g_tryGoManagementCount = 0;

airstrip::CommonBackendConfigDbManager g_commonDb;

std::string g_appWorkDir;
std::string g_serverAddress;
std::string g_managementPassword;
std::string g_signId;
std::string g_commonData;
