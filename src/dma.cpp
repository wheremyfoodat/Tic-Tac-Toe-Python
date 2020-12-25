#include "include/bus.h"

void Bus::writeToDMAControl (int channel, u32 val) {
    DMAChannels[channel].control.raw = val;
    auto isEnabled = DMAChannels[channel].isEnabled();
}
