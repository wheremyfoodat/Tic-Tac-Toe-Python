#include "include/types.h"
#include "include/helpers.h"
#include "include/bus.h"

Bus::Bus(class GPU* _gpu) : gpu(_gpu) {
    constexpr auto kilobyte = 1024;

    BIOS = Helpers::loadROM("D:/Repos/Top secret/TopSecret/ROMs/BIOS.bin");
    RAM.resize (2048 * kilobyte, 0);
    scratchpad.resize (kilobyte, 0);

    DMAControl.raw = 0x07654321; // default value on boot
    DMAInterruptControl.raw = 0;
    std::fill(DMAChannels.begin(), DMAChannels.end(), DMAChannel::DMAChannel()); // initialize DMA Channels

    for (int i = 0; i < 7; i++) {
        DMAChannels[i].channelNumber = i; // initialize indices
    }
}

auto Bus::read8 (u32 address) -> u8 {
    address &= REGION_MASKS[address >> 29]; // AND address with region mask

    if (address < 0x1F00'0000)
        return *(u8*) &RAM[address & 0x1F'FFFF];

    else if (address < 0x1F08'0000) { // expansion 1
        printf ("Read from unimplemented expansion 1 address %08X", address);
        return 0xFF;
    }

    else if (address >= 0x1FC0'0000 && address <= 0x1FC8'0000)
        return *(u8*) &BIOS[address & 0x7FFFF];

    else
        Helpers::panic("Read from unimplemented address %08X\n", address);
}

auto Bus::read16 (u32 address) -> u16 {
    address &= REGION_MASKS[address >> 29]; // AND address with region mask

    if (address < 0x1F00'0000)
        return *(u16*) &RAM[address & 0x1F'FFFF];

    else if (address < 0x1F08'0000) { // expansion 1
        printf ("Read from unimplemented expansion 1 address %08X", address);
        return 0xFFFF;
    }

    else if (address >= 0x1F801000 && address < 0x1F803000) {
        if (address >= 0x1F801C00 && address <= 0x1F801DC0) { // SPU regs
            return 0;
        }

        switch (address) {
        /*
            case 0x1F801DAE: printf("Read from SPU Status\n"); return 0;
            case 0x1F801DAA: printf("Read from SPU Control\n"); return 0;
            case 0x1F801D88: case 0x1F801D8A: case 0x1F801D8C: case 0x1F801D8E: printf("Read from SPU voice\n"); return 0;
            case 0x1F801DAC: printf("Read from SPU transfer control register\n"); return 0;
        */

            case 0x1F801074: printf("Read from interrupt mask reg\n"); return 0;
            case 0x1F801814: printf("Read from GPUSTAT\n"); return 0;
            default: Helpers::panic("16-bit read from unimplemented IO addr %08X\n  ", address);
        }
    }

    else if (address >= 0x1FC0'0000 && address <= 0x1FC8'0000)
        return *(u16*) &BIOS[address & 0x7FFFF];

    else
        Helpers::panic("Read from unimplemented address %08X\n", address);
}


auto Bus::read32 (u32 address) -> u32 {
    address &= REGION_MASKS[address >> 29]; // AND address with region mask

    if (address < 0x1F00'0000)
        return *(u32*) &RAM[address & 0x1F'FFFF];

    else if (address >= 0x1FC0'0000 && address <= 0x1FC8'0000)
        return *(u32*) &BIOS[address & 0x7FFFF];

    else if (address >= 0x1F801000 && address < 0x1F803000) {
        if (address >= 0x1F801C00 && address <= 0x1F801DC0) { // SPU regs
            return 0;
        }

        switch (address) {
        /*
            case 0x1F801DAE: printf("Read from SPU Status\n"); return 0;
            case 0x1F801DAA: printf("Read from SPU Control\n"); return 0;
            case 0x1F801D88: case 0x1F801D8A: case 0x1F801D8C: case 0x1F801D8E: printf("Read from SPU voice\n"); return 0;
            case 0x1F801DAC: printf("Read from SPU transfer control register\n"); return 0;
        */

            case 0x1F801074: printf("Read from interrupt mask reg\n"); return 0;
            case 0x1F8010F0: printf("Read from DPCR\n"); return DMAControl.raw;
            case 0x1F8010F4: printf("Read from DICR\n"); return DMAInterruptControl.raw;
            case 0x1F801810: printf("Read from GPUREAD (Stubbed)\n"); return 0;
            case 0x1F801814: printf("Read from GPUSTAT (Stubbed)\n"); return 0x1C00'0000; // Signal that the GPU is ready to receive stuff from the CPU/DMAC
            default: return 0;//Helpers::panic("32-bit read from unimplemented IO addr %08X\n", address);
        }
    }

    else
        Helpers::panic("Read from unimplemented address %08X\n", address);
}


void Bus::write8 (u32 address, u8 value) {
    address &= REGION_MASKS[address >> 29]; // AND address with region mask

    if (address < 0x1F00'0000)
        *(u8*) &RAM[address & 0x1F'FFFF] = value;

    //else if (address < 0x1F08'0000)
    //    *(u8*) &expansion1[address & 0x7F'FFFF] = value;

    else if (address >= 0x1F80'0000 && address <= 0x1F80'0400)
        *(u8*) &scratchpad[address & 0xFFF] = value;

    else if (address >= 0x1F80'1000 && address < 0x1F80'2000)
        printf("8-bit write to unimplemented IO address %08X (val: %02X)\n", address, value);

    else if (address >= 0x1F80'2000 && address < 0x1F80'3000) { // Expansion 2
        if (address == 0x1F802041)
            printf("[POST] Boot stage: %d\n", value);
        else
            printf("Wrote to unimplemented expansion 2 port %08X (value: %02X)\n", address, value);
    }

    else if (address >= 0xFFFE'0000) { // IO ports
        switch (address) {
            case 0xFFFE'0130: printf("Wrote to cache control\n"); break;
            default: Helpers::panic("Attempted to write %02X to IO port %08X", value, address); break;
        }
    }

    else
        Helpers::panic("Attempted to write %02X to %08X\n", value, address);
}

void Bus::write16 (u32 address, u16 value) {
    address &= REGION_MASKS[address >> 29]; // AND address with region mask

    if (address < 0x1F00'0000)
        *(u16*) &RAM[address & 0x1F'FFFF] = value;

    //else if (address < 0x1F08'0000)
    //    *(u16*) &expansion1[address & 0x7F'FFFF] = value;

    else if (address >= 0x1F80'0000 && address <= 0x1F80'0400)
        *(u16*) &scratchpad[address & 0xFFF] = value;

    else if (address >= 0x1F80'1000 && address < 0x1F80'2000)
        printf("16-bit write to unimplemented IO address %08X (val: %04X)\n", address, value);

    else if (address >= 0xFFFE'0000) { // IO ports
        switch (address) {
            case 0xFFFE'0130: printf("Wrote to cache control\n"); break;
            default: Helpers::panic("Attempted to write %04X to IO port %08X", value, address); break;
        }
    }

    else
        Helpers::panic("Attempted to write %04X to %08X\n", value, address);
}

void Bus::write32 (u32 address, u32 value) {
    address &= REGION_MASKS[address >> 29]; // AND address with region mask

    if (address < 0x1F00'0000)
        *(u32*) &RAM[address & 0x1F'FFFF] = value;

    //else if (address < 0x1F08'0000)
    //    *(u32*) &expansion1[address & 0x7F'FFFF] = value;

    else if (address >= 0x1F80'0000 && address <= 0x1F80'0400)
        *(u32*) &scratchpad[address & 0xFFF] = value;


    else if (address >= 0x1F801000 && address < 0x1F803000) {
        //if (address >= 0x1F801C00 && address <= 0x1F801DC0) { // SPU regs
            //return;
        //}

        switch (address) {

            case 0x1F801074: printf("Wrote to interrupt mask reg\n"); break;
            case 0x1F8010F0: printf("Wrote to DPCR\n"); DMAControl.raw = value; break;
            case 0x1F8010F4:
                printf("Wrote to DICR\n");
                DMAInterruptControl.raw = (DMAInterruptControl.raw & ~(value & 0x7F000000)) | (value & ~0x7F000000); // masking is to make it so that writes to the acknowledge bits actually disable the bits
                break;

            // DMA registers
            // DMA base addresses (only 24-bits are taken into account)
            case 0x1F801080: DMAChannels[0].baseAddr = value & 0xFFFFFF; break; // DMA0 Base address
            case 0x1F801090: DMAChannels[1].baseAddr = value & 0xFFFFFF; break; // DMA1 Base address
            case 0x1F8010A0: DMAChannels[2].baseAddr = value & 0xFFFFFF; break; // DMA2 Base address
            case 0x1F8010B0: DMAChannels[3].baseAddr = value & 0xFFFFFF; break; // DMA3 Base address
            case 0x1F8010C0: DMAChannels[4].baseAddr = value & 0xFFFFFF; break; // DMA4 Base address
            case 0x1F8010D0: DMAChannels[5].baseAddr = value & 0xFFFFFF; break; // DMA5 Base address
            case 0x1F8010E0: DMAChannels[6].baseAddr = value & 0xFFFFFF; break; // DMA6 Base address

            // DMA Channel block control registers
            case 0x1F801084: DMAChannels[0].blockControl.raw = value; break; // DMA0 block control register
            case 0x1F801094: DMAChannels[1].blockControl.raw = value; break; // DMA1 block register
            case 0x1F8010A4: DMAChannels[2].blockControl.raw = value; break; // DMA2 block register
            case 0x1F8010B4: DMAChannels[3].blockControl.raw = value; break; // DMA3 block register
            case 0x1F8010C4: DMAChannels[4].blockControl.raw = value; break; // DMA4 block register
            case 0x1F8010D4: DMAChannels[5].blockControl.raw = value; break; // DMA5 block register
            case 0x1F8010E4: DMAChannels[6].blockControl.raw = value; break; // DMA6 block register

            // DMA Channel control registers
            case 0x1F801088: writeToDMAControl (0, value); break; // DMA0 control register
            case 0x1F801098: writeToDMAControl (1, value); break; // DMA1 control register
            case 0x1F8010A8: writeToDMAControl (2, value); break; // DMA2 control register
            case 0x1F8010B8: writeToDMAControl (3, value); break; // DMA3 control register
            case 0x1F8010C8: writeToDMAControl (4, value); break; // DMA4 control register
            case 0x1F8010D8: writeToDMAControl (5, value); break; // DMA5 control register
            case 0x1F8010E8: writeToDMAControl (6, value); break; // DMA6 control register

            case 0x1F801810: printf("Wrote to GP0 set command (Stubbed)\nCommand: %08X\n", value); break;
            case 0x1F801814: printf("Wrote to GP1 set command (Stubbed)\nCommand: %08X\n", value); break;
            default: printf("32-bit write to unimplemented IO addr %08X\n", address); break;
        }
    }

    else if (address >= 0xFFFE'0000) { // IO ports
        switch (address) {
            case 0xFFFE'0130: printf("Wrote to cache control\n"); break;
            default: Helpers::panic("Attempted to write %08X to IO port %08X", value, address); break;
        }
    }

    else
        Helpers::panic("Attempted to write %08X to %08X\n", value, address);
}
