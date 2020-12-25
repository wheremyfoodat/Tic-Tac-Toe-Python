#pragma once
#include "helpers.h"
#include "types.h"

union DPCR_t {
    u32 raw;

    struct {
        unsigned DMA0_prio: 3;
        unsigned DMA0_enable: 1;

        unsigned DMA1_prio: 3;
        unsigned DMA1_enable: 1;

        unsigned DMA2_prio: 3;
        unsigned DMA2_enable: 1;

        unsigned DMA3_prio: 3;
        unsigned DMA3_enable: 1;

        unsigned DMA4_prio: 3;
        unsigned DMA4_enable: 1;

        unsigned DMA5_prio: 3;
        unsigned DMA5_enable: 1;

        unsigned DMA6_prio: 3;
        unsigned DMA6_enable: 1;

        unsigned unkown: 4;
    };
};

union DICR_t {
    u32 raw;

    struct {
        unsigned unknown: 6;
        unsigned unused: 9;
        unsigned forceIRQ: 1;

        unsigned DMA0IRQEnable: 1;
        unsigned DMA1IRQEnable: 1;
        unsigned DMA2IRQEnable: 1;
        unsigned DMA3IRQEnable: 1;
        unsigned DMA4IRQEnable: 1;
        unsigned DMA5IRQEnable: 1;
        unsigned DMA6IRQEnable: 1;
        unsigned IRQMasterEnable: 1;

        unsigned DMA0IRQFlag: 1;
        unsigned DMA1IRQFlag: 1;
        unsigned DMA2IRQFlag: 1;
        unsigned DMA3IRQFlag: 1;
        unsigned DMA4IRQFlag: 1;
        unsigned DMA5IRQFlag: 1;
        unsigned DMA6IRQFlag: 1;
        unsigned IRQMasterFlag: 1;
    };
};

union DMA_Channel_Control_t {
    u32 raw;

    struct {
        unsigned direction: 1; // 0 = To main RAM.  1 = From main RAM
        unsigned decrement: 1; // 0 = increment by 4, 1 = decrement by 4
        unsigned unused1: 6;
        unsigned chopping: 1; // (0=Normal, 1=Chopping; run CPU during DMA gaps)
        unsigned syncMode: 2; /*
                                0  Start immediately and transfer all at once (used for CDROM, OTC)
                                1  Sync blocks to DMA requests   (used for MDEC, SPU, and GPU-data)
                                2  Linked-List mode              (used for GPU-command-lists)
                                3  Reserved                      (not used)
                             */
        unsigned unused2: 5;
        unsigned choppingDMAWindowSize: 3; // (1 SHL N words)
        unsigned unused3: 1;
        unsigned choppingCPUWindowSize: 3; // (1 SHL N clks)
        unsigned unused4: 1;
        unsigned enable: 1; // (0=Stopped/Completed, 1=Start/Enable/Busy)
        unsigned unused5: 3;
        unsigned trigger: 1; // (0=Normal, 1=Manual Start; use for SyncMode=0)
        unsigned unknown: 3;
    };
};

union BlockControl {
    u32 raw;

    // for sync mode = 0 => low 16 bits is the number of words (0 = 1'0000h), top 16 bits unused
    // for sync mode = 1 => low 16 bits is the size of a block in words, top 16 is the amount of blocks
    // for sync mode = 2 => unused
    struct {
        unsigned blockSize: 16;
        unsigned blockAmount: 16;
    };
};

enum Direction {
    ToRAM = 0,
    FromRAM
};

// see DMA_Channel_Control_t::syncMode
enum SyncMode {
    Immediate = 0,     // transfer starts immediately
    SyncToDMARequests, // sync blocks to DMA requests
    LinkedList,        // for transferring command lists to the GPU
    Reserved           // shouldn't ever be used
};

enum Device { // The Device to target
    MDECIn = 0, // MDEC input
    MDECOut,    // MDEC output
    GPU,        // GPU
    CDROM,      // CDROM
    SPU,        // SPU
    PIO,        // Expansion port according to psx-spx. Don't know what this is used for
    OrderingTableClear // OTC (reverse clear OT) (GPU related)
};

struct DMAChannel {
    DMA_Channel_Control_t control;
    BlockControl blockControl;

    u32 baseAddr;
    int channelNumber;

    DMAChannel() {
        control.raw = 0;
        blockControl.raw = 0;
        baseAddr = 0;
        channelNumber = 0; // set in the bus constructor
    }

    auto isEnabled() -> bool {
        auto enabled = control.enable;
        auto syncMode = (SyncMode) control.syncMode;
        auto triggered = !(syncMode == SyncMode::Immediate && !control.trigger); // If the sync mode is set to immediate, the trigger bit in the control reg needs to be set

        if (enabled && triggered)
            Helpers::panic("Enabled DMA channel %d\n", channelNumber);

        return enabled && triggered;
    }
};
