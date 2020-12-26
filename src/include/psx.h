#pragma once
#include "bus.h"
#include "cpu.h"
#include "gpu.h"
#include "types.h"

class PSX {
    Bus* bus;
    CPU* cpu;
    class GPU* gpu;

public:
    PSX();
    void step();
};
