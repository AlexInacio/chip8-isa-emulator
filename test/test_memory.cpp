// ============================================================================
// test_memory.cpp - Memory Module Tests
// ============================================================================
#include <gtest/gtest.h>
#include "Memory.h"

class MemoryTest : public ::testing::Test {
protected:
    Memory memory;
    
    void SetUp() override {
        memory.clear();
    }
};

TEST_F(MemoryTest, InitialStateIsZero) {
    for(int i = 0x200; i < 4096; ++i) {
        EXPECT_EQ(memory.read(i), 0);
    }
}

TEST_F(MemoryTest, FontsetIsLoaded) {
    // Test if fontset is loaded at 0x000-0x04F
    // Font for '0' starts at 0x000
    EXPECT_EQ(memory.read(0x000), 0xF0);
    EXPECT_EQ(memory.read(0x001), 0x90);
    EXPECT_EQ(memory.read(0x002), 0x90);
    EXPECT_EQ(memory.read(0x003), 0x90);
    EXPECT_EQ(memory.read(0x004), 0xF0);
    
    // Font for 'F' starts at 0x04B
    EXPECT_EQ(memory.read(0x04B), 0xF0);
    EXPECT_EQ(memory.read(0x04C), 0x80);
    EXPECT_EQ(memory.read(0x04D), 0xF0);
    EXPECT_EQ(memory.read(0x04E), 0x80);
    EXPECT_EQ(memory.read(0x04F), 0x80);
}

TEST_F(MemoryTest, ReadWriteOperations) {
    memory.write(0x200, 0x12);
    EXPECT_EQ(memory.read(0x200), 0x12);
    
    memory.write(0x300, 0xAB);
    EXPECT_EQ(memory.read(0x300), 0xAB);
    
    memory.write(0xFFF, 0xFF);
    EXPECT_EQ(memory.read(0xFFF), 0xFF);
}

TEST_F(MemoryTest, AddressWrapping) {
    // Test that addresses wrap correctly
    memory.write(0x1000, 0x42);  // Should wrap to 0x000
    EXPECT_EQ(memory.read(0x000), 0x42);
}

TEST_F(MemoryTest, LoadProgram) {
    uint8_t program[] = {0x12, 0x34, 0x56, 0x78, 0x9A};
    
    EXPECT_TRUE(memory.loadProgram(program, sizeof(program)));
    
    EXPECT_EQ(memory.read(0x200), 0x12);
    EXPECT_EQ(memory.read(0x201), 0x34);
    EXPECT_EQ(memory.read(0x202), 0x56);
    EXPECT_EQ(memory.read(0x203), 0x78);
    EXPECT_EQ(memory.read(0x204), 0x9A);
}

TEST_F(MemoryTest, LoadProgramTooLarge) {
    uint8_t largeProgram[4096];
    EXPECT_FALSE(memory.loadProgram(largeProgram, sizeof(largeProgram)));
}

TEST_F(MemoryTest, ProgramStartAddress) {
    EXPECT_EQ(Memory::getProgramStart(), 0x200);
}