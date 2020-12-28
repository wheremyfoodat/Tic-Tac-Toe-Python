#include "include/gpu.h"

void GPU::gp1_softReset() {
    texture_window_x_mask = 0; // texture window masks (n * 8 pixels)
    texture_window_y_mask = 0;
    texture_window_x_offs = 0; // texture window offsets (n * 8 pixels)
    texture_window_y_offs = 0;

    drawing_area_top = 0; // boundaries of the display area
    drawing_area_bottom = 0;
    drawing_area_left = 0;
    drawing_area_right = 0;

    vertex_x_offs = 0; // x/y offsets applied to each vertex
    vertex_y_offs = 0;

    vram_x_start = 0; // start of the display area in VRAM
    vram_y_start = 0;

    display_h_start = 0x200;
    display_h_end = 0xC00;

    display_v_start = 0x10;
    display_v_end = 0x100;
    status.raw = 0x1C00'0000; // turn off everything except the "ready" bits
}

void GPU::gp1_display_mode(GP1_cmd command) {
    auto params = command.display_mode_params;

    status.hres1 = params.hres1;
    status.hres2 = params.hres2;
    status.vertical_interlace = params.vertical_interlace;
    status.vres = params.vres;
    status.reverse_flag = params.reverse_flag;
    status.vmode = params.vmode;
    status.display_area_color_depth = params.display_area_color_depth;
}

void GPU::gp1_setDMADirection(GP1_cmd command) {
    status.dma_direction = command.raw & 3;
    Helpers::warn ("Set DMA direction\n");
}

void GPU::gp1_set_display_area_start(GP1_cmd command) {
    vram_x_start = command.raw & 0x3FE; // x coord's 0th bit is masked out
    vram_y_start = (command.raw >> 10) & 0x1FF;
}

void GPU::gp1_set_display_horizontal_range (GP1_cmd command) {
    display_h_start = command.raw & 0xFFF;
    display_h_end = (command.raw >> 12) & 0xFFF;
}

void GPU::gp1_set_display_vertical_range (GP1_cmd command) {
    display_h_start = command.raw & 0x3FF;
    display_h_end = (command.raw >> 10) & 0x3FF;
}

void GPU::gp1_display_enable(GP1_cmd command) {
    status.display_enabled = command.raw & 1;
}
