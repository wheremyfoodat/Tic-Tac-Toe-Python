#pragma once
#include "bus.h"
#include "cpu.h"
#include "types.h"

class PSX {
    Bus* bus;
    CPU* cpu;

public:
    PSX();
    void step();
};
