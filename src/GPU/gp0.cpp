#include "include/gpu.h"
#include "include/helpers.h"

void GPU::gp0_set_drawing_area_top_left (GP0_cmd command) {
    drawing_area_left = command.raw & 0x3FF;
    drawing_area_top = (command.raw >> 10) & 0x3FF;
}

void GPU::gp0_set_drawing_area_bottom_right(GP0_cmd command) {
    drawing_area_right = command.raw & 0x3FF;
    drawing_area_bottom = (command.raw >> 10) & 0x3FF;
}

void GPU::gp0_set_drawing_offset (GP0_cmd command) {
    vertex_x_offs = (s16) Helpers::signExtend16(command.raw & 0x7FF, 11); // sign extend the 11-bit x offset value to 16 bits and store it in vertex_x_offs
    vertex_y_offs = (s16) Helpers::signExtend16((command.raw >> 11) & 0x7FF, 11); // likewise for y
}

void GPU::gp0_set_texture_window (GP0_cmd command) {
    texture_window_x_mask = command.raw & 0x1F;
    texture_window_y_mask = (command.raw >> 5) & 0x1F;

    texture_window_x_offs = (command.raw >> 10) & 0x1F;
    texture_window_y_offs = (command.raw >> 15) & 0x1F;
}

void GPU::gp0_set_mask_bit (GP0_cmd command) {
    status.set_mask_bit = command.raw & 1;
    status.draw_pixels = (command.raw >> 1) & 1;
}

void GPU::gp0_draw_mode(GP0_cmd command) {
    auto params = command.draw_mode_params;

    status.texture_x_page = params.texture_x_page;
    status.texture_y_page = params.texture_x_page;
    status.semi_transparency = params.semi_transparency;
    status.texture_depth = params.texture_depth;

    status.dither = params.dither;
    status.draw_to_display = params.draw_to_display;
    status.texture_disable = params.texture_disable;

    rectangle_texture_h_flip = params.rectangle_texture_h_flip;
    rectangle_texture_v_flip = params.rectangle_texture_v_flip;
}

void GPU::gp0_load_texture() {
    fetchingTextureData = true;
    paramsFetched = 0; // this will now be used as the number of halfwords that have been fetched

    auto dest = commandParameters[1];
    u32 dimensions = commandParameters[2];

    auto x_size = dimensions & 0xFFFF;
    auto y_size = dimensions >> 16;

    auto size = x_size * y_size; // size in halfwords (1 halfword = 1 pixel)
    size += size & 1; // if size is odd, add 1 more halfword
    paramsToFetch = size >> 1; // fetch (size / 2) words

}
