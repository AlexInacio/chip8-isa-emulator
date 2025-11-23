// ============================================================================
// Opcode.h - Estrutura para decodificar opcodes
// ============================================================================
#ifndef OPCODE_H
#define OPCODE_H

#include <cstdint>

struct Opcode {
    uint16_t full;      // Opcode completo
    uint8_t category;   // Primeiro nibble (0xF000)
    uint8_t x;          // Segundo nibble (0x0F00)
    uint8_t y;          // Terceiro nibble (0x00F0)
    uint8_t n;          // Quarto nibble (0x000F)
    uint8_t nn;         // Último byte (0x00FF)
    uint16_t nnn;       // Últimos 3 nibbles (0x0FFF)
    
    Opcode(uint16_t opcode) : full(opcode) {
        category = (opcode & 0xF000) >> 12;
        x = (opcode & 0x0F00) >> 8;
        y = (opcode & 0x00F0) >> 4;
        n = opcode & 0x000F;
        nn = opcode & 0x00FF;
        nnn = opcode & 0x0FFF;
    }
};

#endif // OPCODE_H