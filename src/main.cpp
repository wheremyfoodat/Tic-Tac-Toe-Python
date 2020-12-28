#include <iostream>
#include <chrono>
#include "include/psx.h"
#include "include/renderer.h"

constexpr auto CYCLES_PER_FRAME = 33'868'800 / 60 / 2;

auto main(int argc, char *argv[]) -> int {
    auto psx = new PSX ("D:/Repos/Top secret/TopSecret/ROMs/CPUDIV.exe");
    // psx -> sideload();

    while (true) {
        //auto start = std::chrono::system_clock::now();

        for (int i = 0; i < CYCLES_PER_FRAME; i++)
           psx -> step();
        psx -> render();

        //auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count();
        //std::cout << "Frame time: " << millis << "\n";
    }
}
