#pragma once
#include <array>
#include "types.h"
#include "renderer.h"
#include "helpers.h"

const auto WIDTH = 1024;
const auto HEIGHT = 512;

union GPUSTAT {
    u32 raw;

    struct {
        unsigned texture_x_page: 4; // n * 64
        unsigned texture_y_page: 1; // n * 256
        unsigned semi_transparency: 2; // (0=B/2+F/2, 1=B+F, 2=B-F, 3=B+F/4)
        unsigned texture_depth: 2; // (0=4bit, 1=8bit, 2=15bit, 3=Reserved)

        unsigned dither: 1; // (0=Off/strip LSBs, 1=Dither Enabled)
        unsigned draw_to_display: 1; // (0=Prohibited, 1=Allowed)
        unsigned set_mask_bit: 1; // (0=No, 1=Yes/Mask)
        unsigned draw_pixels: 1; // (0=Always, 1=Not to Masked areas)

        unsigned interlace_field: 1; //(always 1 when GP1(08h).5=0)
        unsigned reverse_flag: 1; // (0=Normal, 1=Distorted)
        unsigned texture_disable: 1; // (0=Normal, 1=Disable Textures)
        unsigned hres2: 1; // (0=256/320/512/640, 1=368)

        unsigned hres1: 2; // (0=256, 1=320, 2=512, 3=640)
        unsigned vres: 1;  // (0=240, 1=480, when Bit22=1)
        unsigned vmode: 1; // (0=NTSC/60Hz, 1=PAL/50Hz)
        unsigned display_area_color_depth: 1; // (0=15bit, 1=24bit)

        unsigned vertical_interlace: 1; // (0=Off, 1=On)
        unsigned display_enabled: 1; // (0=Enabled, 1=Disabled)
        unsigned interrupt_request: 1; // (0=Off, 1=IRQ)
        unsigned dma_request: 1; /* meaning depends on GP1(04h) DMA Direction:
                                    When GP1(04h)=0 ---> Always zero (0)
                                    When GP1(04h)=1 ---> FIFO State  (0=Full, 1=Not Full)
                                    When GP1(04h)=2 ---> Same as GPUSTAT.28
                                    When GP1(04h)=3 ---> Same as GPUSTAT.27
                                 */
        unsigned receive_cmd_ready: 1; // 0 = no, 1 = ready
        unsigned send_vram_ready: 1;   // 0 = no, 1 = ready
        unsigned receive_dma_ready: 1; // 0 = no, 1 = ready
        unsigned dma_direction: 2; // (0=Off, 1=?, 2=CPUtoGP0, 3=GPUREADtoCPU)
        unsigned draw_odd: 1; // Drawing even/odd lines in interlace mode (0=Even or Vblank, 1=Odd)
    };
};

union GP0_cmd {
    u32 raw;

    struct {
        unsigned params: 24;
        unsigned opcode: 8;
    };

    struct {
        unsigned texture_x_page: 4;
        unsigned texture_y_page: 1;
        unsigned semi_transparency: 3;
        unsigned texture_depth: 2;
        unsigned dither: 1;
        unsigned draw_to_display: 1;
        unsigned texture_disable: 1;
        unsigned rectangle_texture_h_flip: 1;
        unsigned rectangle_texture_v_flip: 1;
        unsigned padding: 17;
    } draw_mode_params;

    GP0_cmd (u32 val) {
        raw = val;
    }
};

union GP1_cmd {
    u32 raw;

    struct {
        unsigned params: 24;
        unsigned opcode: 8;
    };

    struct {
        unsigned hres1: 2;
        unsigned vres: 1;
        unsigned vmode: 1;
        unsigned display_area_color_depth: 1;
        unsigned vertical_interlace: 1;
        unsigned hres2: 1;
        unsigned reverse_flag: 1;
        unsigned padding: 24;
    } display_mode_params;


    GP1_cmd (u32 val) {
        raw = val;
    }
};

class GPU {
public:
    GPUSTAT status;
    bool rectangle_texture_h_flip;
    bool rectangle_texture_v_flip;

