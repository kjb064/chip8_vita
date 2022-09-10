#ifndef CPU_H
#define CPU_H

#include <fstream>
#include <random>

const int DISPLAY_X = 64;
const int DISPLAY_Y = 32;

class CPU {
    public:
        bool draw_flag = false;
        CPU();
        /** Loads the ROM data from the file with the given path. */
        void LoadData(std::string input_file_path);

        /** Emulates a CPU cycle (fetch, decode, execute). */
        void EmulateCycle();

        void UpdateTimers();

        uint8_t* GetKeys();
        uint8_t* GetGraphics();
    private:

        const unsigned short START_ADDRESS = 0x200;
        const unsigned short FONTSET_START_ADDRESS = 0x50;
        const static int FONTSET_SIZE = 80;

        // Chip 8 graphics system. The display is 64x32 (2048) pixels.
        uint8_t graphics[DISPLAY_X * DISPLAY_Y] = {0};

        /** Represents Chip 8's HEX based keypad of 16 keys. */
        uint8_t key[16] = {0};

        /** The Chip 8 font set. Numbers/characters are
         * 4 pixels wide and 5 pixels high.
        */
        uint8_t fontset[FONTSET_SIZE] = {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };

        /** 15 8-bit registers (V0-VE). Register 16 holds the carry flag. */
        uint8_t V[16] = {0};

        /** 
         * 4K memory. 
         * 
         * 0x000 - 0x1FF: Reserved for Chip 8 interpreter
         * 0x050 - 0x0A0: Used for the built in 4x5 pixel font set (0-F)
         * 0x200 - 0xFFF: Program ROM and work RAM
         */
        uint8_t memory[4096] = {0};

        /** 16-bit Index register (values 0x000-0xFFF) */
        uint16_t I;

        /** 
         * Program counter (values 0x000-0xFFF).
         * 
         * Starts at 0x200.
         */
        unsigned short pc;

        /** Stack to record the current location prior to a jump. Has 16 levels. */
        unsigned short stack[16] = {0};

        /** Stack pointer */
        unsigned short sp;

        uint8_t delay_timer;
        uint8_t sound_timer;

        /** 
         * Fetches the opcode (from memory) pointed to by the PC.
         *  
         * @return the opcode
         */
        unsigned short FetchOpcode();

        /** 
         * Decodes the given opcode to determine how to handle it and
         * then executes the opcode accordingly.
         */
        void DecodeAndExecute(unsigned short opcode);
};

#endif