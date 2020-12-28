#include "include/cpu.h"
#include "include/helpers.h"

void CPU::step() {
    regs[0] = 0; // set $zero to 0 on every instruction, as it's more effective than checking if the reg that's being set in an operation is $zero

    Instruction instruction; // our instruction bitfield
    instruction.raw = bus -> read32(currentPC); // we use 2 PC vars to handle delay slots and exceptions properly

    currentInstructionAddress = currentPC;
    currentPC = nextPC;
    nextPC += 4; // increment PC by 4 (size of 1 instruction)
    execute(instruction);

    inDelaySlot = executedBranch; // if last instr was branch, inDelaySlot gets set
    executedBranch = false; // clear this so inDelaySlot will get cleared in the next instruction if we're no more in a branch delay slot
}

void CPU::execute (Instruction instruction) {
    auto opcode = instruction.raw >> 26;

    switch (opcode) {
        case 0x00: executeSpecial(instruction); break;

        case 0x01: bcond(instruction); break;
        case 0x02: j(instruction); break;
        case 0x03: jal(instruction); break;
        case 0x04: beq(instruction); break;
        case 0x05: bne(instruction); break;
        case 0x06: blez(instruction); break;
        case 0x07: bgtz(instruction); break;

        case 0x08: addi(instruction); break;
        case 0x09: addiu(instruction); break;
        case 0x0A: slti(instruction); break;
        case 0x0B: sltiu(instruction); break;
        case 0x0C: andi(instruction); break;
        case 0x0D: ori(instruction); break;
        case 0x0E: xori(instruction); break;
        case 0x0F: lui(instruction); break;
        case 0x10: cop0_op(instruction); break;

        case 0x20: lb(instruction, true); break; // this one sign extends the value (LB)
        case 0x24: lb(instruction, false); break; // this one doesn't (LBU)
        case 0x21: lh(instruction, true); break; // this one sign extends the value (LH)
        case 0x25: lh(instruction, false); break; // this one doesn't (LHU)
        case 0x23: lw(instruction); break;
        case 0x22: lwl (instruction); break;
        case 0x26: lwr (instruction); break;

        case 0x28: sb(instruction); break;
        case 0x29: sh(instruction); break;
        case 0x2A: swl (instruction); break;
        case 0x2B: sw(instruction); break;
        case 0x2E: swr (instruction); break;

        default: Helpers::panic("Unknown opcode: %X\nInstruction: %08X\n", opcode, instruction.raw);
    }
}

void CPU::executeSpecial (Instruction instruction) {
    auto opcode = instruction.r.subfunction;

    switch (opcode) {
        case 0x00: sll(instruction); break;
        case 0x02: srl(instruction); break;
        case 0x03: sra(instruction); break;
        case 0x04: sllv(instruction); break;
        case 0x06: srlv(instruction); break;
        case 0x07: srav(instruction); break;
        case 0x08: jr(instruction); break;
        case 0x09: jalr(instruction); break;
        case 0x0C: syscall(); break;
        case 0x0D: op_break(); break;

        case 0x10: mfhi(instruction); break;
        case 0x11: mthi(instruction); break;
        case 0x12: mflo(instruction); break;
        case 0x13: mtlo(instruction); break;
        case 0x19: multu(instruction); break;
        case 0x1A: div(instruction); break;
        case 0x1B: divu(instruction); break;

        case 0x20: add(instruction); break;
        case 0x21: addu(instruction); break;
        case 0x23: subu(instruction); break;
        case 0x24: op_and(instruction); break;
        case 0x25: op_or(instruction); break;
        case 0x26: op_xor(instruction); break;
        case 0x27: nor(instruction); break;
        case 0x2A: slt(instruction); break;
        case 0x2B: sltu(instruction); break;
        default: Helpers::panic("Special instruction with unknown opcode: %X\n", opcode);
    }
}

void CPU::sideload_init_regs (u32 newPC, u32 newSP, u32 newGP) {
    currentPC = newPC;
    nextPC = currentPC + 4;

    regs[28] = newGP;
    regs[29] = newSP;
    regs[30] = newSP;
}
