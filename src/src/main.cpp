#include <iostream>
#include <SDL.h>
#include "include/psx.h"

#undef main

auto main(int argc, char *argv[]) -> int {
    auto psx = new PSX();

    while (true) {
        psx -> step();
    }
}
