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
    int isEnable;
    std::string voiceTemplate;
};

void updateLight(int expose, int gain, int light);

void closeLight();

void initFaceRecognition();

void loadFaceDb();

bool faceInsert(const std::string &address, FaceUserInfo &userInfo);

bool faceInsert(const cv::Mat &pic, FaceUserInfo &userInfo);

bool faceDelete(const FaceUserInfo &userInfo);

bool faceUpdate(const cv::Mat &pic, FaceUserInfo &userInfo);

bool faceDisable(const std::string &userId, int isEnable);

bool faceVoiceTemplate(const std::string &userId, const std::string &voiceFeature);

bool faceDetect(const cv::Mat &frame, const cv::Mat &rgaFrame, cv::Rect &rect, int orgCols, int orgRows);

void faceRecognition(const std::string &address, const std::string &addressIr);

void faceRecognition(const cv::Mat &frame, const cv::Mat &frameIr);

#endif //FACE_RECOGNITION_H
