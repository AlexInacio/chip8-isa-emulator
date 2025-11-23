// ============================================================================
// CPU.h - Unidade Central de Processamento
// ============================================================================
#ifndef CPU_H
#define CPU_H

class CPU {
private:
    Memory& memory;
    Registers& registers;
    Display& display;
    Input& input;
    InstructionSet instructionSet;

public:
    CPU(Memory& mem, Registers& reg, Display& disp, Input& inp)
        : memory(mem), registers(reg), display(disp), input(inp),
          instructionSet(mem, reg, disp, inp) {}
    
    void cycle() {
        // Fetch
        uint16_t pc = registers.getPC();
        uint16_t opcode = (memory.read(pc) << 8) | memory.read(pc + 1);
        
        // Decode & Execute
        Opcode op(opcode);
        instructionSet.execute(op);
        
        // Atualiza timers
        registers.decrementDelayTimer();
        registers.decrementSoundTimer();
    }
    
    void reset() {
        registers.reset();
    }
};

#endif // CPU_H