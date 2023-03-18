#pragma once
#include <stdint.h>

enum PixelFormat
{
    kPixelRGB8BitPerColor,
    kPixelBGR8BitPerColor,
};

struct FrameBufferConfig
{
    uint8_t *frame_buffer;
    uint32_t pixels_per_scan_line;
    uint32_t horizon_resolution;
    uint32_t vertical_resolution;
    enum PixelFormat pixel_format;
};