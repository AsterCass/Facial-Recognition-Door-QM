#ifndef CONFIG_CAMERA_H
#define CONFIG_CAMERA_H

#define MAX_OUTPUT_FRAME_SIZE 1382400
#include <opencv2/opencv.hpp>

struct CameraOutputFrameInfo {
    // 这里值计算为 1280 * 720 * 3 /2
    unsigned char data[MAX_OUTPUT_FRAME_SIZE];
    int width;
    int height;
    int size;
};

// 人脸检测
extern CameraOutputFrameInfo g_curRgbData;
extern CameraOutputFrameInfo g_curIrData;
// 人脸识别
extern CameraOutputFrameInfo g_curRgbDataAuth;
extern CameraOutputFrameInfo g_curIrDataAuth;
// 人脸识别
extern cv::Mat g_curRgbDataMat;
extern cv::Mat g_curIrDataMat;


#endif //CONFIG_CAMERA_H
