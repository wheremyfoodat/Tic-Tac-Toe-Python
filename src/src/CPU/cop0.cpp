#include "include/cpu.h"
#include "include/types.h"
#include "include/helpers.h"

void CPU::cop0_op (Instruction instruction) {
    switch (instruction.r.rs) { // the rs field is used to select the cop0 opcode in this instruction
        case 0b00000: mfc0(instruction); break;
        case 0b00100: mtc0(instruction); break;
        case 0b10000: rfe(instruction); break;
        default: Helpers::panic("Unknown cop0 opcode: %X\nPC: %08X\n", instruction.r.rs);
    }
}

void CPU::mtc0 (Instruction instruction) {
    auto val = regs[instruction.r.rt]; // value to be written to the cop0 register is stored in rt
    auto registerNum = instruction.r.rd; // which cop0 reg to write to

    switch (registerNum) {
        case 3: case 5: case 6: // some cop0 registers we'll ignore
        case 7: case 9: case 11:
            if (val != 0)
                Helpers::panic("Tried to use breakpoint cop0 registers");
            break;

        case 12: cop0.status.raw = val; break; // Status register

        case 13:
            if (val != 0)
                Helpers::panic("Unimplemented: CAUSE register");
            break;

        default: Helpers::panic("Wrote to unknown cop0 reg %d\n", registerNum); break;
    }
}

void CPU::mfc0 (Instruction instruction) {
    auto registerNum = instruction.r.rd;

    switch (registerNum) {
        case 12: regs[instruction.r.rt] = cop0.status.raw; break;
        case 13: regs[instruction.r.rt] = cop0.cause; break;
        case 14: regs[instruction.r.rt] = cop0.epc; break;
        default: Helpers::panic("Read from unimplemented cop0 register %d", registerNum);
    }
}
