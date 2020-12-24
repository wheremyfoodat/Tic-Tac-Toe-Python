#include "include/cpu.h"
#include "include/types.h"
#include "include/helpers.h"

void CPU::lui (Instruction instruction) {
    regs[instruction.i.rt] = instruction.i.imm << 16; // set upper halfword of $rt to imm
}

void CPU::mflo (Instruction instruction) {
    regs[instruction.r.rd] = lo;
}

void CPU::mfhi (Instruction instruction) {
    regs[instruction.r.rd] = hi;
}

void CPU::mtlo (Instruction instruction) {
    lo = regs[instruction.r.rs];
}

void CPU::mthi (Instruction instruction) {
    hi = regs[instruction.r.rs];
}

void CPU::lb (Instruction instruction, const bool signExtend) {
    if (cop0.status.cacheIsolation) // if the cache is isolated, dip
        return;

    auto imm = Helpers::signExtend32(instruction.i.imm, 16); // sign extend imm to 32 bits
    auto addr = regs[instruction.i.rs] + imm; // addr = rs + imm
    auto val = (u32) bus -> read8(addr);

    if (signExtend)
        val = Helpers::signExtend32(val, 8);

    regs[instruction.i.rt] = val;
}

void CPU::lh (Instruction instruction, const bool signExtend) {
    if (cop0.status.cacheIsolation) // if the cache is isolated, dip
        return;

    auto imm = Helpers::signExtend32(instruction.i.imm, 16); // sign extend imm to 32 bits
    auto addr = regs[instruction.i.rs] + imm; // addr = rs + imm
    auto val = (u32) bus -> read16(addr);

    if (signExtend)
        val = Helpers::signExtend32(val, 16);

    regs[instruction.i.rt] = val;
}

void CPU::lw (Instruction instruction) {
    if (cop0.status.cacheIsolation) // if the cache is isolated, dip
        return;

    auto imm = Helpers::signExtend32(instruction.i.imm, 16); // sign extend imm to 32 bits
    auto addr = regs[instruction.i.rs] + imm; // addr = rs + imm

    regs[instruction.i.rt] = bus -> read32 (addr); // $rt = mem [$rs + imm]
}

void CPU::sb (Instruction instruction) {
    if (cop0.status.cacheIsolation) // if the cache is isolated, dip
        return;

    auto imm = Helpers::signExtend32(instruction.i.imm, 16); // sign extend imm to 32 bits
    auto addr = regs[instruction.i.rs] + imm; // addr = rs + imm
    auto val = (u8) regs[instruction.i.rt];

    bus -> write8 (addr, val);
}

void CPU::sh (Instruction instruction) {
    if (cop0.status.cacheIsolation) // if the cache is isolated, dip
        return;

    auto imm = Helpers::signExtend32(instruction.i.imm, 16); // sign extend imm to 32 bits
    auto addr = regs[instruction.i.rs] + imm; // addr = rs + imm
    auto val = (u16) regs[instruction.i.rt];

    bus -> write16 (addr, val);
}

void CPU::sw (Instruction instruction) {
    if (cop0.status.cacheIsolation) // if the cache is isolated, dip
        return;

    auto imm = Helpers::signExtend32(instruction.i.imm, 16); // sign extend imm to 32 bits
    auto addr = regs[instruction.i.rs] + imm; // addr = rs + imm
    auto val = regs[instruction.i.rt];

    bus -> write32 (addr, val);
}

