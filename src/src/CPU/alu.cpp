#include "include/cpu.h"
#include "include/types.h"
#include "include/helpers.h"

void CPU::add (Instruction instruction) {
    auto rt = regs[instruction.r.rt];
    auto rs = regs[instruction.r.rs];

    auto res = rs + rt;
    auto overflow = ((rs ^ res) & (rt ^ res)) >> 31; // fast addition signed overflow algorithm

    if (overflow)
        Helpers::panic("Signed overflow in ADD\n");

    regs[instruction.r.rd] = res;
}

void CPU::addu (Instruction instruction) {
    regs[instruction.r.rd] = regs[instruction.r.rt] + regs[instruction.r.rs]; // $rd = $rs + $rt
}

void CPU::addiu (Instruction instruction) {
    auto immediate = Helpers::signExtend32(instruction.i.imm, 16); // sign extend imm to 32 bits
    regs[instruction.i.rt] = regs[instruction.i.rs] + immediate; // rt = rs + imm
}

void CPU::addi (Instruction instruction) {
    auto immediate = Helpers::signExtend32(instruction.i.imm, 16); // fetch and sign extend imm
    auto rs = regs[instruction.i.rs]; // fetch $rs

    auto res = rs + immediate;
    auto overflow = ((rs ^ res) & (immediate ^ res)) >> 31; // fast addition signed overflow algorithm
    regs[instruction.i.rt] = res; // $rt = rs + imm

    if (overflow)
        Helpers::panic("Overflow occured in ADDI!\n");
}

void CPU::subu (Instruction instruction) {
    regs[instruction.r.rd] = regs[instruction.r.rs] - regs[instruction.r.rt]; // $rd = $rs - $rt
}

void CPU::op_or(Instruction instruction) { // fun fact: I couldn't name this "or" because it's a keyword
    regs[instruction.r.rd] = regs[instruction.r.rt] | regs[instruction.r.rs]; // $rd = $rt | $rs
}

void CPU::ori (Instruction instruction) {
    regs[instruction.i.rt] = regs[instruction.i.rs] | instruction.i.imm; // $rt = $rs | i
}

void CPU::op_xor(Instruction instruction) { // fun fact: I couldn't name this "or" because it's a keyword
    regs[instruction.r.rd] = regs[instruction.r.rt] ^ regs[instruction.r.rs]; // $rd = $rs ^ $rt
}

void CPU::xori (Instruction instruction) {
    regs[instruction.i.rt] = regs[instruction.i.rs] ^ instruction.i.imm; // $rt = $rs ^ i
}

void CPU::nor (Instruction instruction) {
    auto res = ~(regs[instruction.r.rt] | regs[instruction.r.rs]);
    regs[instruction.r.rd] = res;
}

void CPU::op_and(Instruction instruction) { // fun fact: I couldn't name this "or" because it's a keyword
    regs[instruction.r.rd] = regs[instruction.r.rt] & regs[instruction.r.rs]; // $rd = $rt & $rs
}

void CPU::andi (Instruction instruction) {
    regs[instruction.i.rt] = regs[instruction.i.rs] & instruction.i.imm; // $rt = $rs & i
}

void CPU::sll (Instruction instruction) {
    regs[instruction.r.rd] = regs[instruction.r.rt] << instruction.r.shift_amount; // rd = rt << imm;
}

void CPU::sllv (Instruction instruction) {
    auto shiftAmount = regs[instruction.r.rs] & 0x1F; // mask by 31 to handle amount >= 32
    regs[instruction.r.rd] = regs[instruction.r.rt] << shiftAmount;
}

void CPU::srlv (Instruction instruction) {
    auto shiftAmount = regs[instruction.r.rs] & 0x1F; // mask by 31 to handle amount >= 32
    regs[instruction.r.rd] = regs[instruction.r.rt] >> shiftAmount;
}

void CPU::srav (Instruction instruction) {
    auto shiftAmount = regs[instruction.r.rs] & 0x1F; // mask by 31 to handle amount >= 32
    auto res = ((s32) regs[instruction.r.rt]) >> shiftAmount;
    regs[instruction.r.rd] = (u32) res;
}

void CPU::srl (Instruction instruction) {
    regs[instruction.r.rd] = regs[instruction.r.rt] >> instruction.r.shift_amount; // rd = rt << imm;
}

void CPU::sra (Instruction instruction) {
    regs[instruction.r.rd] = (u32) (((s32) regs[instruction.r.rt]) >> instruction.r.shift_amount); // rd = rt >> imm; (arithmetic shift right, hence the s32 cast)
}

void CPU::slti (Instruction instruction) {
    auto immediate = (s32) Helpers::signExtend32(instruction.i.imm, 16); // sign extended immediate casted to signed 32
    auto rs = (s32) regs [instruction.i.rs]; // $rs
    regs[instruction.i.rt] = rs < immediate; // if rs < imm (signed), set to 1, else 0
}

void CPU::sltiu (Instruction instruction) {
    auto immediate = Helpers::signExtend32(instruction.i.imm, 16); // sign extended immediate casted to signed 32
    auto rs = regs [instruction.i.rs]; // $rs
    regs[instruction.i.rt] = rs < immediate; // if rs < imm (signed), set to 1, else 0
}

void CPU::sltu (Instruction instruction) {
    regs[instruction.r.rd] = regs[instruction.r.rt] > regs[instruction.r.rs]; // if $rt > $rs: rd = 1. else rd = 0
}

void CPU::slt (Instruction instruction) {
    auto rt = (s32) regs[instruction.r.rt];
    auto rs = (s32) regs[instruction.r.rs];

    regs[instruction.r.rd] = (rt > rs) ? 1 : 0; // if $rt > $rs: rd = 1. else rd = 0
}

void CPU::div(Instruction instruction) {
    auto dividend = (s32) regs[instruction.r.rs];
    auto divisor = (s32) regs[instruction.r.rt];

    if (divisor == 0) { // check if division by 0
        hi = dividend; // hi gets set to the divident

        if (dividend >= 0) // lo depends on the sign of the divident
            lo = -1;
        else
            lo = 1;
    }

    else if ((u32) dividend == 0x8000'0000 && divisor == -1) { // result can't be represented in 32 bits
        hi = 0;
        lo = (u32) dividend;
    }

    else { // normal person division
        hi = (u32) (dividend % divisor);
        lo = (u32) (dividend / divisor);
    }
}

void CPU::divu(Instruction instruction) {
    auto dividend = regs[instruction.r.rs];
    auto divisor = regs[instruction.r.rt];

    if (divisor == 0) { // check if division by 0
        hi = dividend; // hi gets set to the divident
        lo = 0xFFFF'FFFF; // lo = -1
    }

    else { // normal person division
        hi = dividend % divisor;
        lo = dividend / divisor;
    }
}

void CPU::mult(Instruction instruction) {
    auto op1 = (s64) ((s32) regs[instruction.r.rs]); // sign extend to 64-bits
    auto op2 = (u64) ((s32) regs[instruction.r.rt]); // sign extend to 64-bits

    auto res = (u64) (op1 * op2);
    lo = (u32) res;
    hi = (u32) (res >> 32);
}

void CPU::multu(Instruction instruction) {
    auto op1 = (u64) regs[instruction.r.rs];
    auto op2 = (u64) regs[instruction.r.rt];

    auto res = op1 * op2;
    lo = (u32) res;
    hi = (u32) (res >> 32);
}
