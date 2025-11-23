#include "InstructionSet.h"
#include "Opcode.h"
// Inclua os headers reais ou defina Mocks para Memory, Registers, Display, Input
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::SetArgReferee;

// Assumimos que as classes base têm métodos virtuais ou usamos uma abordagem
// de herança de interface (embora as classes fornecidas pareçam concretas,
// o Google Mock permite mocks flexíveis).

class MockMemory {
public:
    MOCK_METHOD(uint8_t, read, (uint16_t address), ());
    MOCK_METHOD(void, write, (uint16_t address, uint8_t value), ());
};

class MockRegisters {
public:
    MOCK_METHOD(uint8_t, getV, (uint8_t index), (const));
    MOCK_METHOD(void, setV, (uint8_t index, uint8_t value), ());
    MOCK_METHOD(uint16_t, getI, (), (const));
    MOCK_METHOD(void, setI, (uint16_t value), ());
    MOCK_METHOD(void, addI, (uint16_t value), ());
    MOCK_METHOD(uint16_t, getPC, (), (const));
    MOCK_METHOD(void, setPC, (uint16_t value), ());
    MOCK_METHOD(void, incrementPC, (), ());
    MOCK_METHOD(void, skipInstruction, (), ());
    MOCK_METHOD(void, pushStack, (uint16_t value), ());
    MOCK_METHOD(uint16_t, popStack, (), ());
    MOCK_METHOD(void, setDelayTimer, (uint8_t value), ());
    MOCK_METHOD(uint8_t, getDelayTimer, (), (const));
};

class MockDisplay {
public:
    MOCK_METHOD(bool, drawSprite, (uint8_t x, uint8_t y, uint8_t height, const uint8_t* spriteData), ());
};

class MockInput {
public:
    MOCK_METHOD(uint8_t, getKey, (), (const));
    MOCK_METHOD(bool, isKeyPressed, (uint8_t key), (const));
    MOCK_METHOD(uint8_t, waitForKeyPress, (), ());
};

// 2. Fixture de Teste
class InstructionSetTest : public ::testing::Test {
protected:
    // Criamos os mocks para injeção de dependência
    NiceMock<MockMemory> memory;
    NiceMock<MockRegisters> registers;
    NiceMock<MockDisplay> display;
    NiceMock<MockInput> input;
    
    // Instância da classe a ser testada, injetando os mocks
    InstructionSet instructionSet;

public:
    InstructionSetTest() 
        : instructionSet(
            reinterpret_cast<Memory&>(memory), 
            reinterpret_cast<Registers&>(registers), 
            reinterpret_cast<Display&>(display), 
            reinterpret_cast<Input&>(input)
        ) 
    {}
};

// ============================================================================
// Testes da Categoria 1: 0xxx (CLS, RET)
// ============================================================================

TEST_F(InstructionSetTest, Execute00E0_CLS) {
    // Opcode: 0x00E0 (Limpa a tela)
    Opcode op(0x00E0);
    
    // Expectativa: Apenas o incremento do PC deve ocorrer (e a chamada CLS, se existisse)
    EXPECT_CALL(registers, incrementPC()).Times(1);

    // Nota: Se a classe Display tivesse um método `clear()`, ele seria chamado aqui.
    // Como não foi fornecido, o teste foca no fluxo de controle.

    instructionSet.execute(op);
}

TEST_F(InstructionSetTest, Execute00EE_RET) {
    // Opcode: 0x00EE (Retorno de subrotina)
    Opcode op(0x00EE);
    const uint16_t RETURN_ADDRESS = 0x500;
    
    // 1. Pop do endereço da pilha
    EXPECT_CALL(registers, popStack()).WillOnce(Return(RETURN_ADDRESS));
    
    // 2. Seta o PC para o endereço retornado
    EXPECT_CALL(registers, setPC(RETURN_ADDRESS)).Times(1);

    instructionSet.execute(op);
}

// ============================================================================
// Testes da Categoria 2: 1NNN, 2NNN, 3XNN, 4XNN, 5XY0, 6XNN, 7XNN
// ============================================================================

TEST_F(InstructionSetTest, Execute1NNN_JMP) {
    // Opcode: 0x1ABC (JMP 0xABC)
    Opcode op(0x1ABC);
    const uint16_t TARGET_ADDRESS = 0xABC;
    
    // Expectativa: Seta o PC para o valor NNN
    EXPECT_CALL(registers, setPC(TARGET_ADDRESS)).Times(1);
    
    instructionSet.execute(op);
}

