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

void CPU::lwl (Instruction instruction) {
    if (cop0.status.cacheIsolation) // if cache is isolated, dip early
        return;

    auto imm = Helpers::signExtend32(instruction.i.imm, 16); // sign extend imm to 32 bits
    auto addr = regs[instruction.i.rs] + imm; // addr = $rs + imm
    auto alignedRead = bus -> read32 (addr & ~3); // force align address and read value
    auto rt = regs [instruction.i.rt];

    u32 val;

    switch (addr & 3) { // Set val depending on the address alignment (& 3 fetches the bottom 2 bits, which are normally 0 in word-aligned addresses)
        case 0: val = (rt & 0x00FF'FFFF) | (alignedRead << 24); break;
        case 1: val = (rt & 0x0000'FFFF) | (alignedRead << 16); break;
        case 2: val = (rt & 0x0000'00FF) | (alignedRead << 8); break;
        case 3: val = alignedRead; break;
    }

    regs[instruction.i.rt] = val;
}

void CPU::lwr (Instruction instruction) {
    if (cop0.status.cacheIsolation) // if cache is isolated, dip early
        return;

    auto imm = Helpers::signExtend32(instruction.i.imm, 16); // sign extend imm to 32 bits
    auto addr = regs[instruction.i.rs] + imm; // addr = $rs + imm
    auto alignedRead = bus -> read32 (addr & ~3); // force align address and read value
    auto rt = regs [instruction.i.rt];

    u32 val;

    switch (addr & 3) { // Set val depending on the address alignment (& 3 fetches the bottom 2 bits, which are normally 0 in word-aligned addresses)
        case 0: val = alignedRead; break;
        case 1: val = (rt & 0xFF00'0000) | (alignedRead >> 8); break;
        case 2: val = (rt & 0xFFFF'0000) | (alignedRead >> 16); break;
        case 3: val = (rt & 0xFFFF'FF00) | (alignedRead >> 24); break;
    }

    regs[instruction.i.rt] = val;
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

void CPU::swl (Instruction instruction) {
    if (cop0.status.cacheIsolation) // if cache is isolated, dip early
        return;

    auto imm = Helpers::signExtend32(instruction.i.imm, 16); // sign extend imm to 32 bits
    auto addr = regs[instruction.i.rs] + imm; // addr = $rs + imm
    auto alignedRead = bus -> read32 (addr & ~3); // force align address and read the value that's already there
    auto rt = regs [instruction.i.rt];

    u32 val;

    switch (addr & 3) { // Set val depending on the address alignment (& 3 fetches the bottom 2 bits, which are normally 0 in word-aligned addresses)
        case 0: val = (alignedRead & 0xFFFF'FF00) | (rt >> 24); break;
        case 1: val = (alignedRead & 0xFFFF'0000) | (rt >> 16); break;
        case 2: val = (alignedRead & 0xFF00'0000) | (rt >> 8); break;
        case 3: val = rt; break;
    }

    bus -> write32 (addr & ~3, val); // write back to aligned addr
}

void CPU::swr (Instruction instruction) {
    if (cop0.status.cacheIsolation) // if cache is isolated, dip early
        return;

    auto imm = Helpers::signExtend32(instruction.i.imm, 16); // sign extend imm to 32 bits
    auto addr = regs[instruction.i.rs] + imm; // addr = $rs + imm
    auto alignedRead = bus -> read32 (addr & ~3); // force align address and read the value that's already there
    auto rt = regs [instruction.i.rt];

    u32 val;

    switch (addr & 3) { // Set val depending on the address alignment (& 3 fetches the bottom 2 bits, which are normally 0 in word-aligned addresses)
        case 0: val = rt; break;
        case 1: val = (alignedRead & 0x0000'00FF) | (rt << 8); break;
        case 2: val = (alignedRead & 0x0000'FFFF) | (rt << 16); break;
        case 3: val = (alignedRead & 0x00FF'FFFF) | (rt << 24); break;
    }

    bus -> write32 (addr & ~3, val); // write back to aligned addr
}
