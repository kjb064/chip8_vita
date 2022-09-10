#include "Platform.h"

Platform::Platform(std::string window_title, int x, int y) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) SDLError();

    window = SDL_CreateWindow(window_title.c_str(), 
                        SDL_WINDOWPOS_UNDEFINED, 
                        SDL_WINDOWPOS_UNDEFINED, 
                        x * 10, 
                        y * 10, 
                        0);
    if (window == nullptr) this->SDLError();

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) this->SDLError();

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, x, y);
    if (texture == nullptr) this->SDLError();
}

void Platform::SDLError() {
    std::cerr << "Error: " + std::string(SDL_GetError()) << std::endl;
    SDL_Quit();
    std::exit(EXIT_FAILURE);
}

Platform::~Platform() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    std::cout << "Application terminated." << std::endl;
}

void Platform::UpdateDisplay(uint32_t* pixels, int video_pitch) {
    SDL_UpdateTexture(texture, nullptr, pixels, video_pitch);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
}

bool Platform::ProcessInput(uint8_t* keys) {
    bool run = true;

    SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    run = false;
                    std::cout << "Quitting..." << std::endl;
                break;

                case SDL_KEYDOWN:
                    SetKeyStatus(keys, event.key.keysym.sym, true);
                break;

                case SDL_KEYUP:
                    SetKeyStatus(keys, event.key.keysym.sym, false);
                break;
            }
        }
        return run;
}

void Platform::SetKeyStatus(uint8_t* keys, SDL_Keycode key_code, bool pressed) {
    /** 
     *  Chip 8 Keypad             Keyboard
        +-+-+-+-+                +-+-+-+-+
        |1|2|3|C|                |1|2|3|4|
        +-+-+-+-+                +-+-+-+-+
        |4|5|6|D|                |Q|W|E|R|
        +-+-+-+-+       =>       +-+-+-+-+
        |7|8|9|E|                |A|S|D|F|
        +-+-+-+-+                +-+-+-+-+
        |A|0|B|F|                |Z|X|C|V|
        +-+-+-+-+                +-+-+-+-+
    */
    switch (key_code) {
        case SDLK_1:
            keys[0x0] = pressed;
        break;

        case SDLK_2:
            keys[0x1] = pressed;
        break;

        case SDLK_3:
            keys[0x2] = pressed;
        break;

        case SDLK_4:
            keys[0x3] = pressed;
        break;

        case SDLK_q:
            keys[0x4] = pressed;
        break;

        case SDLK_w:
            keys[0x5] = pressed;
        break;

        case SDLK_e:
            keys[0x6] = pressed;
        break;

        case SDLK_r:
            keys[0x7] = pressed;
        break;

        case SDLK_a:
            keys[0x8] = pressed;
        break;

        case SDLK_s:
            keys[0x9] = pressed;
        break;

        case SDLK_d:
            keys[0xA] = pressed;
        break;

        case SDLK_f:
            keys[0xB] = pressed;
        break;

        case SDLK_z:
            keys[0xC] = pressed;
        break;

        case SDLK_x:
            keys[0xD] = pressed;
        break;

        case SDLK_c:
            keys[0xE] = pressed;
        break;

        case SDLK_v:
            keys[0xF] = pressed;
        break;
    }
}