    u8 texture_window_x_mask; // texture window masks (n * 8 pixels)
    u8 texture_window_y_mask;
    u8 texture_window_x_offs; // texture window offsets (n * 8 pixels)
    u8 texture_window_y_offs;

    u16 drawing_area_top; // boundaries of the display area
    u16 drawing_area_bottom;
    u16 drawing_area_left;
    u16 drawing_area_right;

    s16 vertex_x_offs; // x/y offsets applied to each vertex
    s16 vertex_y_offs;

    u16 vram_x_start; // start of the display area in VRAM
    u16 vram_y_start;

    u16 display_h_start;
    u16 display_h_end;

    u16 display_v_start;
    u16 display_v_end;

    std::array <u32, 32> commandParameters; // a buffer of parameters for GP0 commands, as those are variable-length
    u32 paramsFetched = 0; // the amount of gp0 we've fetched
    u32 paramsToFetch = 0; // the amount of gp0 params needed to fetch to execute an instruction
    u32 lastGP0Opcode = 0; // the last GP0 opcode we received (used to handle variable length instructions)

    bool fetchingGP0Params = false; // whether we're fetching GP0 params or we're ready to execute GP0 opcodes
    bool fetchingTextureData = false; // if this is 1, GP0 is fetching texture data, NOT commands

    BeegRenderer renderer; // the renderer;
    const unsigned int commandLengths[256] = {
            //0  1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
             1,  1,  3,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, //0
             1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, //1
             4,  4,  4,  4,  7,  7,  7,  7,  5,  5,  5,  5,  9,  9,  9,  9, //2
             6,  6,  6,  6,  9,  9,  9,  9,  8,  8,  8,  8, 12, 12, 12, 12, //3
             3,  3,  3,  3,  3,  3,  3,  3, 16, 16, 16, 16, 16, 16, 16, 16, //4
             4,  4,  4,  4,  4,  4,  4,  4, 16, 16, 16, 16, 16, 16, 16, 16, //5
             3,  3,  3,  1,  4,  4,  4,  4,  2,  1,  2,  1,  3,  3,  3,  3, //6
             2,  1,  2,  1,  3,  3,  3,  3,  2,  1,  2,  2,  3,  3,  3,  3, //7
             4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4, //8
             4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4, //9
             3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3, //A
             3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3, //B
             3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3, //C
             3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3, //D
             1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, //E
             1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1  //F
    };

    GPU() : renderer (WIDTH, HEIGHT, "Poopstation") { // initialize renderer
        status.raw = 0x1C00'0000; // Signal that the GPU is ready to receive stuff from the CPU/DMAC
        rectangle_texture_h_flip = false; // turn texture flipping off
        rectangle_texture_v_flip = false;

        std::fill (commandParameters.begin(), commandParameters.end(), 0); // clear command parameter buffer
        gp1_softReset(); // call the GP1 soft reset command to perform a soft reset of the GPU state
    }

    void gp0_command (u32 val);
    void gp1_command (u32 val);
    void bufferCommand (u32 val); // buffer GP0 command

    // config commands
    void gp1_softReset();
    void gp1_setDMADirection (GP1_cmd command);

    void gp0_draw_mode (GP0_cmd command);
    void gp0_set_drawing_offset (GP0_cmd command);
    void gp0_set_texture_window (GP0_cmd command);
    void gp0_set_mask_bit (GP0_cmd command);
    void gp0_set_drawing_area_top_left (GP0_cmd command);
    void gp0_set_drawing_area_bottom_right (GP0_cmd command);
    void gp0_load_texture();

    void gp1_display_mode (GP1_cmd command);
    void gp1_set_display_area_start (GP1_cmd command);
    void gp1_set_display_horizontal_range (GP1_cmd command);
    void gp1_set_display_vertical_range (GP1_cmd command);
    void gp1_display_enable (GP1_cmd command);

    // draw commands
    void quad_monochrome();
    void quad_shaded();
    void tri_shaded();
};
