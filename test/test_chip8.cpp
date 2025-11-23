// ============================================================================
// test_chip8.cpp - Integration Tests
// ============================================================================
#include <gtest/gtest.h>
#include "Chip8.h"

class Chip8Test : public ::testing::Test {
protected:
    Chip8 emulator;
    
    void SetUp() override {
        emulator.initialize();
    }
};

TEST_F(Chip8Test, Initialization) {
    // Just ensure it doesn't crash
    EXPECT_NO_THROW(emulator.initialize());
}

TEST_F(Chip8Test, LoadROMInvalidFile) {
    EXPECT_FALSE(emulator.loadROM("nonexistent.ch8"));
}

TEST_F(Chip8Test, ExecuteSimpleProgram) {
    // Create a simple program in memory manually
    // This would normally come from loadROM
    // For now, just test that cycle doesn't crash
    EXPECT_NO_THROW(emulator.cycle());
}

TEST_F(Chip8Test, GetComponents) {
    const Display& display = emulator.getDisplay();
    Input& input = emulator.getInput();
    
    EXPECT_NO_THROW(display.getPixels());
    EXPECT_NO_THROW(input.clear());
}

TEST_F(Chip8Test, MultipleInitializations) {
    emulator.initialize();
    emulator.initialize();
    // Should not crash
    EXPECT_NO_THROW(emulator.cycle());
}

// ============================================================================
// Main test runner
// ============================================================================
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}