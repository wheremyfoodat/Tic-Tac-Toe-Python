#include "include/gpu.h"
#include "include/helpers.h"

void GPU::gp0_command(u32 val) {
    GP0_cmd command (val);

    switch (command.opcode) {
        case 0x00: break; // NOP
        case 0xE1: gp0_draw_mode (command); break;
        case 0xE3: gp0_set_drawing_area_top_left(command); break;
        case 0xE4: gp0_set_drawing_area_bottom_right(command); break;
        case 0xE5: gp0_set_drawing_offset(command); break;
        default: Helpers::panic ("Unknown GP0 opcode %02X\n", command.opcode);
    }
}

void GPU::gp1_command(u32 val) {
    GP1_cmd command (val);

    switch (command.opcode & 0x3F) { // & 0x3F because GP1(40h..FFh) are mirrors of GP1(00h..3Fh).
        case 0x00: gp1_softReset(); break;
        case 0x04: gp1_setDMADirection(command); break;
        case 0x08: gp1_display_mode(command); break;
        default: Helpers::panic ("Unknown GP1 opcode %02X\n", command.opcode);
    }
}
