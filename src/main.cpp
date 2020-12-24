#include <iostream>
#include "include/psx.h"
#include <SDL.h>

#undef main

auto main(int argc, char *argv[]) -> int {
    auto psx = new PSX();

    while (true) {
        psx -> step();
    }
}
