#include <cassert>
#include "include/cpu.h"
#include "include/types.h"
#include "include/helpers.h"

void CPU::fireException(Exception exception) {
    if (inDelaySlot)
        Helpers::panic("Exception in delay slot");

    u32 vector = (cop0.status.bev) ? 0xBFC0'0180 : 0x8000'0080;

    // handle the lower 6 bits of cop0.status which are a PITA to get right
    auto cop0_interrupt_bits = cop0.status.raw & 0b11'1111;
    cop0.status.raw &= 0b11'1111;
    cop0.status.raw |= (cop0_interrupt_bits << 2) & 0b11'1111;

    cop0.cause = ((u32) exception) << 2; // set the exception type in CAUSE bits 6:2
    cop0.epc = currentInstructionAddress; // set epc to addr of current instruction

    currentPC = vector;
    nextPC = currentPC + 4;
}

void CPU::syscall() {
    printf("Syscall!\n");
    fireException(Exception::Syscall);
}

void CPU::op_break() {
    printf("Break!\n");
    fireException(Exception::Break);
}

void CPU::rfe(Instruction instruction) {
    assert((instruction.raw & 0x3F) == 0b01'0000); // if this is not true then it's MMU-related

    // undo the thing in the exception fire method
    auto cop0_interrupt_bits = cop0.status.raw & 0b11'1111;
    cop0.status.raw &= ~0b11'1111;
    cop0.status.raw |= cop0_interrupt_bits >> 2;
}
