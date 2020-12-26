#pragma once
#include "types.h"
#include "helpers.h"

union GPUSTAT {
    u32 raw;

    struct {
        unsigned texture_x_page: 4; // n * 64
        unsigned texture_y_page: 1; // n * 256
        unsigned semi_transparency: 2; // (0=B/2+F/2, 1=B+F, 2=B-F, 3=B+F/4)
        unsigned texture_page_colors: 2; // (0=4bit, 1=8bit, 2=15bit, 3=Reserved)

        unsigned dither: 1; // (0=Off/strip LSBs, 1=Dither Enabled)
        unsigned drawing_to_display_area: 1; // (0=Prohibited, 1=Allowed)
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

class GPU {
    GPUSTAT status;

public:
    GPU() {
        status.raw = 0x1C00'0000; // Signal that the GPU is ready to receive stuff from the CPU/DMAC
    }
};
