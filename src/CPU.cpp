#include "CPU.h"
#include <fstream>
#include <iostream>

CPU::CPU() {
    // Set program counter initial value.
    this->pc = this->START_ADDRESS;

    this->I = 0;
    this->sp = 0;

    // Load fontset starting at address 0x50.
    for (int i = 0; i < FONTSET_SIZE; i++) {
        memory[i + FONTSET_START_ADDRESS] = fontset[i];
    }
}

void CPU::LoadData(const std::string input_file_path) {
    // Read file
    std::ifstream input;
    input.open(input_file_path, std::ios::binary);

    // TODO check file extension is .ch8

    if (input.is_open()) {
        std::cout << "Loading file " + input_file_path + " into memory..." << std::endl;

        // Read binary file
        input.seekg(0, input.end);
        int length = input.tellg();
        input.seekg(0, input.beg);

        char* buffer = new char[length];

        input.read(buffer, length);
        input.close();

        // TODO add check that data will fit into memory when starting at 0x200
        // Load data into memory, starting at 0x200
        for (int i = 0; i < length; i++) {
            memory[this->START_ADDRESS + i] = buffer[i];
        }

        delete[] buffer;
        std:: cout << "File loaded." << std::endl;
    } else {
        // TODO could not open file...
        std::cerr << "Failed to load file " + input_file_path << std::endl;
        std::exit(EXIT_FAILURE);
    } 
}

void CPU::UpdateTimers() {
    if (delay_timer > 0) delay_timer--;

    if (sound_timer > 0) {
        std::cout << "[BEEP]" << std::endl;
        sound_timer--;
    }
}

uint8_t* CPU::GetKeys() {
    return this->key;
}

uint8_t* CPU::GetGraphics() {
    return this->graphics;
}

void CPU::EmulateCycle() {
    unsigned short opcode = this->FetchOpcode();
    this->DecodeAndExecute(opcode);
}

unsigned short CPU::FetchOpcode() {
    // Read 2 bytes from memory into one opcode.
    unsigned short opcode = memory[pc] << 8 | memory[pc + 1];

    // Increment PC by 2.
    pc += 2;

    return opcode;
}

