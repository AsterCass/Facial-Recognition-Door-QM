#ifndef FACE_RECOGNITION_H
#define FACE_RECOGNITION_H

#include <opencv2/core.hpp>

#define MAX_OUTPUT_FRAME_SIZE 1382400

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

struct CameraOutputFrameInfo {
    // 这里值计算为 1280 * 720 * 3 /2
    uchar data[MAX_OUTPUT_FRAME_SIZE];
    int width;
    int height;
    int size;
};
// 人脸检测
CameraOutputFrameInfo g_curRgbData = {};
CameraOutputFrameInfo g_curIrData = {};
// 人脸识别
CameraOutputFrameInfo g_curRgbDataAuth = {};
CameraOutputFrameInfo g_curIrDataAuth = {};

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

bool faceDetect(const cv::Mat &frameFull, const cv::Rect &rect);

bool faceDetectInspire(const cv::Mat &frame, const cv::Mat &frameIr, cv::Rect &rectOutput, bool moreAction);

// void faceRecognition(const std::string &address, const std::string &addressIr);

void faceRecognition();

#endif //FACE_RECOGNITION_H
