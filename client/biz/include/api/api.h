#ifndef API_H
#define API_H


#include <map>
#include <string>

#include "db/open_record_db.h"
#include "enums/general_enums.h"
#include "opencv2/opencv.hpp"

typedef struct AppUpdateNotification {
    bool isSuccessful;
    bool isNeedUpdate;
    std::string updateVersion;
    std::string updateUrl;
} AppUpdateNotification;

// local

std::string getSn();

std::string getSign();

void openDoor();

void closeDoor();

void playWav(const std::string &voiceTemplate);

void playWav(PlayWavType type);

bool linkedServer();


// http

void login();

void checkTask();

bool faceGrant(const cv::Mat &frame, const std::string &userPhone);

AppUpdateNotification appUpdate();

bool uploadOpenRecord(const std::vector<OpenRecordInfo> &records);

void taskFinish(const std::map<std::string, int> &taskStatusMap);

bool dataBackupUp();


#endif // API_H
