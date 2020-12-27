#include "include/gpu.h"
#include "include/helpers.h"

void GPU::gp0_command(u32 val) {

    if (fetchingGP0Params) { // handle fetching GP0 commands
        commandParameters[paramsFetched++] = val;
        if (paramsFetched == paramsToFetch) { // check if the command length has been reached
            fetchingGP0Params = false; // reset parameter fetching state
            paramsFetched = 0;

            switch (lastGP0Opcode) {
                case 0x28: quad_monochrome(); break;
                case 0x30: tri_shaded(); break;
                case 0x38: quad_shaded(); break;
                case 0xA0: gp0_load_texture(); break;
                case 0x2C: Helpers::warn ("[GPU] Tried to draw textured quadrilateral with alpha blending\n");
                case 0xC0: Helpers::warn ("[GPU] Tried to send texture data to CPU\n"); break;
                default: Helpers::panic ("Unknown multi-parameter GP0 opcode: %08X\n", lastGP0Opcode);
            }
        }

        return; // don't fall through
    }

    else if (fetchingTextureData) { // handle fetching textures
        std::printf ("Received texture data word: %08X\n", val);
        paramsFetched += 1;

        if (paramsFetched == paramsToFetch) // check if word count has been reached
            fetchingTextureData = false;

        return; // don't fall through
    }

    GP0_cmd command (val);

    switch (command.opcode) {
        case 0x00: break; // NOP
        case 0x01: Helpers::warn ("[GPU] Tried to flush texture cache\n"); break;

        case 0x28: bufferCommand(val); break;
        case 0x30: bufferCommand(val); break;
        case 0x38: bufferCommand(val); break;
        case 0xA0: bufferCommand(val); break;
        case 0x2C: bufferCommand(val); break;
        case 0xC0: bufferCommand(val); break;

        case 0xE1: gp0_draw_mode (command); break;
        case 0xE2: gp0_set_texture_window(command); break;
        case 0xE3: gp0_set_drawing_area_top_left(command); break;
        case 0xE4: gp0_set_drawing_area_bottom_right(command); break;
        case 0xE5: gp0_set_drawing_offset(command); break;
        case 0xE6: gp0_set_mask_bit(command); break;
        default: Helpers::panic ("Unknown GP0 opcode %02X\n", command.opcode);
    }
}

void GPU::gp1_command(u32 val) {
    GP1_cmd command (val);

    switch (command.opcode & 0x3F) { // & 0x3F because GP1(40h..FFh) are mirrors of GP1(00h..3Fh).
        case 0x00: gp1_softReset(); break;
        case 0x01: Helpers::warn ("[GPU Tried to flush command FIFO\n");
        case 0x02: Helpers::warn ("[GPU] Tried to acknowledge interrupt\n"); status.interrupt_request = 0; break;

        case 0x03: gp1_display_enable(command); break;
        case 0x04: gp1_setDMADirection(command); break;
        case 0x05: gp1_set_display_area_start (command); break;
        case 0x06: gp1_set_display_horizontal_range(command); break;
        case 0x07: gp1_set_display_vertical_range(command); break;
        case 0x08: gp1_display_mode(command); break;
        default: Helpers::panic ("Unknown GP1 opcode %02X\n", command.opcode);
    }
}

void GPU::bufferCommand (u32 val) { // used for multi-word GPU commands, such as draw calls
    lastGP0Opcode = val >> 24; // store the opcode
    fetchingGP0Params = true; // start fetching GPU command parameters
    paramsFetched = 0;
    commandParameters[paramsFetched++] = val; // store the command in the param list
    paramsToFetch = commandLengths[lastGP0Opcode]; // the number params we need to fetch to execute this GP0 opcode
}
