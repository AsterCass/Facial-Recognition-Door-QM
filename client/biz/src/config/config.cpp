#include "config/config.h"

using namespace std;

// variable
void *g_mainThreadPool = nullptr;

int g_tryGoManagementCount = 0;

bool g_closeFaceRecognition = true;

bool g_onFaceRegisterProcess = false;

int g_currentLightLevel = 13;

std::string g_prepareUpdateUrl;
std::string g_prepareUpdateVersion;

airstrip::CommonBackendConfigDbManager g_commonDb;

std::string g_appWorkDir;
std::string g_serverAddress;
std::string g_managementPassword;
std::string g_signId;
double g_faceThreshold;
double g_faceThresholdNight;
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
int g_faceRegCount;
int g_taskIvSec;
int g_showConfUser;
int g_camAutoLight;
int g_camExpose;
int g_camGain;
int g_camLight;
int g_fullFaceCompare;
int g_faceRegIvSec;
int g_faceRegCoreIvMillSec;
