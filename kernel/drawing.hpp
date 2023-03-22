#pragma once

#include <new>

#include "frame_buffer_config.hpp"

struct PixelColor {
    uint8_t r, g, b;
};

class ScreenDrawer {
   public:
    ScreenDrawer(const FrameBufferConfig& config) : config_{config} {}
    virtual ~ScreenDrawer() = default;
    virtual void Draw(int x, int y, const PixelColor& c) = 0;

   protected:
    uint8_t* pixelAt(int x, int y) {
        int bytes_per_pixel = 4;
        return config_.frame_buffer +
               bytes_per_pixel * (config_.pixels_per_scan_line * y + x);
    }

   private:
    const FrameBufferConfig& config_;
};

class RGB8BitScreenDrawer : public ScreenDrawer {
   public:
    using ScreenDrawer::ScreenDrawer;
    virtual void Draw(int x, int y, const PixelColor& c) override;
};

class BGR8BitScreenDrawer : public ScreenDrawer {
   public:
    using ScreenDrawer::ScreenDrawer;
    virtual void Draw(int x, int y, const PixelColor& c) override;
};

extern ScreenDrawer* screen_drawer;
extern FrameBufferConfig config;

void InitializeScreenDrawer(const FrameBufferConfig config);

void DrawConsoleScreen();