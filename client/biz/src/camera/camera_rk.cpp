#ifndef WIN32x

#include <config/config_camera.h>
#include <mutex>
#include "camera/camera_rk.h"
#include "airstrip_log.h"

#include "airstrip_program_options.h"
#include "airstrip_thread_pool.h"
#include "camera/camera_frame.h"
#include "config/config.h"
#include "utils/face_recognition.h"
#include <rga/RgaApi.h>
#include "camera/common/aiq_control.h"
#include "camera/common/camir_control.h"
#include "camera/common/camrgb_control.h"
#include "camera/common/display.h"

using namespace std;

mutex mtx;
bool started = false;

int g_appWidth;
int g_appHeight;

int g_onFaceFrameIr = false;
int g_onFaceFrameRga = false;

#define CAMERA_WIDTH 1280
#define CAMERA_HEIGHT 720
#define SAVE_FRAMES 30


void faceRecognitionPreFun(void *irFrame, void *rgaFrame, int width, int height) {
    // 这里加锁防止普通和红外摄像头前后脚进入，导致触发两次 faceRecognition
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);

    logPrintln("Start move data: "
               + to_string(nullptr == irFrame) + " " + to_string(nullptr == rgaFrame),
               airstrip::DEBUG, __FUNCTION__);

    const int calSize = width * height * 3 / 2;

    if (irFrame && MAX_OUTPUT_FRAME_SIZE >= calSize) {
        g_curIrData.size = calSize;
        g_curIrData.height = height;
        g_curIrData.width = width;
        memcpy(g_curIrData.data, irFrame, calSize);
    }

    logPrintln("Free ir and clone",
               airstrip::DEBUG, __FUNCTION__);

    if (rgaFrame && MAX_OUTPUT_FRAME_SIZE >= calSize) {
        g_curRgbData.size = calSize;
        g_curRgbData.height = height;
        g_curRgbData.width = width;
        memcpy(g_curRgbData.data, rgaFrame, calSize);
    }

    logPrintln("Free rga and clone",
               airstrip::DEBUG, __FUNCTION__);

    if (0 == g_curIrData.size || 0 == g_curRgbData.size) {
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
        faceRecognition();
    } catch (const exception &e) {
        logPrintln("Face Recognition fail : " + string(e.what()),
                   airstrip::ERROR, __FUNCTION__);
    }

    logPrintln("FaceRecognition finish",
               airstrip::DEBUG, __FUNCTION__);

    memset(&g_curRgbData, 0, sizeof(g_curRgbData));
    memset(&g_curIrData, 0, sizeof(g_curIrData));

    logPrintln("FaceRecognition release",
               airstrip::DEBUG, __FUNCTION__);

    g_onFaceFrameRga = false;
    g_onFaceFrameIr = false;
}

void processWithMbIr(void *buf, int size, int width, int height, uint64_t cnt) {
    if (g_onFaceFrameIr || g_closeFaceRecognition || g_closeFaceRecognitionRegister || !g_allowFaceOpen) {
        return;
    }
    g_onFaceFrameIr = true;

    // 30帧数据，每5帧调用一次
    static uint64_t lastCnt = 0;
    if (cnt - lastCnt < 5) {
        g_onFaceFrameIr = false;
        return;
    }
    lastCnt = cnt;

    logPrintln("The ir data : " + to_string(size) + " " + to_string(width) + " " + to_string(height),
               airstrip::DEBUG, __FUNCTION__);

    // int calSize = width * height * 3 / 2;
    //
    // auto *newBuf = new uchar[calSize];
    // memcpy(newBuf, buf, calSize);
    //
    // // new thread do somthing
    // {
    //     delete[] newBuf;
    // }

    faceRecognitionPreFun(buf, nullptr, width, height);
}

void processWithMbRga(void *buf, int size, int width, int height, uint64_t cnt) {
    if (g_onFaceFrameRga || g_closeFaceRecognition || g_closeFaceRecognitionRegister || !g_allowFaceOpen) {
        return;
    }
    g_onFaceFrameRga = true;


    // 30帧数据，每5帧调用一次
    static uint64_t lastCnt = 0;
    if (cnt - lastCnt < 5) {
        g_onFaceFrameRga = false;
        return;
    }
    lastCnt = cnt;

    logPrintln("The rga data : " + to_string(size) + " " + to_string(width) + " " + to_string(height),
               airstrip::DEBUG, __FUNCTION__);


    // int calSize = width * height * 3 / 2;
    //
    // auto *newBuf = new uchar[calSize];
    // memcpy(newBuf, buf, calSize);
    //
    // // new thread do somthing
    // {
    //     delete[] newBuf;
    // }

    faceRecognitionPreFun(nullptr, buf, width, height);
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

    // Init
    // 这里回调会在 display_switch 被输出屏幕的方法占用，所以不在这里设置
    set_rgb_param(CAMERA_WIDTH,CAMERA_HEIGHT, nullptr, true);
    set_ir_param(CAMERA_WIDTH,CAMERA_HEIGHT, nullptr);
    set_rgb_rotation(90);

    set_ir_display_iv(processWithMbIr);
    set_rgb_display_iv(processWithMbRga);

    display_switch(DISPLAY_VIDEO_RGB);
    if (display_init(g_appWidth, g_appHeight)) {
        logPrintln("Display init failed", airstrip::ERROR, __FUNCTION__);
        return;
    }

    logPrintln("Init display finish", airstrip::INFO, __FUNCTION__);

    if (c_RkRgaInit()) {
        logPrintln("Rga init failed", airstrip::ERROR, __FUNCTION__);
        return;
    }

    aiq_control_alloc();
    for (int i = 0; i < 10; i++) {
        if (aiq_control_get_status(AIQ_CONTROL_RGB)) {
            logPrintln("RGB aiq status ok.", airstrip::INFO, __FUNCTION__);
            camrgb_control_init();
            break;
        }
        sleep(1);
    }

    for (int i = 0; i < 10; i++) {
        if (aiq_control_get_status(AIQ_CONTROL_IR)) {
            logPrintln("IR aiq status ok.", airstrip::INFO, __FUNCTION__);
            camir_control_init();
            break;
        }
        sleep(1);
    }


    logPrintln("Camera RK initial finish", airstrip::INFO, __FUNCTION__);
    started = true;
}

void stopCameraRk() {
    std::lock_guard<std::mutex> lock(mtx);
    if (!started) {
        logPrintln("Camera RK has stoped", airstrip::WARN, __FUNCTION__);
        return;
    }

    //todo close aiq


    logPrintln("Camera RK stop finish", airstrip::INFO, __FUNCTION__);

    started = false;
}

#endif
