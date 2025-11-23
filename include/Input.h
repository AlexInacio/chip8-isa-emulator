// ============================================================================
// Input.h - Sistema de entrada
// ============================================================================
#ifndef INPUT_H
#define INPUT_H

#include <cstdint>
#include <cstring>

class Input {
private:
    static constexpr size_t KEY_COUNT = 16;
    uint8_t keys[KEY_COUNT];

public:
    Input() {
        clear();
    }
    
    void clear() {
        std::memset(keys, 0, KEY_COUNT);
    }
    
    void setKey(uint8_t key, bool pressed) {
        if(key < KEY_COUNT) {
            keys[key] = pressed ? 1 : 0;
        }
    }
    
    bool isKeyPressed(uint8_t key) const {
        return key < KEY_COUNT && keys[key] != 0;
    }
    
    int getAnyKeyPressed() const {
        for(int i = 0; i < KEY_COUNT; ++i) {
            if(keys[i]) return i;
        }
        return -1;
    }
};

#endif // INPUT_H