#include "include/psx.h"
#include "include/helpers.h"

PSX::PSX(std::string directory) {
    gpu = new class GPU();
    bus = new Bus(gpu);
    cpu = new CPU(bus);

    auto ROM = Helpers::loadROM (directory);
    bus -> ROM = ROM;
}

void PSX::step() {
    cpu -> step();
}

void PSX::render() {
    gpu -> renderer.draw();
}

void PSX::sideload() {
    auto exe_header = (PSX_EXE_HEADER*) bus -> ROM.data();

    if (exe_header -> keyword != 0x45584520582D5350) { // The magic value for the string 'PS-X EXE' in ASCII
        Helpers::panic ("Invalid PSX exe\n");
    }

    auto initialPC = exe_header -> initialPC;
    auto initialGP = exe_header -> initialGP;
    auto initialSP = exe_header -> sp_base + exe_header -> sp_offs;

    cpu -> sideload_init_regs(initialPC, initialSP, initialGP);
    for (int i = 0; i < exe_header -> size; i++) {
        auto val = bus -> ROM[0x800 + i];
        bus -> write8 (exe_header -> dest + i, val);
    }
}
