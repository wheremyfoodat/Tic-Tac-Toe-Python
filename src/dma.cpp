#include "include/bus.h"
#include "include/dma.h"

void Bus::writeToDMAControl (int channel, u32 val) {
    DMAChannels[channel].control.raw = val;
    auto isEnabled = DMAChannels[channel].isEnabled();

    if (!isEnabled) // if the DMA is not enabled, dip early
        return;

    constexpr std::array <u32, 2> offsets = {4, -4};

    auto control = DMAChannels[channel].control;
    auto syncMode = (SyncMode) control.syncMode;
    auto direction = (Direction) control.direction;
    auto device = (Device) channel;

    auto offset = offsets [control.decrement]; // if decrement bit is 0 => offset is 4. If 1 => offset is -4
    auto baseAddr = DMAChannels[channel].baseAddr;
    auto blockSettings = DMAChannels[channel].blockControl;
    s64 length; // length in words (unused for LL transfers, because that's how LLs work)

    if (syncMode == 0)
        length = (blockSettings.blockSize == 0) ? 0x10000 : blockSettings.blockSize; // if the size is 0, it gets set to 0x10000 instead

    else
        length = blockSettings.blockSize * blockSettings.blockAmount;

    switch (syncMode) {
        case SyncMode::Immediate: DMA_transferBlock(syncMode, direction, device, offset, baseAddr, length); break;
        case SyncMode::SyncToDMARequests: DMA_transferBlock(syncMode, direction, device, offset, baseAddr, length); break; // This should actually run the CPU between blocks. Tekken 3 depends on this...
        case SyncMode::LinkedList: DMA_transferLLs(direction, device, offset, baseAddr); break;
        case SyncMode::Reserved: Helpers::panic ("Illegal DMA\n"); break;
    }
}

void Bus::DMA_transferBlock (SyncMode syncMode, Direction direction, Device device, u32 offset, u32 baseAddr, s64 length) {
    Helpers::debug_printf ("DMA requested\nSync mode: %d, direction: %s, device: %d\nOffset: %d, base addr: %08X, length in words: %08X\n",
                            syncMode, direction ? "From RAM" : "To RAM", device, offset, baseAddr, length);

    if (direction == ToRAM) {
        if (device ==  Device::OrderingTableClear) {
            while (length > 0) {
                auto addr = baseAddr & 0x1F'FFFC; // Wrap around the WRAM, forcibly word-align the address
                auto val = (length != 1) ? ((addr - 4) & 0x1FFFFF) : 0xFFFFFF; // the value to write to the OT is based on the current DMA addr and it's supposed to be a pointer to the previous entry
                                                                               // the last unit however points to the end of the table (0xFF'FFFF)
                                                                               // TODO: Optimize out the conditional, unconditionally set the last transferred word to 0xFFFFFF
                *(u32*) &RAM[addr] = val; // write value to RAM
                baseAddr += offset; // increment or decrement by 4 as appropriate
                length -= 1; // decrement unit counter
            }
        }

        else
            Helpers::panic ("DMA to RAM from unknown device %d", device);
    }

    else { // DMA from RAM
        if (device == Device::GPU) {
            while (length > 0) {
                auto addr = baseAddr & 0x1F'FFFC; // Wrap around the WRAM, forcibly word-align the address
                auto val = *(u32*) &RAM[addr]; // read 32 bits
                std::printf ("Wrote %08X to GPU\n", val);

                baseAddr += offset; // increment or decrement by 4 as appropriate
                length -= 1;  // decrement unit counter
            }
        }

        else
            Helpers::panic ("DMA from RAM to unknown device: %d\n", device);
    }

    markDMAComplete((int) device); // signal that the DMA has finished
}

void Bus::DMA_transferLLs(Direction direction, Device device, u32 offset, u32 baseAddr) {
    if (direction != Direction::FromRAM || device != Device::GPU || offset == 0xFFFFFFFC) {
        Helpers::panic ("Weird Linked List DMA configuration.\nDirection: %s, device: %d, offset: %d", direction ? "From RAM" : "To RAM", device, offset);
    }

    Helpers::debug_printf ("Linked List DMA requested\nDirection: %s, device: %d\nOffset: %d, base addr: %08X\n",
                            direction ? "From RAM" : "To RAM", device, offset, baseAddr);

    baseAddr &= 0x1F'FFFC; // force word-align address, mask so that addr wraps around VRAM
    LinkedListNode node;

    while (true) { // loop won't be broken till a LL with a word count of 0xFF'FFFF gets encountered
        node.raw = *(u32*) &RAM[baseAddr]; // read a word. top byte of the word will show us how many commands to transfer to GPU
        baseAddr += 4; // increment pointer

        while (node.commandCount > 0) {
            baseAddr &= 0x1F'FFFC; // wrap addr around WRAM and force-align
            auto command = *(u32*) &RAM[baseAddr]; // read a command from memory
            std::printf ("GPU command %08X\n", command);
            gpu -> gp0_command(command);
            baseAddr += 4; // increment pointer
            node.commandCount -= 1; // decrement command count
        }

        if (node.next == 0xFF'FFFF) // 0xFF'FFFF is the marker of the last LL node
            break; // so if we encounter this, end the DMA

        baseAddr = node.next; // start sending the next LL
    }

    markDMAComplete((int) Device::GPU);
}

void Bus::markDMAComplete (int channel) {
    DMAChannels[channel].control.enable = 0; // turn off the busy bits
    DMAChannels[channel].control.trigger = 0;

    auto interruptSettings = DMAInterruptControl;
    if (interruptSettings.forceIRQ || interruptSettings.IRQMasterEnable) { // Check if IRQs enabled for DMA and panic
        Helpers::warn ("Tried to fire DMA interrupt\n");
    }
}
