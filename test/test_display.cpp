// ============================================================================
// test_display.cpp - Display Module Tests
// ============================================================================
#include <gtest/gtest.h>
#include "Display.h"

class DisplayTest : public ::testing::Test {
protected:
    Display display;
    
    void SetUp() override {
        display.clear();
    }
};

TEST_F(DisplayTest, InitialStateIsBlack) {
    const uint8_t* pixels = display.getPixels();
    for(size_t i = 0; i < 64 * 32; ++i) {
        EXPECT_EQ(pixels[i], 0);
    }
}

TEST_F(DisplayTest, ClearScreen) {
    // Draw something first
    uint8_t sprite[1] = {0xFF};
    display.drawSprite(0, 0, sprite, 1);
    
    // Clear
    display.clear();
    
    const uint8_t* pixels = display.getPixels();
    for(size_t i = 0; i < 64 * 32; ++i) {
        EXPECT_EQ(pixels[i], 0);
    }
}

TEST_F(DisplayTest, DrawSinglePixel) {
    uint8_t sprite[1] = {0x80};  // 10000000
    display.drawSprite(0, 0, sprite, 1);
    
    const uint8_t* pixels = display.getPixels();
    EXPECT_EQ(pixels[0], 1);
    EXPECT_EQ(pixels[1], 0);
}

TEST_F(DisplayTest, DrawSprite) {
    uint8_t sprite[3] = {
        0xF0,  // 11110000
        0x90,  // 10010000
        0x90   // 10010000
    };
    
    display.drawSprite(0, 0, sprite, 3);
    
    const uint8_t* pixels = display.getPixels();
    
    // First row
    EXPECT_EQ(pixels[0], 1);
    EXPECT_EQ(pixels[1], 1);
    EXPECT_EQ(pixels[2], 1);
    EXPECT_EQ(pixels[3], 1);
    EXPECT_EQ(pixels[4], 0);
    
    // Second row
    EXPECT_EQ(pixels[64], 1);
    EXPECT_EQ(pixels[65], 0);
    EXPECT_EQ(pixels[66], 0);
    EXPECT_EQ(pixels[67], 1);
}

TEST_F(DisplayTest, XORDrawing) {
    uint8_t sprite[1] = {0x80};
    
    // Draw once
    bool collision1 = display.drawSprite(0, 0, sprite, 1);
    EXPECT_FALSE(collision1);
    
    const uint8_t* pixels = display.getPixels();
    EXPECT_EQ(pixels[0], 1);
    
    // Draw again at same position (XOR should turn it off)
    bool collision2 = display.drawSprite(0, 0, sprite, 1);
    EXPECT_TRUE(collision2);  // Collision detected
    EXPECT_EQ(pixels[0], 0);   // Pixel turned off
}

TEST_F(DisplayTest, SpriteWrapping) {
    uint8_t sprite[1] = {0x80};
    
    // Draw at right edge (should wrap)
    display.drawSprite(63, 0, sprite, 1);
    
    const uint8_t* pixels = display.getPixels();
    EXPECT_EQ(pixels[63], 1);
    
    // Draw at bottom edge
    display.drawSprite(0, 31, sprite, 1);
    EXPECT_EQ(pixels[31 * 64], 1);
}

TEST_F(DisplayTest, RedrawFlag) {
    EXPECT_TRUE(display.getNeedsRedraw());
    
    display.resetRedrawFlag();
    EXPECT_FALSE(display.getNeedsRedraw());
    
    uint8_t sprite[1] = {0x80};
    display.drawSprite(0, 0, sprite, 1);
    EXPECT_TRUE(display.getNeedsRedraw());
}

TEST_F(DisplayTest, Dimensions) {
    EXPECT_EQ(Display::getWidth(), 64);
    EXPECT_EQ(Display::getHeight(), 32);
}