#ifndef API_H
#define API_H


#include <map>
#include <string>

#include "db/open_record_db.h"
#include "enums/general_enums.h"
#include "opencv2/opencv.hpp"


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

void appUpdate();

bool uploadOpenRecord(const std::vector<OpenRecordInfo> &records);

void taskFinish(const std::map<std::string, int> &taskStatusMap);

bool dataBackupUp();

bool dataBackupDown();


#endif // API_H