void CPU::DecodeAndExecute(unsigned short opcode) {
    int x = (opcode & 0x0F00) >> 8;
    int y = (opcode & 0x00F0) >> 4;

    switch (opcode & 0xF000) {
        case 0x0000: {
            switch (opcode) {
                // Clears the screen.
                case 0x00E0: 
                    for (int i = 0; i < DISPLAY_X * DISPLAY_Y; i++) {
                        graphics[i] = 0;
                    }
                    draw_flag = true;
                break;

                // Return from a subroutine.
                case 0x00EE: 
                    sp--;
                    pc = stack[sp];
                break;
            }
        }
        break;

        // 1NNN: Jumps to address NNN.
        case 0x1000: 
            pc = opcode & 0x0FFF;
        break;

        // 2NNN: Calls subroutine at NNN.
        case 0x2000: 
            stack[sp] = pc;
            sp++;
            pc = opcode & 0x0FFF;
        break;

        // 3XNN: Skips the next instruction if VX == NN.
        case 0x3000: { 
            int value = opcode & 0x00FF;
            if (V[x] == value) pc += 2;
        }
        break;

        // 4XNN: Skips the next instruction if VX != NN.
        case 0x4000: { 
            int value = opcode & 0x00FF;
            if (V[x] != value) pc += 2;
        }
        break;

        // 5XY0: Skips the next instruction if VX == VY.
        case 0x5000: { 
            if (V[x] == V[y]) pc += 2;
        }
        break;

         // 6XNN: Sets VX to NN.
        case 0x6000: {
            int value = opcode & 0x00FF;
            V[x] = value;   
        }     
        break;

        // 7XNN: Adds NN to VX.
        case 0x7000: { 
            int value = opcode & 0x00FF;
            V[x] += value;
        }
        break;

        case 0x8000: {
            switch (opcode & 0x000F) {
                // VX = VY
                case 0x0000: 
                    V[x] = V[y];
                break;

                // VX = VX | VY
                case 0x0001: 
                    V[x] |= V[y]; 
                break;

                // VX = VX & VY
                case 0x0002: 
                    V[x] &= V[y];
                break;

                // VX = VX ^ VY
                case 0x0003: 
                    V[x] ^= V[y]; 
                break;

                // VX += VY (VF set to 1 if there's a carry, 0 if not)
                case 0x0004:  
                    V[0xF] = (V[x] + V[y]) > 0xFF;
                    V[x] += V[y];
                break;

                // VX -= VY (VF set to 0 if there's a borrow, 1 if not)
                case 0x0005: 
                    V[0xF] = V[x] > V[y];
                    V[x] -= V[y];
                break;

                // Store LSB of VX in VF, then shift VX right by 1.
                case 0x0006: 
                    V[0xF] = V[x] & 0x1;
                    V[x] >>= 1;
                break;

                // VX = VY - VX (VF set to 0 if there's a borrow, 1 if not)
                case 0x0007: 
                    V[0xF] = V[x] < V[y];
                    V[x] = V[y] - V[x]; 
                break;

                // Stores MSB of VX in VF, then shift VX left by 1.
                case 0x000E: 
                    V[0xF] = V[x] >> 7;
                    V[x] <<= 1;
                break;
            }
        }
        break;

        // 9XY0: Skip next instruction if VX != VY
        case 0x9000:
            if (V[x] != V[y]) pc += 2;
        break;

        // ANNN: I = NNN
        case 0xA000: 
            this->I = opcode & 0x0FFF;
        break;

        // BNNN: Jump to address NNN + V0
        case 0xB000: 
            pc = (opcode & 0x0FFF) + V[0];
        break;

        // CXNN: VX = rand() & NN
        case 0xC000: 
            V[x] = (rand() % 256) & (opcode & 0x00FF);
        break;

        // DXYN: Draws a sprite at coordinate (VX, VY) that has a width 
        // of 8 pixels and a height of N+1 pixels.
        case 0xD000: {
            int rows = opcode & 0x000F;
            int cols = 8; // Each 'column' is 1 bit
            V[0xF] = 0;

            for (int i = 0; i < rows; i++) {
                uint8_t sprite = memory[this->I + i];

                // Iterate over each bit (pixel) in sprite
                for (int j = 0; j < cols; j++) {
                    uint8_t pixel = sprite & (0x80 >> j);
                    // Check if current pixel is on.
                    if (pixel) {
                        // Check if pixel at (VX, VY) is on. If so, set VF to 1.
                        uint16_t coord = (V[x] + j + ((V[y] + i) * DISPLAY_X)) % (DISPLAY_X * DISPLAY_Y);
                        bool collision = (graphics[coord] == 1);
                        // OR with collision because VF is 1 when there is at
                        // least one collision
                        V[0xF] |= collision;
                        graphics[coord] ^= 1;
                    }
                }
            }
            draw_flag = true;
        }
        break;

        case 0xE000: {
            switch (opcode & 0x00FF) {
                // Skips the next instruction if the key stored in VX is pressed
                case 0x009E:
                    if (key[V[x]] != 0) this->pc += 2;
                break;

                // Skips the next instruction if the key stored in VX isn't pressed
                case 0x00A1:
                    if (key[V[x]] == 0) this->pc += 2;
                break;
            }
        }
        break;

        case 0xF000: {
            switch (opcode & 0x00FF) {
                // Set VX to the value of the delay timer.
                case 0x0007: 
                    V[x] = this->delay_timer;
                break;
                
                // A key press is awaited, and then stored in VX. 
                case 0x000A: {
                    int length = sizeof(key) / sizeof(key[0]);
                    for (int i = 0; i < length; i++) {
                        if (key[i]) {
                            V[x] = i; 
                            return;
                        }
                    }
                    // No key pressed; decrement PC to simulate awaiting key press.
                    this->pc--;
                }
                break;

                // Set the delay timer to VX.
                case 0x0015: 
                    this->delay_timer = V[x];
                break;
                
                // Set the sound timer to VX.
                case 0x0018: 
                    this->sound_timer = V[x];
                break;

                // I += VX
                case 0x001E: 
                    this->I += V[x];
                break;

                // Sets I to the location of the sprite for the character in VX.
                case 0x0029:
                    this->I = FONTSET_START_ADDRESS + (V[x] * 5); // Each font character is 5 bytes
                break;

                // Stores the binary-coded decimal representation of VX, with 
                // the most significant of three digits at the address in I, 
                // the middle digit at I plus 1, and the least significant 
                // digit at I plus 2.
                case 0x0033:
                    memory[this->I] = V[x] / 100;
                    memory[this->I + 1] = (V[x] / 10) % 10;
                    memory[this->I + 2] = (V[x] % 100) % 10; 
                break;

                // Stores V0 to VX (including VX) in memory starting at address I
                case 0x0055: 
                    for (int i = 0; i <= x; i++) {
                        memory[i + this->I] = V[i];
                    }
                break;

                // Fills V0 to VX (including VX) with values from memory starting at address I
                case 0x0065: 
                    for (int i = 0; i <= x; i++) {
                        V[i] = memory[i + this->I];
                    }
                break;
            }
        }
        break;
    }
}

