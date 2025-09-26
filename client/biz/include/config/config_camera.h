#ifndef CONFIG_CAMERA_H
#define CONFIG_CAMERA_H

#define MAX_OUTPUT_FRAME_SIZE 1382400
#define CAMERA_WIDTH 1280
#define CAMERA_HEIGHT 720
#define SAVE_FRAMES 30

#include <opencv2/opencv.hpp>

struct CameraOutputFrameInfo {
    // 这里值计算为 1280 * 720 * 3 /2
    unsigned char data[MAX_OUTPUT_FRAME_SIZE];
    int width;
    int height;
    int size;
};

// 人脸识别
extern cv::Mat g_curRgbDataMat;
extern cv::Mat g_curIrDataMat;

extern cv::Mat g_curRgbDataMatAuth;
extern cv::Mat g_curIrDataMatAuth;

extern int g_appWidth;
extern int g_appHeight;


#endif //CONFIG_CAMERA_H
