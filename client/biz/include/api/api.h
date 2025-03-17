#ifndef API_H
#define API_H


#include <map>
#include <string>

#include "enums/general_enums.h"


// local

std::string getSn();

std::string getSign();

void openDoor();

void closeDoor();

void playWav(const std::string &voiceTemplate);

void playWav(PlayWavType type);


// http

void login();

void checkTask();

void faceGrant();

void appUpdate();

void uploadOpenRecord();

void taskFinish(const std::map<std::string, int> &taskStatusMap);

void dataBackupUp();

void dataBackupDown();


#endif // API_H
