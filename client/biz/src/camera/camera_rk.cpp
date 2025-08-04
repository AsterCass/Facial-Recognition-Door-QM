#ifndef WIN32x

#include <rkmedia_api.h>
#include <mutex>
#include "camera/common/sample_common.h"
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
#define CAMERA_WIDTH_VI 480;
#define CAMERA_HEIGHT_VI 270;


void faceRecognitionPreFun(uchar *irFrame, uchar *rgaFrame) {
    static uchar *s_irFrame = nullptr;
    static uchar *s_rgaFrame = nullptr;
    if (nullptr != irFrame) {
        s_irFrame = irFrame;
    }
    if (nullptr != rgaFrame) {
        s_rgaFrame = rgaFrame;
    }
    if (s_irFrame == nullptr || s_rgaFrame == nullptr) {
        return;
    }

    const cv::Mat frameIr(g_appHeightIr, g_appWidthIr, CV_8UC3, s_irFrame);
    const cv::Mat frameRga(g_appHeight, g_appWidth, CV_8UC3, s_rgaFrame);
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
        faceRecognition(frameRga);
    } catch (const exception &e) {
        logPrintln("Face Recognition fail : " + string(e.what()),
                   airstrip::ERROR, __FUNCTION__);
    }

    delete [] s_irFrame;
    delete [] s_rgaFrame;
    s_irFrame = nullptr;
    s_rgaFrame = nullptr;
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

    static_cast<airstrip::ThreadPool *>(g_mainThreadPool)->enqueue(boundFunction);
    RK_MPI_MB_ReleaseBuffer(mb);
}

void processWithMbIr(MEDIA_BUFFER mb) {
    if (closeProcess)return;
    if (g_onFaceFrameIr || g_closeFaceRecognition || g_closeFaceRecognitionRegister || !g_allowFaceOpen ||
        g_isOperateOnIrFace) {
        RK_MPI_MB_ReleaseBuffer(mb);
        return;
    }
    g_onFaceFrameIr = true;

    const void *data = RK_MPI_MB_GetPtr(mb);
    const size_t size = RK_MPI_MB_GetSize(mb);
    auto *buff = new uchar[size];
    memcpy(buff, data, size);

    const cv::Mat frameIr(g_appHeightIr, g_appWidthIr, CV_8UC3, buff);
    g_currentIrFace = frameIr.clone();

    delete [] buff;
    buff = nullptr;
    RK_MPI_MB_ReleaseBuffer(mb);
    g_onFaceFrameIr = false;
}

void processWithMbRga(MEDIA_BUFFER mb) {
    if (closeProcess)return;
    if (g_onFaceFrameRga || g_closeFaceRecognition || g_closeFaceRecognitionRegister || !g_allowFaceOpen) {
        RK_MPI_MB_ReleaseBuffer(mb);
        return;
    }
    g_onFaceFrameRga = true;

    const void *data = RK_MPI_MB_GetPtr(mb);
    const size_t size = RK_MPI_MB_GetSize(mb);
    auto *buff = new uchar[size];
    memcpy(buff, data, size);


    const cv::Mat frameRga(g_appHeight, g_appWidth, CV_8UC3, buff);
    try {
        faceRecognition(frameRga);
    } catch (const exception &e) {
        logPrintln("Face Recognition fail : " + string(e.what()),
                   airstrip::ERROR, __FUNCTION__);
    }
    delete [] buff;
    buff = nullptr;
    RK_MPI_MB_ReleaseBuffer(mb);
    g_onFaceFrameRga = false;
}