TEST_F(InstructionSetTest, Execute2NNN_CALL) {
    // Opcode: 0x2DEF (CALL 0xDEF)
    Opcode op(0x2DEF);
    const uint16_t TARGET_ADDRESS = 0xDEF;
    const uint16_t NEXT_INSTRUCTION_PC = 0x202; // Simulação do PC atual

    // 1. Obtém o PC atual
    EXPECT_CALL(registers, getPC()).WillOnce(Return(NEXT_INSTRUCTION_PC));
    
    // 2. Push do PC atual na pilha
    EXPECT_CALL(registers, pushStack(NEXT_INSTRUCTION_PC)).Times(1);
    
    // 3. Seta o PC para o endereço NNN
    EXPECT_CALL(registers, setPC(TARGET_ADDRESS)).Times(1);
    
    instructionSet.execute(op);
}

TEST_F(InstructionSetTest, Execute3XNN_SE_SkipIfEqual) {
    // Opcode: 0x3542 (SE V5, 0x42)
    Opcode op(0x3542);
    
    // Caso 1: V5 == 0x42 (Salto)
    EXPECT_CALL(registers, getV(0x5)).WillOnce(Return(0x42));
    EXPECT_CALL(registers, skipInstruction()).Times(1);
    instructionSet.execute(op);

    // Caso 2: V5 != 0x42 (Não Salto)
    EXPECT_CALL(registers, getV(0x5)).WillOnce(Return(0x10));
    EXPECT_CALL(registers, incrementPC()).Times(1);
    instructionSet.execute(op);
}

// ============================================================================
// Testes da Categoria 3: 8xxx (Operações Aritméticas/Lógicas)
// ============================================================================

TEST_F(InstructionSetTest, Execute8XY0_LD) {
    // Opcode: 0x8120 (LD V1, V2)
    Opcode op(0x8120);
    const uint8_t VALUE = 0xAB;

    // 1. Obtém V2
    EXPECT_CALL(registers, getV(0x2)).WillOnce(Return(VALUE));

    // 2. Seta V1 = V2
    EXPECT_CALL(registers, setV(0x1, VALUE)).Times(1);
    EXPECT_CALL(registers, incrementPC()).Times(1);

    instructionSet.execute(op);
}

TEST_F(InstructionSetTest, Execute8XY4_ADD_WithCarry) {
    // Opcode: 0x8A34 (ADD VA, V3)
    Opcode op(0x8A34);
    const uint8_t VA = 0xFF;
    const uint8_t V3 = 0x01;
    
    // 1. Obtém VA e V3
    EXPECT_CALL(registers, getV(0xA)).WillOnce(Return(VA));
    EXPECT_CALL(registers, getV(0x3)).WillOnce(Return(V3));
    
    // 2. Seta VA = VA + V3 (0xFF + 0x01 = 0x00, Carry)
    EXPECT_CALL(registers, setV(0xA, 0x00)).Times(1);
    
    // 3. Seta VF = 1 (Carry)
    EXPECT_CALL(registers, setV(0xF, 0x01)).Times(1);
    EXPECT_CALL(registers, incrementPC()).Times(1);

    instructionSet.execute(op);
}

TEST_F(InstructionSetTest, Execute8XY5_SUB_WithBorrow) {
    // Opcode: 0x8235 (SUB V2, V3) (V2 - V3)
    Opcode op(0x8235);
    const uint8_t V2 = 0x10;
    const uint8_t V3 = 0x20;
    
    // 1. Obtém V2 e V3
    EXPECT_CALL(registers, getV(0x2)).WillOnce(Return(V2));
    EXPECT_CALL(registers, getV(0x3)).WillOnce(Return(V3));
    
    // 2. Seta V2 = V2 - V3 (0x10 - 0x20 = 0xF0, No Borrow)
    EXPECT_CALL(registers, setV(0x2, 0xF0)).Times(1); // 0xF0 é o resultado de 0x10 - 0x20 com 8 bits
    
    // 3. Seta VF = 0 (No Borrow)
    EXPECT_CALL(registers, setV(0xF, 0x00)).Times(1);
    EXPECT_CALL(registers, incrementPC()).Times(1);

    instructionSet.execute(op);
}

// ============================================================================
// Testes da Categoria 4: DXYN (DRW - Desenhar Sprite)
// ============================================================================

