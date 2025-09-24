#include <pthread.h>
#include "camera/common/camir_control.h"
#include <stdbool.h>
#include <stdio.h>
#include <rkaiq/rkisp_api.h>
#include <rga/rga.h>
#include <linux/media-bus-format.h>
#include <opencv2/opencv.hpp>


#include "camera/common/common.h"

static const struct rkisp_api_ctx *ctx;
static const struct rkisp_api_buf *buf;
static bool g_run;
static pthread_t g_tid;

static bool g_ir_en;
static int g_ir_width;
static int g_ir_height;
static display_callback g_display_cb = NULL;
static pthread_mutex_t g_display_lock = PTHREAD_MUTEX_INITIALIZER;
static int g_rotation = HAL_TRANSFORM_ROT_270;

void set_ir_rotation(int angle) {
    if (angle == 90)
        g_rotation = HAL_TRANSFORM_ROT_90;
    else if (angle == 270)
        g_rotation = HAL_TRANSFORM_ROT_270;
}

void set_ir_display(display_callback cb) {
    pthread_mutex_lock(&g_display_lock);
    g_display_cb = cb;
    pthread_mutex_unlock(&g_display_lock);
}

void set_ir_param(int width, int height, display_callback cb) {
    g_ir_en = true;
    g_ir_width = width;
    g_ir_height = height;
    set_ir_display(cb);
}

void getIrFrame(void *buf, int width, int height, int cnt) {
    time_t now;
    time(&now);
    static time_t lastTime = 0;
    if (now - lastTime > 5) {
        lastTime = now;

        uint8_t *yuv_data = (uint8_t *) buf;
        cv::Mat yuv(height * 3 / 2, width, CV_8UC1, yuv_data);
        cv::Mat bgr;
        cv::cvtColor(yuv, bgr, cv::COLOR_YUV2BGR_NV12);
        cv::rotate(bgr, bgr, cv::ROTATE_90_COUNTERCLOCKWISE);

        cv::imwrite("/data/frd/ir-" + std::to_string(cnt) + ".jpg", bgr);
    }
}


static void *process(void *arg) {
    static int cnt = 0;
    do {
        ++cnt;
        buf = rkisp_get_frame(ctx, 0);

        // rockface_control_convert_ir(buf->buf, ctx->width, ctx->height,
        //                             RK_FORMAT_YCbCr_420_SP, g_rotation);
        getIrFrame(buf->buf, ctx->width, ctx->height, cnt);

        pthread_mutex_lock(&g_display_lock);
        if (g_display_cb)
            g_display_cb(buf->buf, buf->fd, RK_FORMAT_YCbCr_420_SP,
                         ctx->width, ctx->height, g_rotation);
        pthread_mutex_unlock(&g_display_lock);

        rkisp_put_frame(ctx, buf);
    } while (g_run);

    pthread_exit(NULL);
}

int camir_control_init(void) {
    if (!g_ir_en)
        return 0;

    ctx = rkisp_open_device2(CAM_TYPE_RKCIF);
    if (ctx == NULL) {
        printf("%s: ctx is NULL\n", __func__);
        return -1;
    }

    rkisp_set_buf(ctx, 3, NULL, 0);

    rkisp_set_fmt(ctx, g_ir_width, g_ir_height, V4L2_PIX_FMT_NV12);

    if (rkisp_start_capture(ctx))
        return -1;


    g_run = true;
    if (pthread_create(&g_tid, NULL, process, NULL)) {
        printf("pthread_create fail\n");
        return -1;
    }

    return 0;
}
