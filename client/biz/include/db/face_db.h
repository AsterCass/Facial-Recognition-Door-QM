#ifndef FACE_DB_H
#define FACE_DB_H

#include <string>
#include <vector>
#include "utils/face_recognition.h"

void initFaceDB();

std::vector<FaceUserInfo> getAllFace();

bool insertFaceDB(const FaceUserInfo &info, int64_t *faceId);

bool updateFaceDB(const FaceUserInfo &info);

bool deleteFaceDB(const std::string &userId);

bool disableFaceUser(const std::string &userId, int isEnable);

bool voiceTmpFaceUser(const std::string &userId, const std::string &voiceTmp);

#endif //FACE_DB_H
