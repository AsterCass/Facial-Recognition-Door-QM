#pragma once

#include <stdint.h>

#define HAL_TRANSFORM_FLIP_H     0x01
#define HAL_TRANSFORM_FLIP_V     0x02
#define HAL_TRANSFORM_ROT_90     0x04
#define HAL_TRANSFORM_ROT_180    0x03
#define HAL_TRANSFORM_ROT_270    0x07

typedef void (*display_callback_iv)(void *ptr, int size, int w, int h, uint64_t cnt);

typedef void (*display_callback)(void *ptr, int fd, int fmt, int w, int h, int rotation);

enum aiq_control_type {
    AIQ_CONTROL_RGB,
    AIQ_CONTROL_IR,
};
