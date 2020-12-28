#pragma once
#include "bus.h"
#include "cpu.h"
#include "gpu.h"
#include "types.h"

struct PSX_EXE_HEADER {
    u64 keyword;
    u64 trash;

    u32 initialPC;
    u32 initialGP;
    u32 dest; // dest address of the ROM in RAM
    u32 size; // must be n * 800h

    u64 trash2;
    u64 memfillStuff;

    u32 sp_base;
    u32 sp_offs;
};

class PSX {
    Bus* bus;
    CPU* cpu;
    class GPU* gpu;

public:
    PSX(std::string directory);
    void step();
    void sideload();
    void render();
};
