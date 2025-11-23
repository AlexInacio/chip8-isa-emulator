// ============================================================================
// test_input.cpp - Input Module Tests
// ============================================================================
#include <gtest/gtest.h>
#include "Input.h"

class InputTest : public ::testing::Test {
protected:
    Input input;
    
    void SetUp() override {
        input.clear();
    }
};

TEST_F(InputTest, InitialState) {
    for(int i = 0; i < 16; ++i) {
        EXPECT_FALSE(input.isKeyPressed(i));
    }
}

TEST_F(InputTest, KeyPressAndRelease) {
    input.setKey(5, true);
    EXPECT_TRUE(input.isKeyPressed(5));
    
    input.setKey(5, false);
    EXPECT_FALSE(input.isKeyPressed(5));
}

TEST_F(InputTest, MultipleKeys) {
    input.setKey(0, true);
    input.setKey(5, true);
    input.setKey(10, true);
    
    EXPECT_TRUE(input.isKeyPressed(0));
    EXPECT_TRUE(input.isKeyPressed(5));
    EXPECT_TRUE(input.isKeyPressed(10));
    EXPECT_FALSE(input.isKeyPressed(1));
}

TEST_F(InputTest, GetAnyKeyPressed) {
    EXPECT_EQ(input.getAnyKeyPressed(), -1);
    
    input.setKey(7, true);
    EXPECT_EQ(input.getAnyKeyPressed(), 7);
    
    input.setKey(7, false);
    EXPECT_EQ(input.getAnyKeyPressed(), -1);
}

TEST_F(InputTest, ClearAllKeys) {
    input.setKey(0, true);
    input.setKey(5, true);
    input.setKey(10, true);
    
    input.clear();
    
    for(int i = 0; i < 16; ++i) {
        EXPECT_FALSE(input.isKeyPressed(i));
    }
}

TEST_F(InputTest, InvalidKeyIndex) {
    // Should handle gracefully
    input.setKey(16, true);
    input.setKey(100, true);
    // No crash = pass
}