// static void *process(void *) {
//     MEDIA_BUFFER irMb = nullptr;
//     MEDIA_BUFFER rgaMb = nullptr;
//
//     while (!closeProcess) {
//         irMb = RK_MPI_SYS_GetMediaBuffer(RK_ID_RGA, 1, -1);
//         rgaMb = RK_MPI_SYS_GetMediaBuffer(RK_ID_RGA, 2, -1);
//
//         if (!irMb || !rgaMb) {
//             if (irMb) {
//                 RK_MPI_MB_ReleaseBuffer(irMb);
//             }
//             if (rgaMb) {
//                 RK_MPI_MB_ReleaseBuffer(rgaMb);
//             }
//             if (!usleep(20 * 1000)) closeProcess = true;
//             continue;
//         }
//
//         if (!g_onFaceFrame) {
//             g_onFaceFrame = true;
//
//             const void *dataIr = RK_MPI_MB_GetPtr(irMb);
//             const size_t sizeIr = RK_MPI_MB_GetSize(irMb);
//             void *buffIr = malloc(sizeIr);
//             memcpy(buffIr, dataIr, sizeIr);
//
//             const void *dataRga = RK_MPI_MB_GetPtr(rgaMb);
//             const size_t sizeRga = RK_MPI_MB_GetSize(rgaMb);
//             void *buffRga = malloc(sizeRga);
//             memcpy(buffRga, dataRga, sizeRga);
//
//             if (g_mainThreadPool) {
//                 static_cast<airstrip::ThreadPool *>(g_mainThreadPool)->enqueue([buffIr, buffRga] {
//                     // Face Recognition ...
//                     free(buffIr);
//                     free(buffRga);
//                     g_onFaceFrame = false;
//                 });
//             } else {
//                 free(buffIr);
//                 free(buffRga);
//                 g_onFaceFrame = false;
//             }
//         }
//         RK_MPI_MB_ReleaseBuffer(irMb);
//         RK_MPI_MB_ReleaseBuffer(rgaMb);
//         usleep(200 * 1000);
//     }
//     return nullptr;
// }

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
    RK_MPI_SYS_Init();

    constexpr RK_S32 irCameraId = 0;
    constexpr RK_S32 rgaCameraId = 1;
    SAMPLE_COMM_ISP_Init(rgaCameraId, RK_AIQ_WORKING_MODE_NORMAL, RK_FALSE, "/etc/iqfiles");
    SAMPLE_COMM_ISP_Run(rgaCameraId);
    SAMPLE_COMM_ISP_SetFrameRate(rgaCameraId, 30);

    SAMPLE_COMM_ISP_Init(irCameraId, RK_AIQ_WORKING_MODE_NORMAL, RK_FALSE, "/etc/iqfiles");
    SAMPLE_COMM_ISP_Run(irCameraId);
    SAMPLE_COMM_ISP_SetFrameRate(irCameraId, 10);


    SAMPLE_COMM_ISP_SET_ManualExposureManualGain(1, 0, 0);


    // Init vi 0
    VI_CHN_ATTR_S vi_chn_attr;
    vi_chn_attr.pcVideoNode = "rkispp_scale0";
    vi_chn_attr.u32BufCnt = 3;
    vi_chn_attr.u32Width = CAMERA_WIDTH;
    vi_chn_attr.u32Height = CAMERA_HEIGHT;
    vi_chn_attr.enPixFmt = IMAGE_TYPE_NV12;
    vi_chn_attr.enWorkMode = VI_WORK_MODE_NORMAL;
    ret = RK_MPI_VI_SetChnAttr(rgaCameraId, 0, &vi_chn_attr);
    ret |= RK_MPI_VI_EnableChn(rgaCameraId, 0);
    if (ret) {
        logPrintln("Create vi[0] failed! ret = " + ret,
                   airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }

    // Init vi 1
    vi_chn_attr.pcVideoNode = "rkispp_scale0";
    vi_chn_attr.u32BufCnt = 3;
    vi_chn_attr.u32Width = CAMERA_WIDTH_VI;
    vi_chn_attr.u32Height = CAMERA_HEIGHT_VI;
    vi_chn_attr.enPixFmt = IMAGE_TYPE_NV12;
    vi_chn_attr.enWorkMode = VI_WORK_MODE_NORMAL;
    ret = RK_MPI_VI_SetChnAttr(irCameraId, 1, &vi_chn_attr);
    ret |= RK_MPI_VI_EnableChn(irCameraId, 1);
    if (ret) {
        logPrintln("Create vi[1] failed! ret = " + ret,
                   airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }

    // Init rga 0
    RGA_ATTR_S stRgaAttr = {};
    stRgaAttr.bEnBufPool = RK_TRUE;
    stRgaAttr.u16BufPoolCnt = 2;
    stRgaAttr.u16Rotaion = 90;
    stRgaAttr.stImgIn.u32X = 0;
    stRgaAttr.stImgIn.u32Y = 0;
    stRgaAttr.stImgIn.imgType = IMAGE_TYPE_NV12;
    stRgaAttr.stImgIn.u32Width = CAMERA_WIDTH;
    stRgaAttr.stImgIn.u32Height = CAMERA_HEIGHT;
    stRgaAttr.stImgIn.u32HorStride = CAMERA_WIDTH;
    stRgaAttr.stImgIn.u32VirStride = CAMERA_HEIGHT;
    stRgaAttr.stImgOut.u32X = 0;
    stRgaAttr.stImgOut.u32Y = 0;
    stRgaAttr.stImgOut.imgType = IMAGE_TYPE_RGB888;
    stRgaAttr.stImgOut.u32Width = g_appWidth;
    stRgaAttr.stImgOut.u32Height = g_appHeight;
    stRgaAttr.stImgOut.u32HorStride = g_appWidth;
    stRgaAttr.stImgOut.u32VirStride = g_appHeight;
    ret = RK_MPI_RGA_CreateChn(0, &stRgaAttr);
    if (ret) {
        logPrintln("Create rga[0] failed! ret = " + ret,
                   airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }

    // Init rga 1
    stRgaAttr.bEnBufPool = RK_TRUE;
    stRgaAttr.u16BufPoolCnt = 4;
    stRgaAttr.u16Rotaion = 270;
    stRgaAttr.stImgIn.u32X = 0;
    stRgaAttr.stImgIn.u32Y = 0;
    stRgaAttr.stImgIn.imgType = IMAGE_TYPE_NV12;
    stRgaAttr.stImgIn.u32Width = CAMERA_WIDTH_VI;
    stRgaAttr.stImgIn.u32Height = CAMERA_HEIGHT_VI;
    stRgaAttr.stImgIn.u32HorStride = CAMERA_WIDTH_VI;
    stRgaAttr.stImgIn.u32VirStride = CAMERA_HEIGHT_VI;
    stRgaAttr.stImgOut.u32X = 0;
    stRgaAttr.stImgOut.u32Y = 0;
    stRgaAttr.stImgOut.imgType = IMAGE_TYPE_RGB888;
    stRgaAttr.stImgOut.u32Width = g_appWidthIr;
    stRgaAttr.stImgOut.u32Height = g_appHeightIr;
    stRgaAttr.stImgOut.u32HorStride = g_appWidthIr;
    stRgaAttr.stImgOut.u32VirStride = g_appHeightIr;
    ret = RK_MPI_RGA_CreateChn(1, &stRgaAttr);
    if (ret) {
        logPrintln("Create rga[1] failed! ret = " + ret,
                   airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }

    // Init rga 2
    stRgaAttr.bEnBufPool = RK_TRUE;
    stRgaAttr.u16BufPoolCnt = 4;
    stRgaAttr.u16Rotaion = 90;
    stRgaAttr.stImgIn.u32X = 0;
    stRgaAttr.stImgIn.u32Y = 0;
    stRgaAttr.stImgIn.imgType = IMAGE_TYPE_NV12;
    stRgaAttr.stImgIn.u32Width = CAMERA_WIDTH;
    stRgaAttr.stImgIn.u32Height = CAMERA_HEIGHT;
    stRgaAttr.stImgIn.u32HorStride = CAMERA_WIDTH;
    stRgaAttr.stImgIn.u32VirStride = CAMERA_HEIGHT;
    stRgaAttr.stImgOut.u32X = 0;
    stRgaAttr.stImgOut.u32Y = 0;
    stRgaAttr.stImgOut.imgType = IMAGE_TYPE_RGB888;
    stRgaAttr.stImgOut.u32Width = g_appWidth;
    stRgaAttr.stImgOut.u32Height = g_appHeight;
    stRgaAttr.stImgOut.u32HorStride = g_appWidth;
    stRgaAttr.stImgOut.u32VirStride = g_appHeight;
    ret = RK_MPI_RGA_CreateChn(2, &stRgaAttr);
    if (ret) {
        logPrintln("Create rga[2] failed! ret = " + ret,
                   airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }


    // Init vo 0
    VO_CHN_ATTR_S stVoAttr = {};
    stVoAttr.pcDevNode = "/dev/dri/card0";
    stVoAttr.emPlaneType = VO_PLANE_OVERLAY;
    stVoAttr.enImgType = IMAGE_TYPE_RGB888;
    stVoAttr.u16Zpos = 0;
    stVoAttr.stDispRect.s32X = 0;
    stVoAttr.stDispRect.s32Y = 0;
    stVoAttr.stDispRect.u32Width = g_appWidth;
    stVoAttr.stDispRect.u32Height = g_appHeight;
    ret = RK_MPI_VO_CreateChn(0, &stVoAttr);
    if (ret) {
        logPrintln("Create vo[0] failed! ret = " + ret,
                   airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }


    // Bind
    MPP_CHN_S stSrcChn = {};
    MPP_CHN_S stDestChn = {};

    logPrintln("Bind VI[0] to RGA[0]...", airstrip::INFO, __FUNCTION__);
    stSrcChn.enModId = RK_ID_VI;
    stSrcChn.s32ChnId = 0;
    stDestChn.enModId = RK_ID_RGA;
    stDestChn.s32ChnId = 0;
    ret = RK_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (ret) {
        logPrintln("Bind vi[0] to rga[0] failed! ret = " + ret,
                   airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }

    logPrintln("Bind VI[0] to RGA[2]...", airstrip::INFO, __FUNCTION__);
    stSrcChn.enModId = RK_ID_VI;
    stSrcChn.s32ChnId = 0;
    stDestChn.enModId = RK_ID_RGA;
    stDestChn.s32ChnId = 2;
    ret = RK_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (ret) {
        logPrintln("Bind vi[0] to rga[2] failed! ret = " + ret,
                   airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }

    logPrintln("Bind VI[1] to RGA[1]...", airstrip::INFO, __FUNCTION__);
    stSrcChn.enModId = RK_ID_VI;
    stSrcChn.s32ChnId = 1;
    stDestChn.enModId = RK_ID_RGA;
    stDestChn.s32ChnId = 1;
    ret = RK_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (ret) {
        logPrintln("Bind vi[1] to rga[1] failed! ret = " + ret,
                   airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }

    logPrintln("Bind RGA[0] to VO[0]...", airstrip::INFO, __FUNCTION__);
    stSrcChn.enModId = RK_ID_RGA;
    stSrcChn.s32ChnId = 0;
    stDestChn.enModId = RK_ID_VO;
    stDestChn.s32ChnId = 0;
    ret = RK_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (ret) {
        logPrintln("Bind rga[0] to vo[0] failed! ret = " + ret,
                   airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }

    MPP_CHN_S stEncChn;
    stEncChn.enModId = RK_ID_RGA;
    stEncChn.s32DevId = 1;
    stEncChn.s32ChnId = 1;
    ret = RK_MPI_SYS_RegisterOutCb(&stEncChn, processWithMbIr);
    if (ret) {
        logPrintln("Register out cb ir failed! ret = " + ret,
                   airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }

    stEncChn.enModId = RK_ID_RGA;
    stEncChn.s32DevId = 2;
    stEncChn.s32ChnId = 2;
    ret = RK_MPI_SYS_RegisterOutCb(&stEncChn, processWithMbRga);
    if (ret) {
        logPrintln("Register out rga cb failed! ret = " + ret,
                   airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }


    // pthread_t readThread;
    // ret = pthread_create(&readThread, nullptr, process, nullptr);
    // if (ret) {
    //     logPrintln("Create monitor read thread failed! ret = " + ret,
    //                airstrip::CRITICAL, __FUNCTION__);
    //     exit(-1);
    // }

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
    int ret = 0;

    constexpr RK_S32 irCameraId = 0;
    constexpr RK_S32 rgaCameraId = 1;

    // Unbind
    MPP_CHN_S stSrcChn = {};
    MPP_CHN_S stDestChn = {};

    stSrcChn.enModId = RK_ID_VI;
    stSrcChn.s32ChnId = 0;
    stDestChn.enModId = RK_ID_RGA;
    stDestChn.s32ChnId = 0;
    ret = RK_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (ret) {
        logPrintln("Unbind vi[0] to rga[0] failed! ret = " + ret,
                   airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }

    stSrcChn.enModId = RK_ID_VI;
    stSrcChn.s32ChnId = 0;
    stDestChn.enModId = RK_ID_RGA;
    stDestChn.s32ChnId = 2;
    ret = RK_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (ret) {
        logPrintln("Unbind vi[0] to rga[2] failed! ret = " + ret,
                   airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }

    stSrcChn.enModId = RK_ID_VI;
    stSrcChn.s32ChnId = 1;
    stDestChn.enModId = RK_ID_RGA;
    stDestChn.s32ChnId = 1;
    ret = RK_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (ret) {
        logPrintln("Unbind vi[1] to rga[1] failed! ret = " + ret,
                   airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }

    stSrcChn.enModId = RK_ID_RGA;
    stSrcChn.s32ChnId = 0;
    stDestChn.enModId = RK_ID_VO;
    stDestChn.s32ChnId = 0;
    ret = RK_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (ret) {
        logPrintln("Unbind rga[0] to vo[0] failed! ret = " + ret,
                   airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }

    RK_MPI_VO_DestroyChn(0);
    RK_MPI_RGA_DestroyChn(0);
    RK_MPI_RGA_DestroyChn(1);
    RK_MPI_RGA_DestroyChn(2);
    RK_MPI_VI_DisableChn(rgaCameraId, 0);
    RK_MPI_VI_DisableChn(irCameraId, 1);

    logPrintln("Camera RK stop finish", airstrip::INFO, __FUNCTION__);

    started = false;
}

#endif
