#include <iostream>
#include "include/psx.h"
#include "include/renderer.h"

constexpr auto CYCLES_PER_FRAME = 33'868'800 / 60;

auto main(int argc, char *argv[]) -> int {
    auto psx = new PSX();

    while (true) {
        for (int i = 0; i < CYCLES_PER_FRAME; i++)
           psx -> step();
        psx -> render();
    }
}
