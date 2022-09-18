#include <iostream>
#include <chrono>
#include "Platform.h"
#include "CPU.h"

// TODO fix speed
// TODO implement sound

int main(int argc, char *argv[]) {
    // if (argc != 2) {
    //     std::cerr << "Path to ROM file must be provided. Please try again." << std::endl;
    //     std::exit(EXIT_FAILURE);
    // }

    Platform platform("Chip8", DISPLAY_X, DISPLAY_Y);

    // TODO load roms from ux0:data
    CPU cpu;
    cpu.LoadData("roms/Pong.ch8");

    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool run = true;
    while (run) {
        uint8_t* keys = cpu.GetKeys();
        run = platform.ProcessInput(keys);
        auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
        
        // TODO 1 is usual cycle delay here... Sometimes use 2 for more playable speed
        if (dt > 2) {
            lastCycleTime = currentTime;
            cpu.EmulateCycle();
            cpu.UpdateTimers();
            
            if (cpu.draw_flag) {
                uint8_t* graphics = cpu.GetGraphics();
                uint32_t pixels[DISPLAY_X * DISPLAY_Y] = {0};
                for (int i = 0; i < 2048; i++) {
                    pixels[i] = graphics[i] == 0 ? 0x000000FF : 0xFFFFFFFF;
                }
                
                platform.UpdateDisplay(pixels, sizeof(Uint32) * DISPLAY_X);
                cpu.draw_flag = false;
            }
        }
        
    }

    return EXIT_SUCCESS;
}