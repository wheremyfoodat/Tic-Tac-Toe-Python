#pragma once
#include <vector>
#include <cstdarg>
#include <fstream>
#include "types.h"

class Helpers {
public:
    [[noreturn]] static void panic(const char* fmt, ...) {
        std::va_list args;
        va_start(args, fmt);
        std::printf ("[FATAL] ");
        std::vprintf (fmt, args);
        va_end(args);

        exit(1);
    }

    static auto loadROM(std::string directory) -> std::vector <u8> {
        std::ifstream file (directory, std::ios::binary);
        if (file.fail())
            panic("Couldn't read BIOS\n");

        std::vector<u8> ROM;

        file.unsetf(std::ios::skipws); //Απαγορευει να αγνοει το whitespace
        std::streampos fileSize;
        file.seekg(0, std::ios::end);
        fileSize = file.tellg();
        file.seekg(0, std::ios::beg); //Βρισκει το μεγεθος του ROM

        ROM.insert(ROM.begin(),
                  std::istream_iterator<uint8_t>(file),
                   std::istream_iterator<uint8_t>()); //Φορτωνει ολο το αρχειο

        file.close();

        std::cout << "ROM Loaded successfully!\n";
        return ROM;
    }

    static constexpr auto buildingInDebugMode() -> bool {
        #ifdef NDEBUG
            return false;
         #endif

         return true;
    }

    static void debug_printf (const char* fmt, ...) {
        if constexpr (buildingInDebugMode()) {
            std::va_list args;
            va_start(args, fmt);
            std::vprintf (fmt, args);
            va_end(args);
        }
    }

    static constexpr auto signExtend32 (u32 value, u32 startingSize) -> u32 {
        auto temp = (s32) value;
        auto bitsToShift = 32 - startingSize;
        return (u32) (temp << bitsToShift >> bitsToShift);
    }

    static constexpr auto isBitSet (u32 value, int bit) -> bool {
        return (value >> bit) & 1;
    }
};
