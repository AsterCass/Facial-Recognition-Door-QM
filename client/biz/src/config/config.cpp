#include "config/config.h"

using namespace std;

// variable
void *g_mainThreadPool = nullptr;

int g_tryGoManagementCount = 0;

bool g_closeFaceRecognition = true;

bool g_onFaceRegisterProcess = false;

airstrip::CommonBackendConfigDbManager g_commonDb;

std::string g_appWorkDir;
std::string g_serverAddress;
std::string g_managementPassword;
std::string g_signId;
double g_faceThreshold;
int g_volNum;
int g_faceDistance;
int g_netModel;
std::string g_wifiAccount;
std::string g_wifiPasswd;
int g_enableFaceSpoof;
int g_lightOnlyCheck;
double g_lightThreshold;
double g_darkThreshold;
double g_lightRatio;
double g_darkRatio;
