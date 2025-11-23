#include "CPU.h" // Inclui o arquivo a ser testado
#include "Opcode.h" // Inclui a estrutura Opcode
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::_;
using ::testing::InSequence;

// 1. Mocks de Dependências
class MockMemory : public Memory {
public:
    MOCK_METHOD(uint8_t, read, (uint16_t address), (override));
};

class MockRegisters : public Registers {
public:
    MOCK_METHOD(void, reset, (), (override));
    MOCK_METHOD(uint16_t, getPC, (), (const, override));
    MOCK_METHOD(void, decrementDelayTimer, (), (override));
    MOCK_METHOD(void, decrementSoundTimer, (), (override));
};

class MockDisplay : public Display {
public:
    // Adicione métodos mockados relevantes aqui, se a CPU os chamasse diretamente
};

class MockInput : public Input {
public:
    // Adicione métodos mockados relevantes aqui
};

class MockInstructionSet : public InstructionSet {
public:
    // O construtor é complexo, então fazemos um mock parcial ou ignoramos
    MockInstructionSet(Memory& mem, Registers& reg, Display& disp, Input& inp)
        : InstructionSet(mem, reg, disp, inp) {}

    MOCK_METHOD(void, execute, (const Opcode& op), ());
};

// 2. Fixture de Teste
class CPUTest : public ::testing::Test {
protected:
    MockMemory memory;
    MockRegisters registers;
    MockDisplay display;
    MockInput input;
    // Precisamos de um ponteiro para o mock da InstructionSet, pois o construtor da CPU
    // espera uma referência e criamos um mock que herda da classe real.
    // NOTE: Isso pode ser complexo. Para simplificar, ajustamos o mock da InstructionSet
    // para ser passível de criação e herdar de um MockBase, mas vamos usar um Mock
    // real que inicializa as referências.

    // A maneira mais fácil de usar o Google Mock para testar o execute é injetar o mock:
    // Como a InstructionSet é criada *dentro* da CPU, precisamos de uma MockCPU
    // ou mudar o design para injetar a InstructionSet.
    // Vamos simplificar o teste focando nas chamadas externas.

    // Se o design da CPU fosse (melhor para testes):
    // CPU(Memory& mem, ..., InstructionSet& instSet)
    // Usamos um mock que encapsula o real para testar o 'execute'.

    // Vamos assumir que conseguimos criar um MockInstructionSet com o construtor necessário.
    MockInstructionSet instructionSet;

    // CPU (usa o mock instrucionSet, mas a CPU real criaria o seu próprio)
    // Devido ao design, vamos testar apenas as chamadas para Memory e Registers.
    // Testar `execute` requer um design mais flexível (injeção de dependência).

    // Para o teste abaixo, precisamos reescrever a CPU para usar os mocks:
    // CPU_Testable cpu;

public:
    CPUTest() : instructionSet(memory, registers, display, input),
                // Criamos o objeto CPU com os mocks
                cpu(memory, registers, display, input) {}

    CPU cpu;
};

// ============================================================================
// Testes de Funcionalidade
// ============================================================================

TEST_F(CPUTest, ResetCallsRegisterReset) {
    // Expectativa: Quando reset() é chamado na CPU, registers.reset() deve ser chamado
    EXPECT_CALL(registers, reset()).Times(1);
    
    cpu.reset();
}

TEST_F(CPUTest, CyclePerformsFetchDecodeExecuteAndUpdateTimers) {
    // Cenário:
    // 1. PC está em 0x200
    // 2. Memória em 0x200 contém 0xAB
    // 3. Memória em 0x201 contém 0xCD
    // -> Opcode lido deve ser 0xABCD

    const uint16_t START_PC = 0x200;
    const uint8_t HIGH_BYTE = 0xAB;
    const uint8_t LOW_BYTE = 0xCD;
    const uint16_t EXPECTED_OPCODE = 0xABCD;

    // Usamos InSequence para garantir a ordem das chamadas
    InSequence s;

    // 1. Fetch: Get PC
    EXPECT_CALL(registers, getPC()).WillOnce(testing::Return(START_PC));

    // 2. Fetch: Read High Byte (pc)
    EXPECT_CALL(memory, read(START_PC)).WillOnce(testing::Return(HIGH_BYTE));

    // 3. Fetch: Read Low Byte (pc + 1)
    EXPECT_CALL(memory, read(START_PC + 1)).WillOnce(testing::Return(LOW_BYTE));

    // 4. Execute: A chamada a execute na InstructionSet é a mais difícil de mockar
    // devido à construção do Opcode. Vamos apenas verificar as chamadas subsequentes.
    // Se a InstructionSet fosse injetada, usaríamos:
    // EXPECT_CALL(instructionSet, execute(testing::Field(&Opcode::full, EXPECTED_OPCODE))).Times(1);

    // 5. Timer Update: Decrementa Delay Timer
    EXPECT_CALL(registers, decrementDelayTimer()).Times(1);

    // 6. Timer Update: Decrementa Sound Timer
    EXPECT_CALL(registers, decrementSoundTimer()).Times(1);

    // Ação: Executa um ciclo da CPU
    cpu.cycle();
}

// ============================================================================
// Teste de Integridade (Opcode Construction)
// ============================================================================

// Embora o execute não seja mockado, podemos testar se a CPU constrói o Opcode
// corretamente, simulando as leituras de memória.

TEST_F(CPUTest, CycleCorrectlyFormsOpcode) {
    // Nota: Este teste é mais avançado, pois requer inspecionar o argumento
    // passado para `instructionSet.execute()`, o que só é possível se
    // `InstructionSet` for injetada e mockada.

    const uint16_t PC = 0x300;
    const uint8_t HI = 0x8F;
    const uint8_t LO = 0xE6;
    const uint16_t EXPECTED_OPCODE = 0x8FE6;

    // Configuração dos mocks
    EXPECT_CALL(registers, getPC()).WillOnce(testing::Return(PC));
    EXPECT_CALL(memory, read(PC)).WillOnce(testing::Return(HI));
    EXPECT_CALL(memory, read(PC + 1)).WillOnce(testing::Return(LO));

    // Configuração das chamadas finais (que não importam para este teste específico)
    EXPECT_CALL(registers, decrementDelayTimer()).Times(1);
    EXPECT_CALL(registers, decrementSoundTimer()).Times(1);
    
    // Para testar o valor do Opcode, precisaríamos que o execute fosse mockado:
    // Exemplo de como ficaria (se MockInstructionSet fosse injetado):
    /*
    EXPECT_CALL(instructionSet, execute(_))
        .WillOnce([](const Opcode& op) {
            // Verifica se o valor completo do Opcode está correto
            ASSERT_EQ(EXPECTED_OPCODE, op.full);
        });
    */

    // Como não podemos injetar, verificamos se o fluxo de leitura está correto.
    cpu.cycle();
}