#include "include/renderer.h"
const auto WIDTH = 1024;
const auto HEIGHT = 512;

VRAM::VRAM () {
    pixels.resize (WIDTH * HEIGHT);
}

u32 VRAM::getPixel24bit(int x, int y) {
    return pixels[x + y * WIDTH];
}

void VRAM::setPixel(int x, int y, u32 color) {
    pixels[x + y * WIDTH] = color;
}
