#include <rkmedia_api.h>
#include "camera/common/sample_common.h"


static bool quit = false;

static void sigterm_handler(int sig) {
    fprintf(stderr, "signal %d\n", sig);
    quit = true;
}


void startCamera() {
    int ret = 0;

    int video_width = 1920;
    int video_height = 1080;

    int disp_width = 800;
    int disp_height = 1280;

    // 0: 红外 1: rga
    RK_S32 s32CamId = 1;

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
        printf("Create vi[0] failed! ret=%d\n", ret);
        exit(-1);
    }


    RGA_ATTR_S stRgaAttr;
    memset(&stRgaAttr, 0, sizeof(stRgaAttr));
    stRgaAttr.bEnBufPool = RK_TRUE;
    stRgaAttr.u16BufPoolCnt = 2;
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
        printf("Create rga[0] falied! ret=%d\n", ret);
        exit(-1);
    }

    VO_CHN_ATTR_S stVoAttr = {0};
    // VO[0] for primary plane
    stVoAttr.pcDevNode = "/dev/dri/card0";
    stVoAttr.emPlaneType = VO_PLANE_PRIMARY;
    stVoAttr.enImgType = IMAGE_TYPE_RGB888;
    stVoAttr.u16Zpos = 0;
    stVoAttr.stDispRect.s32X = 0;
    stVoAttr.stDispRect.s32Y = 0;
    stVoAttr.stDispRect.u32Width = disp_width;
    stVoAttr.stDispRect.u32Height = disp_height;
    ret = RK_MPI_VO_CreateChn(0, &stVoAttr);
    if (ret) {
        printf("Create vo[0] failed! ret=%d\n", ret);
        exit(-1);
    }


    MPP_CHN_S stSrcChn = {};
    MPP_CHN_S stDestChn = {};

    printf("#Bind VI[0] to RGA[0]....\n");
    stSrcChn.enModId = RK_ID_VI;
    stSrcChn.s32ChnId = 0;
    stDestChn.enModId = RK_ID_RGA;
    stDestChn.s32ChnId = 0;
    ret = RK_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (ret) {
        printf("Bind vi[0] to rga[0] failed! ret=%d\n", ret);
        exit(-1);
    }


    printf("# Bind RGA[0] to VO[0]....\n");
    stSrcChn.enModId = RK_ID_RGA;
    stSrcChn.s32ChnId = 0;
    stDestChn.enModId = RK_ID_VO;
    stDestChn.s32ChnId = 0;
    ret = RK_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (ret) {
        printf("Bind rga[0] to vo[0] failed! ret=%d\n", ret);
        exit(-1);
    }


    printf("%s initial finish\n", __func__);
    signal(SIGINT, sigterm_handler);
    while (!quit) {
        usleep(500000);
    }
}
