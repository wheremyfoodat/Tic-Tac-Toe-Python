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
    u32 length; // length in words

    if (syncMode == 0)
        length = (blockSettings.blockSize == 0) ? 0x10000 : blockSettings.blockSize; // if the size is 0, it gets set to 0x10000 instead

    else
        length = blockSettings.blockSize * blockSettings.blockAmount;

    switch (syncMode) {
        case SyncMode::Immediate: DMA_transferBlock(syncMode, direction, device, offset, baseAddr, length); break;
        case SyncMode::SyncToDMARequests: Helpers::panic ("Weird DMA mode\n"); break;
        case SyncMode::LinkedList: Helpers::panic ("Linked list DMA\n"); break;
        case SyncMode::Reserved: Helpers::panic ("Illegal DMA\n"); break;
    }
}

void Bus::DMA_transferBlock (SyncMode syncMode, Direction direction, Device device, u32 offset, u32 baseAddr, u32 length) {
    Helpers::debug_printf ("DMA requested\nSync mode: %d, direction: %s, device: %d\nOffset: %d, base addr: %08X, length in words: %08X\n",
                            syncMode, direction ? "From RAM" : "To RAM", device, offset, baseAddr, length);

    Helpers::panic("A");
}
