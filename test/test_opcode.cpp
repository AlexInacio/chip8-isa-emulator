#include "Opcode.h"
#include <gtest/gtest.h>
#include <cstdint>

// ============================================================================
// Testes de Decodificação da Estrutura Opcode
// ============================================================================

// Teste Básico: Decodificação de um Opcode genérico 
TEST(OpcodeTest, GenericOpcodeDecomposition) {
    // Opcode de exemplo: 0x5AB2
    uint16_t raw_opcode = 0x5AB2;
    Opcode op(raw_opcode);

    // full: O valor completo
    ASSERT_EQ(0x5AB2, op.full);

    // category: O primeiro nibble (0x5...)
    ASSERT_EQ(0x5, op.category); 

    // x: O segundo nibble (...A..)
    ASSERT_EQ(0xA, op.x);

    // y: O terceiro nibble (....B.)
    ASSERT_EQ(0xB, op.y);

    // n: O quarto nibble (.....2)
    ASSERT_EQ(0x2, op.n);

    // nn: O último byte (..B2)
    ASSERT_EQ(0xB2, op.nn);

    // nnn: Os últimos 3 nibbles (.AB2)
    ASSERT_EQ(0xAB2, op.nnn);
}

// Teste de Limite: Opcode com todos os bits ligados (0xFFFF)
TEST(OpcodeTest, MaxValueOpcode) {
    // Opcode de exemplo: 0xFFFF
    uint16_t raw_opcode = 0xFFFF;
    Opcode op(raw_opcode);

    ASSERT_EQ(0xFFFF, op.full);
    ASSERT_EQ(0xF, op.category);
    ASSERT_EQ(0xF, op.x);
    ASSERT_EQ(0xF, op.y);
    ASSERT_EQ(0xF, op.n);
    ASSERT_EQ(0xFF, op.nn);
    ASSERT_EQ(0xFFF, op.nnn);
}

// Teste de Limite: Opcode com todos os bits desligados (0x0000)
TEST(OpcodeTest, MinValueOpcode) {
    // Opcode de exemplo: 0x0000
    uint16_t raw_opcode = 0x0000;
    Opcode op(raw_opcode);

    ASSERT_EQ(0x0000, op.full);
    ASSERT_EQ(0x0, op.category);
    ASSERT_EQ(0x0, op.x);
    ASSERT_EQ(0x0, op.y);
    ASSERT_EQ(0x0, op.n);
    ASSERT_EQ(0x00, op.nn);
    ASSERT_EQ(0x000, op.nnn);
}

// Teste de Alinhamento: Opcode que usa apenas os campos mais significativos
TEST(OpcodeTest, HighBitsOnly) {
    // Opcode de exemplo: 0x9000
    uint16_t raw_opcode = 0x9000;
    Opcode op(raw_opcode);

    ASSERT_EQ(0x9, op.category);
    // Campos menos significativos devem ser zero
    ASSERT_EQ(0x0, op.x);
    ASSERT_EQ(0x0, op.y);
    ASSERT_EQ(0x0, op.n);
    ASSERT_EQ(0x00, op.nn);
    ASSERT_EQ(0x000, op.nnn);
}

// Teste de Alinhamento: Opcode que usa apenas os campos menos significativos
TEST(OpcodeTest, LowBitsOnly) {
    // Opcode de exemplo: 0x00EE (Instrução RET)
    uint16_t raw_opcode = 0x00EE;
    Opcode op(raw_opcode);

    ASSERT_EQ(0x0, op.category);
    ASSERT_EQ(0x0, op.x);
    ASSERT_EQ(0xE, op.y); // O 'E' aqui é o y
    ASSERT_EQ(0xE, op.n);
    ASSERT_EQ(0xEE, op.nn);
    ASSERT_EQ(0x0EE, op.nnn);
}

// Teste Específico: Decodificação correta para o campo nnn (endereço)
TEST(OpcodeTest, NNN_FieldExtraction) {
    // Exemplo: 0x1CCD (Instrução JMP NNN)
    uint16_t raw_opcode = 0x1CCD;
    Opcode op(raw_opcode);

    ASSERT_EQ(0x1, op.category);
    // NNN deve ignorar o primeiro nibble (0x1) e reter 0xCCD
    ASSERT_EQ(0xCCD, op.nnn); 
}

// Teste Específico: Decodificação correta para o campo nn (constante/byte)
TEST(OpcodeTest, NN_FieldExtraction) {
    // Exemplo: 0x75A9 (Instrução ADD Vx, byte)
    uint16_t raw_opcode = 0x75A9;
    Opcode op(raw_opcode);

    ASSERT_EQ(0x7, op.category);
    ASSERT_EQ(0x5, op.x);
    // NN deve ignorar os dois primeiros nibbles (0x75) e reter 0xA9
    ASSERT_EQ(0xA9, op.nn); 
}