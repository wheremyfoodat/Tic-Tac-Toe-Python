#pragma once
#include <array>
#include "types.h"
#include "bus.h"
#include "cop0.h"

union Instruction {
    u32 raw;

    struct {
        unsigned imm: 16;
        unsigned rt: 5;
        unsigned rs: 5;
        unsigned opcode: 6;
    } i; // for i-type instructions

    struct {
        unsigned subfunction: 6;
        unsigned shift_amount: 5;
        unsigned rd: 5;
        unsigned rt: 5;
        unsigned rs: 5;
        unsigned opcode: 6;
    } r; // for r-type instructions

    struct {
        unsigned imm: 26;
        unsigned opcode: 6;
    } j; // for j-type opcodes
};

enum Exception {
    Interrupt = 0,
    LoadAddressError = 4,
    StoreAddressError = 5,
    Syscall = 8,
    Break = 9,
    IllegalInstruction = 10,
    CoprocessorError = 11,
    Overflow = 12
};

class CPU {
    std::array <u32, 32> regs;
    u32 currentPC;
    u32 nextPC;
    u32 currentInstructionAddress; // used for exceptions
    Bus* bus;
    cop0_t cop0;

    u32 hi; // used in div/mul operations
    u32 lo; // used in div/mul operations

    bool executedBranch;
    bool inDelaySlot;
    bool DEBUG_VAR_REMOVE_LATER;

    void execute (Instruction instruction);
    void executeSpecial (Instruction instruction);
    void fireException(Exception exception);
    void rfe(Instruction instruction);
    void syscall();
    void op_break();

    void op_and(Instruction instruction);
    void op_or (Instruction instruction);
    void op_xor (Instruction instruction);

    void add (Instruction instruction);
    void addi (Instruction instruction);
    void addu (Instruction instruction);
    void addiu (Instruction instruction);
    void subu (Instruction instruction);

    void lb  (Instruction instruction, const bool signExtend);
    void lh  (Instruction instruction, const bool signExtend);
    void lw  (Instruction instruction);
    void sb  (Instruction instruction);
    void sh  (Instruction instruction);
    void sw  (Instruction instruction);

    void andi (Instruction instruction);
    void ori (Instruction instruction);
    void xori (Instruction instruction);
    void lui (Instruction instruction);
    void sll (Instruction instruction);
    void sllv (Instruction instruction);
    void srlv (Instruction instruction);
    void srav (Instruction instruction);
    void srl (Instruction instruction);
    void sra (Instruction instruction);
    void slti (Instruction instruction);
    void sltiu (Instruction instruction);
    void slt (Instruction instruction);
    void sltu (Instruction instruction);
    void nor (Instruction instruction);

    void mflo (Instruction instruction);
    void mtlo (Instruction instruction);
    void mfhi (Instruction instruction);
    void mthi (Instruction instruction);
    void div (Instruction instruction);
    void divu (Instruction instruction);
    void mult (Instruction instruction);
    void multu (Instruction instruction);

    void jumpRelative (u32 offset);
    void j (Instruction instruction);
    void jr (Instruction instruction);
    void jal (Instruction instruction);
    void jalr (Instruction instruction);
    void beq (Instruction instruction);
    void bne (Instruction instruction);
    void bgtz (Instruction instruction);
    void blez (Instruction instruction);
    void bcond(Instruction instruction);

    void cop0_op (Instruction instruction);
    void mtc0 (Instruction instruction);
    void mfc0 (Instruction instruction);

public:
    CPU (Bus* _bus) : bus(_bus) {
        std::fill(regs.begin(), regs.end(), 0); // zero-fill registers
        hi = 0;
        lo = 0;
        DEBUG_VAR_REMOVE_LATER = false;

        currentPC = 0xBFC0'0000; // BIOS start
        nextPC = currentPC + 4;

        executedBranch = false;
        inDelaySlot = false;
    };

    void step();
};
