// ============================================================================
// Registers.h - Registradores da CPU
// ============================================================================
#ifndef REGISTERS_H
#define REGISTERS_H

#include <cstdint>
#include <cstring>

class Registers {
private:
    uint8_t V[16];          // Registradores V0-VF
    uint16_t I;             // Registrador de índice
    uint16_t PC;            // Program Counter
    uint8_t SP;             // Stack Pointer
    uint16_t stack[16];     // Stack
    uint8_t delayTimer;
    uint8_t soundTimer;

public:
    Registers() {
        reset();
    }
    
    void reset() {
        std::memset(V, 0, sizeof(V));
        std::memset(stack, 0, sizeof(stack));
        I = 0;
        PC = 0x200;
        SP = 0;
        delayTimer = 0;
        soundTimer = 0;
    }
    
    // Registradores V
    uint8_t getV(uint8_t index) const { return V[index & 0xF]; }
    void setV(uint8_t index, uint8_t value) { V[index & 0xF] = value; }
    
    // Registrador I
    uint16_t getI() const { return I; }
    void setI(uint16_t value) { I = value; }
    void addI(uint16_t value) { I += value; }
    
    // Program Counter
    uint16_t getPC() const { return PC; }
    void setPC(uint16_t value) { PC = value; }
    void incrementPC() { PC += 2; }
    void skipInstruction() { PC += 4; }
    
    // Stack
    void pushStack(uint16_t value) { 
        stack[SP++] = value; 
    }
    uint16_t popStack() { 
        return stack[--SP]; 
    }
    
    // Timers
    uint8_t getDelayTimer() const { return delayTimer; }
    void setDelayTimer(uint8_t value) { delayTimer = value; }
    void decrementDelayTimer() { if(delayTimer > 0) --delayTimer; }
    
    uint8_t getSoundTimer() const { return soundTimer; }
    void setSoundTimer(uint8_t value) { soundTimer = value; }
    void decrementSoundTimer() { if(soundTimer > 0) --soundTimer; }
};

#endif // REGISTERS_H
