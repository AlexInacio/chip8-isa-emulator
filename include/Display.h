// ============================================================================
// Display.h - Sistema de vídeo
// ============================================================================
#ifndef DISPLAY_H
#define DISPLAY_H

#include <cstdint>
#include <cstring>

class Display {
private:
    static constexpr size_t WIDTH = 64;
    static constexpr size_t HEIGHT = 32;
    static constexpr size_t PIXEL_COUNT = WIDTH * HEIGHT;
    
    uint8_t pixels[PIXEL_COUNT];
    bool needsRedraw;

public:
    Display() {
        clear();
    }
    
    void clear() {
        std::memset(pixels, 0, PIXEL_COUNT);
        needsRedraw = true;
    }
    
    bool drawSprite(uint8_t x, uint8_t y, const uint8_t* sprite, uint8_t height) {
        bool collision = false;
        x %= WIDTH;
        y %= HEIGHT;
        
        for(uint8_t row = 0; row < height; ++row) {
            uint8_t spriteData = sprite[row];
            
            for(uint8_t col = 0; col < 8; ++col) {
                if(spriteData & (0x80 >> col)) {
                    uint16_t pixelX = (x + col) % WIDTH;
                    uint16_t pixelY = (y + row) % HEIGHT;
                    uint16_t index = pixelY * WIDTH + pixelX;
                    
                    if(pixels[index] == 1) {
                        collision = true;
                    }
                    pixels[index] ^= 1;
                }
            }
        }
        
        needsRedraw = true;
        return collision;
    }
    
    const uint8_t* getPixels() const { return pixels; }
    bool getNeedsRedraw() const { return needsRedraw; }
    void resetRedrawFlag() { needsRedraw = false; }
    
    static constexpr size_t getWidth() { return WIDTH; }
    static constexpr size_t getHeight() { return HEIGHT; }
};

#endif // DISPLAY_H