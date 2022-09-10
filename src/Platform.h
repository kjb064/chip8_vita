#ifndef Platform_H
#define Platform_H

#include <iostream>
#include <SDL2/SDL.h>

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

        /** Updates the status of a key to pressed or released. */
        void SetKeyStatus(uint8_t* keys, SDL_Keycode key_code, bool pressed);

        /** Outputs an SDL error, frees resources, and causes the application to terminate. */
        void SDLError();
};

#endif