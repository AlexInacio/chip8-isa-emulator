// ============================================================================
// InstructionSet.h - Implementação das instruções
// ============================================================================
#ifndef INSTRUCTION_SET_H
#define INSTRUCTION_SET_H

#include <random>
#include <chrono>

class Memory;
class Registers;
class Display;
class Input;
struct Opcode;

class InstructionSet {
private:
    Memory& memory;
    Registers& registers;
    Display& display;
    Input& input;
    
    std::default_random_engine rng;
    std::uniform_int_distribution<uint8_t> randByte;

public:
    InstructionSet(Memory& mem, Registers& reg, Display& disp, Input& inp)
        : memory(mem), registers(reg), display(disp), input(inp),
          rng(std::chrono::system_clock::now().time_since_epoch().count()),
          randByte(0, 255) {}
    
    void execute(const Opcode& op);

private:
    // Categorias de instruções
    void execute0xxx(const Opcode& op);
    void execute8xxx(const Opcode& op);
    void executeExxx(const Opcode& op);
    void executeFxxx(const Opcode& op);
};

#endif // INSTRUCTION_SET_H