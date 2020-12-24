#include "include/psx.h"

PSX::PSX() {
    bus = new Bus();
    cpu = new CPU(bus);
}

void PSX::step() {
    cpu -> step();
}
