#ifndef Platform_H
#define Platform_H

#include <iostream>
#include <SDL2/SDL.h>
#include <psp2/ctrl.h>

class Platform {
    public:
        Platform(std::string, int x, int y);
        ~Platform();

        /** Checks if a key has been pressed or released. */
        bool ProcessInput(uint8_t* keys);

        /** Updates the display window with the given pixels. */
        void UpdateDisplay(uint32_t* pixels, int video_pitch);
    private:
        SDL_Window* window{};
        SDL_Renderer* renderer{};
        SDL_Texture* texture{};

        /** 
         * Enum to represent the keys on a Chip 8 keypad. 
         * Enum values are important since they are used to index an array.
         */
        enum Control {
            ONE,    TWO,    THREE,  C,
            FOUR,   FIVE,   SIX,    D,
            SEVEN,  EIGHT,  NINE,   E,
            A,      ZERO,   B,      F
        };

        /** Updates the status of all keys to pressed or released. */
        void SetKeyStatus(uint8_t* keys, SceCtrlData ctrl_data);

        /** Outputs an SDL error, frees resources, and causes the application to terminate. */
        void SDLError();
};

#endif