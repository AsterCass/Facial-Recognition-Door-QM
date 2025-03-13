#ifndef FACE_RECOGNITION_H
#define FACE_RECOGNITION_H

#include <opencv2/core.hpp>

struct FaceUserInfo {
    int64_t faceId;
    std::string faceAddress;
    std::string faceFeat;
    std::string userId;
    int64_t startTime;
    int64_t endTime;
    bool isEnable;
    std::string voiceTemplate;
};

void initFaceRecognition();

bool faceInsert(const std::string &address, const FaceUserInfo &userInfo);

bool faceInsert(const cv::Mat &pic, const FaceUserInfo &userInfo);

bool faceDelete(const FaceUserInfo &userInfo);

bool faceUpdate(const cv::Mat &pic, const FaceUserInfo &userInfo);

bool faceDetect(const cv::Mat &frame, cv::Rect &rect, int orgCols, int orgRows);

void faceRecognition(const cv::Mat &frame, const cv::Rect &rect);

#endif //FACE_RECOGNITION_H
