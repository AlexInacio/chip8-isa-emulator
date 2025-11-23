// ============================================================================
// main.cpp - Ponto de entrada
// ============================================================================
int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cout << "Uso: " << argv[0] << " <ROM_file>" << std::endl;
        return 1;
    }
    
    Chip8 emulator;
    emulator.initialize();
    
    if(!emulator.loadROM(argv[1])) {
        return 1;
    }
    
    std::cout << "Emulador CHIP-8 Modular iniciado!" << std::endl;
    std::cout << "Arquitetura: Componentes separados e reutilizáveis" << std::endl;
    
    // Loop de exemplo
    for(int i = 0; i < 10; ++i) {
        emulator.cycle();
    }
    
    return 0;
}