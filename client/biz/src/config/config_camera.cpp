#include "config/config_camera.h"


// 人脸检测
CameraOutputFrameInfo g_curRgbData = {};
CameraOutputFrameInfo g_curIrData = {};
// 人脸识别
CameraOutputFrameInfo g_curRgbDataAuth = {};
CameraOutputFrameInfo g_curIrDataAuth = {};
// 人脸识别
cv::Mat g_curRgbDataMat;
cv::Mat g_curIrDataMat;
