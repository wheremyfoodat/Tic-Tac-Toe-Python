#include "include/cpu.h"
#include "include/types.h"
#include "include/helpers.h"

#define $ra regs[31]

void CPU::jumpRelative (u32 offset) {
    if (inDelaySlot)
        Helpers::panic("Branch in branch delay slot");
    executedBranch = true;

    offset <<= 2; // mul offset by 4
    nextPC = nextPC - 4 + offset;
}

void CPU::j(Instruction instruction) {
    if (inDelaySlot)
        Helpers::panic("Branch in branch delay slot");
    executedBranch = true;

    auto immediate = instruction.j.imm << 2; // fetch immediate, shift left by 2
    nextPC = (nextPC & 0xF000'0000) | immediate; // preserve upper nibble of PC, set the lower bits to the immediate
}

void CPU::jr(Instruction instruction) {
    if (inDelaySlot)
        Helpers::panic("Branch in branch delay slot");
    executedBranch = true;

    nextPC = regs[instruction.r.rs]; // pc = $rs
}

void CPU::jalr(Instruction instruction) { // this is literally j but with pc stored in $ra
    if (inDelaySlot)
        Helpers::panic("Branch in branch delay slot");
    executedBranch = true;

    $ra = nextPC; // store return addr
    nextPC = regs[instruction.r.rs];
}

void CPU::jal(Instruction instruction) { // this is literally j but with pc stored in $ra
    $ra = nextPC;
    j (instruction);
}

void CPU::beq (Instruction instruction) {
    auto offset = Helpers::signExtend32(instruction.i.imm, 16); // sign extend offset
    auto rs = regs [instruction.i.rs];
    auto rt = regs [instruction.i.rt];

    if (rs == rt) // branch if rs == rt
        jumpRelative(offset);
}

void CPU::bne (Instruction instruction) {
    auto offset = Helpers::signExtend32(instruction.i.imm, 16); // sign extend offset
    auto rs = regs [instruction.i.rs];
    auto rt = regs [instruction.i.rt];

    if (rs != rt) // branch if rs != rt
        jumpRelative(offset);
}

void CPU::bgtz(Instruction instruction) {
    auto offset = Helpers::signExtend32(instruction.i.imm, 16);
    auto reg = (s32) regs[instruction.i.rs];

    if (reg > 0)
        jumpRelative(offset);
}

void CPU::blez (Instruction instruction) {
    auto offset = Helpers::signExtend32(instruction.i.imm, 16);
    auto reg = (s32) regs[instruction.i.rs];

    if (reg <= 0)
        jumpRelative(offset);
}

void CPU::bcond (Instruction instruction) {
    auto offset = Helpers::signExtend32(instruction.i.imm, 16); // sign extended immediate
    auto isBGEZ = (instruction.raw >> 16) & 1; // if 1 -> BGEZ, if 0 -> BLTZ
    auto link = ((instruction.raw >> 17) & 0xF) == 8;

    auto rs = (s32) regs[instruction.i.rs]; // The reg to compare to 0
    auto shouldBranch = (rs < 0); // Assuming the instruction is BLTZ

    shouldBranch ^= isBGEZ; // if the instruction is BGEZ, flip the shouldBranch bool

    if (link) // if you should link, store the return address in $ra
        $ra = nextPC; // the address is stored even if the jump doesn't occur

    if (shouldBranch)
        jumpRelative(offset);
}
