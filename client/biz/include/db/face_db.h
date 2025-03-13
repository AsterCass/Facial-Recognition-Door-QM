#ifndef FACE_DB_H
#define FACE_DB_H

#include <string>
#include <vector>
#include "utils/face_recognition.h"

void initFaceDB();

std::vector<FaceUserInfo> getAllFace();

bool insertFaceDB(const std::string &userId, const std::string &extra, const std::string &feature, int64_t *faceId);

bool updateFaceDB(const std::string &userId, const std::string &extra, const std::string &feature);

bool deleteFaceDB(const std::string &userId);

#endif //FACE_DB_H
