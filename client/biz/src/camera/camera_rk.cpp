#ifndef WIN32x

#include <rkmedia_api.h>
#include <mutex>
#include "camera/camera_rk.h"
#include "airstrip_log.h"
#include <opencv2/opencv.hpp>

#include "airstrip_program_options.h"
#include "airstrip_thread_pool.h"
#include "camera/camera_frame.h"
#include "config/config.h"
#include "utils/face_recognition.h"
#include "camera/common/display.h"

using namespace std;

mutex mtx;
bool started = false;
int closeProcess = false;

int g_appWidth;
int g_appHeight;

int g_appWidthIr;
int g_appHeightIr;

int g_onFaceFrameIr = false;
int g_onFaceFrameRga = false;

#define CAMERA_WIDTH 1920;
#define CAMERA_HEIGHT 1080;
#define CAMERA_WIDTH_VI 1920;
#define CAMERA_HEIGHT_VI 1080;


void faceRecognitionPreFun(uchar *irFrame, uchar *rgaFrame) {
    // 这里加锁防止普通和红外摄像头前后脚进入，导致触发两次 faceRecognition
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);

    logPrintln("Start move data: "
               + to_string(nullptr == irFrame) + " " + to_string(nullptr == rgaFrame),
               airstrip::DEBUG, __FUNCTION__);

    static cv::Mat s_irFrame;
    static cv::Mat s_rgaFrame;

    if (irFrame) {
        cv::Mat tmp(g_appHeightIr, g_appWidthIr, CV_8UC3, irFrame);
        s_irFrame = tmp.clone(); // 深拷贝
        delete[] irFrame; // 释放原始 buffer
    }

    logPrintln("Free ir and clone",
               airstrip::DEBUG, __FUNCTION__);

    if (rgaFrame) {
        cv::Mat tmp(g_appHeight, g_appWidth, CV_8UC3, rgaFrame);
        s_rgaFrame = tmp.clone(); // 深拷贝
        delete[] rgaFrame; // 释放原始 buffer
    }

    logPrintln("Free rga and clone",
               airstrip::DEBUG, __FUNCTION__);

    if (s_irFrame.empty() || s_rgaFrame.empty()) {
        return;
    }

    logPrintln("Start faceRecognition",
               airstrip::DEBUG, __FUNCTION__);


    try {
        //rectangle(frameRga, rect, cv::Scalar(255, 0, 0), 2);
        //cv::imwrite("/data/frd/test.jpg", frameRga);

        // if (currentIsNight()) {
        //     cv::imwrite("/data/frd/current.jpg", frameRga);
        //     faceRecognition("/data/frd/current.jpg", rect);
        // } else {
        //     faceRecognition(frameRga, rect);
        // }
        //CameraFrame::getInstance()->setFaceRects(rect.x, rect.y, rect.width, rect.height);
        faceRecognition(s_rgaFrame, s_irFrame);
    } catch (const exception &e) {
        logPrintln("Face Recognition fail : " + string(e.what()),
                   airstrip::ERROR, __FUNCTION__);
    }

    logPrintln("FaceRecognition finish",
               airstrip::DEBUG, __FUNCTION__);

    s_irFrame.release();
    s_rgaFrame.release();

    logPrintln("FaceRecognition release",
               airstrip::DEBUG, __FUNCTION__);

    g_onFaceFrameRga = false;
    g_onFaceFrameIr = false;
}

void processWithMb(bool isIr, MEDIA_BUFFER mb) {
    const void *data = RK_MPI_MB_GetPtr(mb);
    const size_t size = RK_MPI_MB_GetSize(mb);
    auto *buff = new uchar[size];
    memcpy(buff, data, size);

    uchar *otherBuff = nullptr;
    auto boundFunction = isIr
                             ? bind(faceRecognitionPreFun, buff, otherBuff)
                             : bind(faceRecognitionPreFun, otherBuff, buff);

    boundFunction();
    RK_MPI_MB_ReleaseBuffer(mb);
}

void processWithMbIr(MEDIA_BUFFER mb) {
    if (closeProcess)return;
    if (g_onFaceFrameIr || g_closeFaceRecognition || g_closeFaceRecognitionRegister || !g_allowFaceOpen) {
        RK_MPI_MB_ReleaseBuffer(mb);
        return;
    }
    g_onFaceFrameIr = true;

    // 限制帧率，人脸检测频率没必要那么高，浪费cpu
    static int64_t lastMillisecondCount = 0L;
    const int64_t currentMillisecondCount =
            std::chrono::duration_cast<chrono::milliseconds>(
                chrono::system_clock::now().time_since_epoch()).
            count();
    if (currentMillisecondCount - lastMillisecondCount < 150) {
        RK_MPI_MB_ReleaseBuffer(mb);
        g_onFaceFrameIr = false;
        return;
    }
    lastMillisecondCount = currentMillisecondCount;

    processWithMb(true, mb);
}

void processWithMbRga(MEDIA_BUFFER mb) {
    if (closeProcess)return;
    if (g_onFaceFrameRga || g_closeFaceRecognition || g_closeFaceRecognitionRegister || !g_allowFaceOpen) {
        RK_MPI_MB_ReleaseBuffer(mb);
        return;
    }
    g_onFaceFrameRga = true;


    // 限制帧率，人脸检测频率没必要那么高，浪费cpu
    static int64_t lastMillisecondCount = 0L;
    const int64_t currentMillisecondCount =
            std::chrono::duration_cast<chrono::milliseconds>(
                chrono::system_clock::now().time_since_epoch()).
            count();
    if (currentMillisecondCount - lastMillisecondCount < 150) {
        RK_MPI_MB_ReleaseBuffer(mb);
        g_onFaceFrameRga = false;
        return;
    }
    lastMillisecondCount = currentMillisecondCount;

    processWithMb(false, mb);
}



void startCameraRk() {
    std::lock_guard<std::mutex> lock(mtx);
    if (started) {
        logPrintln("Camera RK has started", airstrip::WARN, __FUNCTION__);
        return;
    }

    int appWidth, appHeight;
    airstrip::getProgramOptions(PRO_OPT_APP_WIDTH, &appWidth);
    airstrip::getProgramOptions(PRO_OPT_APP_HEIGHT, &appHeight);
    g_appWidth = appWidth;
    g_appHeight = appHeight;
    g_appWidthIr = static_cast<int>(appWidth * IR_SCALE);
    g_appHeightIr = static_cast<int>(appHeight * IR_SCALE);


    display_init(0, 0);
    display_exit();

    closeProcess = false;
    int ret = 0;

    // Init
    // todo init camera


    logPrintln("Camera RK initial finish", airstrip::INFO, __FUNCTION__);
    started = true;
}

void stopCameraRk() {
    std::lock_guard<std::mutex> lock(mtx);
    if (!started) {
        logPrintln("Camera RK has stoped", airstrip::WARN, __FUNCTION__);
        return;
    }

    closeProcess = true;

    //todo close aiq


    logPrintln("Camera RK stop finish", airstrip::INFO, __FUNCTION__);

    started = false;
}

#endif