TEST_F(InstructionSetTest, ExecuteDXYN_DRW_Basic) {
    // Opcode: 0xD345 (DRW V3, V4, 5)
    Opcode op(0xD345);
    const uint8_t VX_X = 50;
    const uint8_t VY_Y = 30;
    const uint8_t HEIGHT = 5;
    const uint16_t I_ADDRESS = 0x400;
    const uint16_t PC = 0x200;
    const bool COLLISION = true;

    // 1. Obtém coordenadas (VX, VY) e endereço I
    EXPECT_CALL(registers, getV(0x3)).WillOnce(Return(VX_X));
    EXPECT_CALL(registers, getV(0x4)).WillOnce(Return(VY_Y));
    EXPECT_CALL(registers, getI()).WillOnce(Return(I_ADDRESS));
    
    // 2. Chama drawSprite (Mock Display)
    // O mock precisa saber onde o spriteData está.
    // Neste teste, apenas verificamos se a função é chamada corretamente.
    // O último argumento `const uint8_t* spriteData` é difícil de mockar
    // sem expor a memória, então usamos `_` e focamos nos outros args.
    EXPECT_CALL(display, drawSprite(VX_X, VY_Y, HEIGHT, _)).WillOnce(Return(COLLISION));

    // 3. Seta VF com o status de colisão
    EXPECT_CALL(registers, setV(0xF, 0x01)).Times(1);
    EXPECT_CALL(registers, incrementPC()).Times(1);

    instructionSet.execute(op);
}

// ============================================================================
// Testes da Categoria 5: Fxxx (Timers e Memória)
// ============================================================================

TEST_F(InstructionSetTest, ExecuteFX07_LD_Vx_DT) {
    // Opcode: 0xF207 (LD V2, DT)
    Opcode op(0xF207);
    const uint8_t DT_VALUE = 45;
    
    // 1. Obtém o valor do Delay Timer
    EXPECT_CALL(registers, getDelayTimer()).WillOnce(Return(DT_VALUE));
    
    // 2. Seta V2 = DT
    EXPECT_CALL(registers, setV(0x2, DT_VALUE)).Times(1);
    EXPECT_CALL(registers, incrementPC()).Times(1);

    instructionSet.execute(op);
}

TEST_F(InstructionSetTest, ExecuteFX15_LD_DT_Vx) {
    // Opcode: 0xF315 (LD DT, V3)
    Opcode op(0xF315);
    const uint8_t V3_VALUE = 120;
    
    // 1. Obtém o valor de V3
    EXPECT_CALL(registers, getV(0x3)).WillOnce(Return(V3_VALUE));
    
    // 2. Seta DT = V3
    EXPECT_CALL(registers, setDelayTimer(V3_VALUE)).Times(1);
    EXPECT_CALL(registers, incrementPC()).Times(1);

    instructionSet.execute(op);
}

TEST_F(InstructionSetTest, ExecuteFX1E_ADD_I_Vx) {
    // Opcode: 0xFA1E (ADD I, VA)
    Opcode op(0xFA1E);
    const uint8_t VA_VALUE = 0x50;
    
    // 1. Obtém o valor de VA
    EXPECT_CALL(registers, getV(0xA)).WillOnce(Return(VA_VALUE));
    
    // 2. Adiciona VA a I
    EXPECT_CALL(registers, addI(VA_VALUE)).Times(1);
    EXPECT_CALL(registers, incrementPC()).Times(1);

    instructionSet.execute(op);
}

TEST_F(InstructionSetTest, ExecuteFX65_LD_Vx_I) {
    // Opcode: 0xF465 (LD V0-V4, [I])
    Opcode op(0xF465); // X=4 -> V0, V1, V2, V3, V4
    const uint16_t START_ADDRESS = 0x300;
    
    // 1. Obtém endereço I
    EXPECT_CALL(registers, getI()).WillOnce(Return(START_ADDRESS));
    
    // 2. Expectativas de Leitura da Memória e Escrita nos Registradores
    // V0
    EXPECT_CALL(memory, read(START_ADDRESS + 0)).WillOnce(Return(0x11));
    EXPECT_CALL(registers, setV(0x0, 0x11)).Times(1);
    // V1
    EXPECT_CALL(memory, read(START_ADDRESS + 1)).WillOnce(Return(0x22));
    EXPECT_CALL(registers, setV(0x1, 0x22)).Times(1);
    // V2
    EXPECT_CALL(memory, read(START_ADDRESS + 2)).WillOnce(Return(0x33));
    EXPECT_CALL(registers, setV(0x2, 0x33)).Times(1);
    // V3
    EXPECT_CALL(memory, read(START_ADDRESS + 3)).WillOnce(Return(0x44));
    EXPECT_CALL(registers, setV(0x3, 0x44)).Times(1);
    // V4 (Index X)
    EXPECT_CALL(memory, read(START_ADDRESS + 4)).WillOnce(Return(0x55));
    EXPECT_CALL(registers, setV(0x4, 0x55)).Times(1);
    
    EXPECT_CALL(registers, incrementPC()).Times(1);

    instructionSet.execute(op);
}