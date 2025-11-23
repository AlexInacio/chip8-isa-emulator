// ============================================================================
// Memory.h - Gerenciamento de memória
// ============================================================================
#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <cstring>

class Memory {
private:
    static constexpr size_t MEMORY_SIZE = 4096;
    static constexpr size_t FONT_START = 0x000;
    static constexpr size_t PROGRAM_START = 0x200;
    
    uint8_t data[MEMORY_SIZE];
    
    static constexpr uint8_t FONTSET[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

public:
    Memory() {
        clear();
        loadFontset();
    }
    
    void clear() {
        std::memset(data, 0, MEMORY_SIZE);
    }
    
    void loadFontset() {
        for(size_t i = 0; i < 80; ++i) {
            data[FONT_START + i] = FONTSET[i];
        }
    }
    
    uint8_t read(uint16_t address) const {
        return data[address & 0xFFF];
    }
    
    void write(uint16_t address, uint8_t value) {
        data[address & 0xFFF] = value;
    }
    
    bool loadProgram(const uint8_t* program, size_t size) {
        if(size > (MEMORY_SIZE - PROGRAM_START)) {
            return false;
        }
        std::memcpy(&data[PROGRAM_START], program, size);
        return true;
    }
    
    static constexpr uint16_t getProgramStart() { return PROGRAM_START; }
};

#endif // MEMORY_H