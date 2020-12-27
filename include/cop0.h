#pragma once
#include "types.h"

union cop0_status {
    u32 raw;

    struct { // note: most of this field are entirely unneeded on the PSX
        unsigned IEc: 1;
        unsigned KUc: 1;
        unsigned IEp: 1;
        unsigned KUp: 1;

        unsigned IEo: 1;
        unsigned KUo: 1;

        unsigned reserved1: 2;
        unsigned interrupt_mask: 8;
        unsigned cacheIsolation: 1;

        unsigned swc: 1;
        unsigned pz: 1;
        unsigned cm: 1;
        unsigned pe: 1;
        unsigned ts: 1;
        unsigned bev: 1;

        unsigned reserved2: 2;
        unsigned re: 1; // Reverse endianness bit (doesn't exist on PSX)
        unsigned reserved3: 2;

        unsigned cu0_enable: 1; // 0 = Kernel mode, 1 = Kernel/User mode
        unsigned cu1_enable: 1; // None in the PSX
        unsigned cu2_enable: 1; // GTE
        unsigned cu3_enable: 1; // None in the pSX
    } ;
};

struct cop0_t {
    cop0_status status;
    u32 epc;
    u32 cause;

    cop0_t() {
        epc = 0;
        cause = 0;
        status.raw = 0; // initialize cop0 status
    }
};
