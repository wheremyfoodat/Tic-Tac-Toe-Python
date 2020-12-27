#include "include/psx.h"

PSX::PSX() {
    gpu = new class GPU();
    bus = new Bus(gpu);
    cpu = new CPU(bus);
}

void PSX::step() {
    cpu -> step();
}

void PSX::render() {
    gpu -> renderer.draw();
}
