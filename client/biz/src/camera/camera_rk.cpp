#ifndef WIN32x

#include <rkmedia_api.h>
#include <mutex>
#include "camera/common/sample_common.h"
#include "camera/camera_rk.h"
#include "airstrip_log.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>

#include "airstrip_thread_pool.h"
#include "camera/camera_frame.h"
#include "config/config.h"


using namespace std;

mutex mtx;
bool started = false;
int closeProcess = false;

// 0: 红外 1: rga
RK_S32 s32CamId = 1;

int disp_width = 800;
int disp_height = 1280;

static void *process(void *) {
    MEDIA_BUFFER mb = nullptr;

    while (true) {
        mb = RK_MPI_SYS_GetMediaBuffer(RK_ID_RGA, 0, -1);
        if (!mb) {
            usleep(10 * 1000);
            continue;
        }
        if (closeProcess) {
            RK_MPI_MB_ReleaseBuffer(mb);
            usleep(10 * 1000);
            break;
        }
        bool static onSendFrame = false;
        if (!onSendFrame) {
            onSendFrame = true;

            const void *data = RK_MPI_MB_GetPtr(mb);
            const size_t size = RK_MPI_MB_GetSize(mb);
            void *buff = malloc(size);
            memcpy(buff, data, size);

            if (mainThreadPool) {
                static_cast<airstrip::ThreadPool *>(mainThreadPool)->enqueue([buff] {
                    cv::Mat frame(disp_height, disp_width, CV_8UC3, buff);
                    cvtColor(frame, frame, cv::COLOR_RGB2BGR);
                    CameraFrame::getInstance()->updateFrameRK(static_cast<uchar *>(buff), disp_height, disp_width);

                    free(buff);
                    usleep(10 * 1000);
                    onSendFrame = false;
                });
            } else {
                free(buff);
                usleep(10 * 1000);
                onSendFrame = false;
            }
        }
        RK_MPI_MB_ReleaseBuffer(mb);
        usleep(10 * 1000);
    }
    return nullptr;
}

void startCameraRk() {
    std::lock_guard<std::mutex> lock(mtx);
    if (started) {
        logPrintln("Camera RK has started", airstrip::WARN, __FUNCTION__);
        return;
    }

    int ret = 0;

    int video_width = 1920;
    int video_height = 1080;

    // Init
    SAMPLE_COMM_ISP_Init(s32CamId, RK_AIQ_WORKING_MODE_NORMAL, RK_FALSE, "/etc/iqfiles");
    SAMPLE_COMM_ISP_Run(s32CamId);
    SAMPLE_COMM_ISP_SetFrameRate(s32CamId, 30);

    RK_MPI_SYS_Init();
    VI_CHN_ATTR_S vi_chn_attr;
    vi_chn_attr.pcVideoNode = "rkispp_scale0";
    vi_chn_attr.u32BufCnt = 3;
    vi_chn_attr.u32Width = video_width;
    vi_chn_attr.u32Height = video_height;
    vi_chn_attr.enPixFmt = IMAGE_TYPE_NV12;
    vi_chn_attr.enWorkMode = VI_WORK_MODE_NORMAL;
    ret = RK_MPI_VI_SetChnAttr(s32CamId, 0, &vi_chn_attr);
    ret |= RK_MPI_VI_EnableChn(s32CamId, 0);
    if (ret) {
        logPrintln("Create vi[0] failed! ret = " + ret,
                   airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }

    RGA_ATTR_S stRgaAttr = {};
    stRgaAttr.bEnBufPool = RK_TRUE;
    stRgaAttr.u16BufPoolCnt = 8;
    stRgaAttr.u16Rotaion = 90;
    stRgaAttr.stImgIn.u32X = 0;
    stRgaAttr.stImgIn.u32Y = 0;
    stRgaAttr.stImgIn.imgType = IMAGE_TYPE_NV12;
    stRgaAttr.stImgIn.u32Width = video_width;
    stRgaAttr.stImgIn.u32Height = video_height;
    stRgaAttr.stImgIn.u32HorStride = video_width;
    stRgaAttr.stImgIn.u32VirStride = video_height;
    stRgaAttr.stImgOut.u32X = 0;
    stRgaAttr.stImgOut.u32Y = 0;
    stRgaAttr.stImgOut.imgType = IMAGE_TYPE_RGB888;
    stRgaAttr.stImgOut.u32Width = disp_width;
    stRgaAttr.stImgOut.u32Height = disp_height;
    stRgaAttr.stImgOut.u32HorStride = disp_width;
    stRgaAttr.stImgOut.u32VirStride = disp_height;
    ret = RK_MPI_RGA_CreateChn(0, &stRgaAttr);
    if (ret) {
        logPrintln("Create rga[0] failed! ret = " + ret,
                   airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
    }

    // VO_CHN_ATTR_S stVoAttr = {};
    // // VO[0] for primary plane
    // stVoAttr.pcDevNode = "/dev/dri/card0";
    // stVoAttr.emPlaneType = VO_PLANE_OVERLAY;
    // stVoAttr.enImgType = IMAGE_TYPE_RGB888;
    // stVoAttr.u16Zpos = 0;
    // stVoAttr.stDispRect.s32X = 0;
    // stVoAttr.stDispRect.s32Y = 0;
    // stVoAttr.stDispRect.u32Width = disp_width;
    // stVoAttr.stDispRect.u32Height = disp_height;
    // ret = RK_MPI_VO_CreateChn(0, &stVoAttr);
    // if (ret) {
    //     logPrintln("Create vo[0] failed! ret = " + ret,
    //                airstrip::CRITICAL, __FUNCTION__);
    //     exit(-1);
    // }


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


    // logPrintln("Bind RGA[0] to VO[0]...", airstrip::INFO, __FUNCTION__);
    // stSrcChn.enModId = RK_ID_RGA;
    // stSrcChn.s32ChnId = 0;
    // stDestChn.enModId = RK_ID_VO;
    // stDestChn.s32ChnId = 0;
    // ret = RK_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    // if (ret) {
    //     logPrintln("Bind rga[0] to vo[0] failed! ret = " + ret,
    //                airstrip::CRITICAL, __FUNCTION__);
    //     exit(-1);
    // }


    pthread_t readThread;
    ret = pthread_create(&readThread, nullptr, process, nullptr);
    if (ret) {
        logPrintln("Create monitor read thread failed! ret = " + ret,
                   airstrip::CRITICAL, __FUNCTION__);
        exit(-1);
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

    int ret = 0;

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

    // stSrcChn.enModId = RK_ID_RGA;
    // stSrcChn.s32ChnId = 0;
    // stDestChn.enModId = RK_ID_VO;
    // stDestChn.s32ChnId = 0;
    // ret = RK_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    // if (ret) {
    //     logPrintln("Unbind rga[0] to vo[0] failed! ret = " + ret,
    //                airstrip::CRITICAL, __FUNCTION__);
    //     exit(-1);
    // }

    //RK_MPI_VO_DestroyChn(0);
    RK_MPI_RGA_DestroyChn(0);
    RK_MPI_VI_DisableChn(s32CamId, 0);

    logPrintln("Camera RK stop finish", airstrip::INFO, __FUNCTION__);

    started = false;
}

#endif
