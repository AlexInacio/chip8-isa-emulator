#include "Registers.h"
#include <gtest/gtest.h>
#include <cstdint>

// Estrutura de Teste (Fixture) para a classe Registers
class RegistersTest : public ::testing::Test {
protected:
    Registers reg;

    void SetUp() override {
        // O construtor já chama reset(), mas podemos garantir aqui se necessário.
        reg.reset();
    }
};

// ============================================================================
// Testes de Inicialização e Reset
// ============================================================================

TEST_F(RegistersTest, InitialStateIsCorrect) {
    // Valores após o reset()
    
    // V Registers
    for (int i = 0; i < 16; ++i) {
        ASSERT_EQ(0, reg.getV(i)) << "V[" << i << "] deve ser 0 após reset.";
    }

    // I, SP, Timers
    ASSERT_EQ(0, reg.getI());
    ASSERT_EQ(0x200, reg.getPC()); // PC inicia em 0x200
    ASSERT_EQ(0, reg.SP); // Stack Pointer (assumindo que SP é public ou pode ser lido)
    ASSERT_EQ(0, reg.getDelayTimer());
    ASSERT_EQ(0, reg.getSoundTimer());
}

TEST_F(RegistersTest, ResetClearsAllState) {
    // 1. Altera alguns valores
    reg.setV(0xA, 0xFF);
    reg.setI(0x1234);
    reg.setPC(0xABCD);
    reg.setDelayTimer(50);
    reg.setSoundTimer(10);
    reg.pushStack(0xFEED);
    
    // 2. Chama o reset
    reg.reset();
    
    // 3. Verifica se os valores voltaram ao estado inicial
    ASSERT_EQ(0, reg.getV(0xA));
    ASSERT_EQ(0, reg.getI());
    ASSERT_EQ(0x200, reg.getPC());
    ASSERT_EQ(0, reg.getDelayTimer());
    ASSERT_EQ(0, reg.getSoundTimer());
    // PC precisa ser 0x200
    ASSERT_EQ(0x200, reg.getPC());
    // Stack Pointer deve ser 0 (não há getter/setter para SP, confiando em reset() interno)
    // Se SP fosse acessível: ASSERT_EQ(0, reg.getSP());
}

// ============================================================================
// Testes dos Registradores V
// ============================================================================

TEST_F(RegistersTest, SetAndGetV) {
    uint8_t index = 0x5; // V5
    uint8_t value = 0xBE;
    
    reg.setV(index, value);
    ASSERT_EQ(value, reg.getV(index));
}

TEST_F(RegistersTest, VRegisterIndexMasking) {
    // Teste para índices maiores que 0xF (devem ser mascarados para 0x0)
    reg.setV(0x10, 0x42); // 0x10 & 0xF = 0x0
    ASSERT_EQ(0x42, reg.getV(0x0));
    
    // Teste para índice 0x1F (deve ser mascarado para 0xF)
    reg.setV(0x1F, 0x99); // 0x1F & 0xF = 0xF
    ASSERT_EQ(0x99, reg.getV(0xF));
}

// ============================================================================
// Testes do Registrador I
// ============================================================================

TEST_F(RegistersTest, SetAndGetI) {
    uint16_t value = 0xABCD;
    reg.setI(value);
    ASSERT_EQ(value, reg.getI());
}

TEST_F(RegistersTest, AddI) {
    reg.setI(0x0010);
    reg.addI(0x0005);
    ASSERT_EQ(0x0015, reg.getI());
    
    reg.addI(0xFFE0); // Teste de overflow de 16-bit (I é 16-bit)
    ASSERT_EQ(0xFFFF, reg.getI());
}

// ============================================================================
// Testes do Program Counter (PC)
// ============================================================================

TEST_F(RegistersTest, SetAndGetPC) {
    uint16_t value = 0xFACE;
    reg.setPC(value);
    ASSERT_EQ(value, reg.getPC());
}

TEST_F(RegistersTest, IncrementPC) {
    reg.setPC(0x200);
    reg.incrementPC(); // PC += 2
    ASSERT_EQ(0x202, reg.getPC());
    
    reg.incrementPC();
    ASSERT_EQ(0x204, reg.getPC());
}

TEST_F(RegistersTest, SkipInstruction) {
    reg.setPC(0x500);
    reg.skipInstruction(); // PC += 4
    ASSERT_EQ(0x504, reg.getPC());
    
    reg.setPC(0xFFFC); // Testa o comportamento próximo ao limite
    reg.skipInstruction();
    ASSERT_EQ(0x0000, reg.getPC()); // Espera-se o wrap-around
}

// ============================================================================
// Testes da Stack (Pilha)
// ============================================================================

TEST_F(RegistersTest, PushAndPopStack) {
    uint16_t value1 = 0x1234;
    uint16_t value2 = 0xABCD;
    
    reg.pushStack(value1);
    reg.pushStack(value2);
    
    // Pop deve retornar o último valor inserido (LIFO)
    ASSERT_EQ(value2, reg.popStack());
    ASSERT_EQ(value1, reg.popStack());
}

// Teste de underflow/overflow da stack - requer verificação manual ou
// tratamento na implementação, mas aqui testamos apenas a funcionalidade.
TEST_F(RegistersTest, StackBoundary_Simple) {
    // Push e Pop em série
    reg.pushStack(0x1111);
    reg.pushStack(0x2222);
    ASSERT_EQ(0x2222, reg.popStack());
    reg.pushStack(0x3333);
    ASSERT_EQ(0x3333, reg.popStack());
    ASSERT_EQ(0x1111, reg.popStack());
    
    // Neste ponto, SP deve ser 0 novamente (não acessível)
    // Se tentarmos mais um pop, pode haver um erro/comportamento indefinido.
}

// ============================================================================
// Testes dos Timers
// ============================================================================

TEST_F(RegistersTest, SetAndGetTimers) {
    uint8_t delayValue = 0x50;
    uint8_t soundValue = 0x10;
    
    reg.setDelayTimer(delayValue);
    reg.setSoundTimer(soundValue);
    
    ASSERT_EQ(delayValue, reg.getDelayTimer());
    ASSERT_EQ(soundValue, reg.getSoundTimer());
}

TEST_F(RegistersTest, DecrementDelayTimer) {
    reg.setDelayTimer(3);
    
    reg.decrementDelayTimer();
    ASSERT_EQ(2, reg.getDelayTimer());
    
    reg.decrementDelayTimer();
    ASSERT_EQ(1, reg.getDelayTimer());
    
    reg.decrementDelayTimer();
    ASSERT_EQ(0, reg.getDelayTimer());
    
    // Deve parar em 0
    reg.decrementDelayTimer();
    ASSERT_EQ(0, reg.getDelayTimer());
}

TEST_F(RegistersTest, DecrementSoundTimer) {
    reg.setSoundTimer(1);
    
    reg.decrementSoundTimer();
    ASSERT_EQ(0, reg.getSoundTimer());
    
    // Deve parar em 0
    reg.decrementSoundTimer();
    ASSERT_EQ(0, reg.getSoundTimer());
}

// Função principal para o Google Test (se estiver usando gtest)
/*
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
*/