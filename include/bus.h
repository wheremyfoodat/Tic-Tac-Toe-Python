#pragma once
#include <array>
#include <vector>
#include "types.h"
#include "dma.h"

class Bus {
    const std::array <u32, 8> REGION_MASKS = {
        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,  // KUSEG (2048MB)
        0x7FFFFFFF,                                      // KSEG0 (512 MB)
        0x1FFFFFFF,                                      // KSEG1 (512 MB)
        0xFFFFFFFF, 0xFFFFFFFF,                          // KSEG2 (1024 MB)
    };

    std::vector<u8> ROM;
    std::vector<u8> RAM;
    std::vector<u8> expansion1;
    std::vector<u8> scratchpad;
    std::vector<u8> BIOS;

    // DMA stuff
    DPCR_t DMAControl;
    DICR_t DMAInterruptControl;
    std::array <DMAChannel, 7> DMAChannels;

    void writeToDMAControl(int channel, u32 val);

public:
    u8 read8 (u32 address);
    u16 read16 (u32 address);
    u32 read32 (u32 address);

    void write8  (u32 address, u8 value);
    void write16 (u32 address, u16 value);
    void write32 (u32 address, u32 value);
    Bus();
};
