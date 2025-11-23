// ============================================================================
// InstructionSet.cpp - Implementação das instruções
// ============================================================================
#include <iostream>

void InstructionSet::execute(const Opcode& op) {
    switch(op.full & 0xF000) {
        case 0x0000: execute0xxx(op); break;
        case 0x1000: // 1NNN - JP addr
            registers.setPC(op.nnn);
            break;
        case 0x2000: // 2NNN - CALL addr
            registers.pushStack(registers.getPC());
            registers.setPC(op.nnn);
            break;
        case 0x3000: // 3XNN - SE Vx, byte
            if(registers.getV(op.x) == op.nn)
                registers.skipInstruction();
            else
                registers.incrementPC();
            break;
        case 0x4000: // 4XNN - SNE Vx, byte
            if(registers.getV(op.x) != op.nn)
                registers.skipInstruction();
            else
                registers.incrementPC();
            break;
        case 0x5000: // 5XY0 - SE Vx, Vy
            if(registers.getV(op.x) == registers.getV(op.y))
                registers.skipInstruction();
            else
                registers.incrementPC();
            break;
        case 0x6000: // 6XNN - LD Vx, byte
            registers.setV(op.x, op.nn);
            registers.incrementPC();
            break;
        case 0x7000: // 7XNN - ADD Vx, byte
            registers.setV(op.x, registers.getV(op.x) + op.nn);
            registers.incrementPC();
            break;
        case 0x8000: execute8xxx(op); break;
        case 0x9000: // 9XY0 - SNE Vx, Vy
            if(registers.getV(op.x) != registers.getV(op.y))
                registers.skipInstruction();
            else
                registers.incrementPC();
            break;
        case 0xA000: // ANNN - LD I, addr
            registers.setI(op.nnn);
            registers.incrementPC();
            break;
        case 0xB000: // BNNN - JP V0, addr
            registers.setPC(op.nnn + registers.getV(0));
            break;
        case 0xC000: // CXNN - RND Vx, byte
            registers.setV(op.x, randByte(rng) & op.nn);
            registers.incrementPC();
            break;
        case 0xD000: { // DXYN - DRW Vx, Vy, N
            uint8_t x = registers.getV(op.x);
            uint8_t y = registers.getV(op.y);
            uint16_t addr = registers.getI();
            
            uint8_t sprite[15];
            for(int i = 0; i < op.n; ++i) {
                sprite[i] = memory.read(addr + i);
            }
            
            bool collision = display.drawSprite(x, y, sprite, op.n);
            registers.setV(0xF, collision ? 1 : 0);
            registers.incrementPC();
            break;
        }
        case 0xE000: executeExxx(op); break;
        case 0xF000: executeFxxx(op); break;
        default:
            std::cerr << "Opcode desconhecido: 0x" << std::hex << op.full << std::endl;
            registers.incrementPC();
    }
}

void InstructionSet::execute0xxx(const Opcode& op) {
    switch(op.nn) {
        case 0xE0: // 00E0 - CLS
            display.clear();
            registers.incrementPC();
            break;
        case 0xEE: // 00EE - RET
            registers.setPC(registers.popStack());
            registers.incrementPC();
            break;
        default:
            registers.incrementPC();
    }
}

void InstructionSet::execute8xxx(const Opcode& op) {
    uint8_t vx = registers.getV(op.x);
    uint8_t vy = registers.getV(op.y);
    
    switch(op.n) {
        case 0x0: registers.setV(op.x, vy); break;
        case 0x1: registers.setV(op.x, vx | vy); break;
        case 0x2: registers.setV(op.x, vx & vy); break;
        case 0x3: registers.setV(op.x, vx ^ vy); break;
        case 0x4: {
            uint16_t sum = vx + vy;
            registers.setV(0xF, sum > 255 ? 1 : 0);
            registers.setV(op.x, sum & 0xFF);
            break;
        }
        case 0x5:
            registers.setV(0xF, vx > vy ? 1 : 0);
            registers.setV(op.x, vx - vy);
            break;
        case 0x6:
            registers.setV(0xF, vx & 0x1);
            registers.setV(op.x, vx >> 1);
            break;
        case 0x7:
            registers.setV(0xF, vy > vx ? 1 : 0);
            registers.setV(op.x, vy - vx);
            break;
        case 0xE:
            registers.setV(0xF, (vx & 0x80) >> 7);
            registers.setV(op.x, vx << 1);
            break;
    }
    registers.incrementPC();
}

void InstructionSet::executeExxx(const Opcode& op) {
    uint8_t key = registers.getV(op.x);
    
    switch(op.nn) {
        case 0x9E: // EX9E - SKP Vx
            if(input.isKeyPressed(key))
                registers.skipInstruction();
            else
                registers.incrementPC();
            break;
        case 0xA1: // EXA1 - SKNP Vx
            if(!input.isKeyPressed(key))
                registers.skipInstruction();
            else
                registers.incrementPC();
            break;
        default:
            registers.incrementPC();
    }
}

void InstructionSet::executeFxxx(const Opcode& op) {
    switch(op.nn) {
        case 0x07:
            registers.setV(op.x, registers.getDelayTimer());
            registers.incrementPC();
            break;
        case 0x0A: {
            int key = input.getAnyKeyPressed();
            if(key >= 0) {
                registers.setV(op.x, key);
                registers.incrementPC();
            }
            break;
        }
        case 0x15:
            registers.setDelayTimer(registers.getV(op.x));
            registers.incrementPC();
            break;
        case 0x18:
            registers.setSoundTimer(registers.getV(op.x));
            registers.incrementPC();
            break;
        case 0x1E:
            registers.addI(registers.getV(op.x));
            registers.incrementPC();
            break;
        case 0x29:
            registers.setI(registers.getV(op.x) * 5);
            registers.incrementPC();
            break;
        case 0x33: {
            uint8_t val = registers.getV(op.x);
            uint16_t addr = registers.getI();
            memory.write(addr, val / 100);
            memory.write(addr + 1, (val / 10) % 10);
            memory.write(addr + 2, val % 10);
            registers.incrementPC();
            break;
        }
        case 0x55:
            for(int i = 0; i <= op.x; ++i) {
                memory.write(registers.getI() + i, registers.getV(i));
            }
            registers.incrementPC();
            break;
        case 0x65:
            for(int i = 0; i <= op.x; ++i) {
                registers.setV(i, memory.read(registers.getI() + i));
            }
            registers.incrementPC();
            break;
        default:
            registers.incrementPC();
    }
}
