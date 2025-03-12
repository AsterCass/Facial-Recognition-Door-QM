#ifndef API_H
#define API_H

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


#endif // API_H
