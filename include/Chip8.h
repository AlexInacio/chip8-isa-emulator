// ============================================================================
// Chip8.h - Classe principal que coordena todos os componentes
// ============================================================================
#ifndef CHIP8_H
#define CHIP8_H

#include <fstream>
#include <iostream>
#include <vector>

class Chip8 {
private:
    Memory memory;
    Registers registers;
    Display display;
    Input input;
    CPU cpu;

public:
    Chip8() : cpu(memory, registers, display, input) {}
    
    void initialize() {
        memory.clear();
        memory.loadFontset();
        registers.reset();
        display.clear();
        input.clear();
    }
    
    bool loadROM(const char* filename) {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        
        if(!file.is_open()) {
            std::cerr << "Erro ao abrir ROM: " << filename << std::endl;
            return false;
        }
        
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        
        std::vector<uint8_t> buffer(size);
        if(!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
            std::cerr << "Erro ao ler ROM" << std::endl;
            return false;
        }
        
        if(!memory.loadProgram(buffer.data(), size)) {
            std::cerr << "ROM muito grande!" << std::endl;
            return false;
        }
        
        std::cout << "ROM carregada: " << size << " bytes" << std::endl;
        return true;
    }
    
    void cycle() {
        cpu.cycle();
    }
    
    // Interface pública para componentes
    const Display& getDisplay() const { return display; }
    Input& getInput() { return input; }
    const Registers& getRegisters() const { return registers; }
    bool shouldBeep() const { return registers.getSoundTimer() > 0; }
};

#endif // CHIP8_H
