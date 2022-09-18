#include "Platform.h"

Platform::Platform(std::string window_title, int x, int y) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) SDLError();

    window = SDL_CreateWindow(window_title.c_str(), 
                        SDL_WINDOWPOS_UNDEFINED, 
                        SDL_WINDOWPOS_UNDEFINED, 
                        960, /** PS Vita screen: 960x544 */
                        544, 
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
    SceCtrlData ctrl_data;
    sceCtrlPeekBufferPositive(0, &ctrl_data, 1);
    if ((ctrl_data.buttons & SCE_CTRL_SELECT)) {
        // Quit
        run = false;
    } else {
        SetKeyStatus(keys, ctrl_data);
    }
    return run;
}

void Platform::SetKeyStatus(uint8_t* keys, SceCtrlData ctrl_data) {
    /**
    *   Chip 8 Keypad             Vita Controls
        +-+-+-+-+                +-+-+-+-+
        |1|2|3|C|                |U|R|D|Le|
        +-+-+-+-+                +-+-+-+-+
        |4|5|6|D|                |T|C|X|S|
        +-+-+-+-+       =>       +-+-+-+-+
        |7|8|9|E|                |L+U|L+R|L+D|L+Le|
        +-+-+-+-+                +-+-+-+-+
        |A|0|B|F|                |L+T|L+C|L+X|L+S|
        +-+-+-+-+ 
   */
    bool modifier_pressed = (ctrl_data.buttons & SCE_CTRL_LTRIGGER);

    keys[Control::ONE] = !modifier_pressed && (ctrl_data.buttons & SCE_CTRL_UP);
    keys[Control::TWO] = !modifier_pressed && (ctrl_data.buttons & SCE_CTRL_RIGHT);
    keys[Control::THREE] = !modifier_pressed && (ctrl_data.buttons & SCE_CTRL_DOWN);
    keys[Control::C] = !modifier_pressed && (ctrl_data.buttons & SCE_CTRL_LEFT);

    keys[Control::FOUR] = !modifier_pressed && (ctrl_data.buttons & SCE_CTRL_TRIANGLE);
    keys[Control::FIVE] = !modifier_pressed && (ctrl_data.buttons & SCE_CTRL_CIRCLE);
    keys[Control::SIX] = !modifier_pressed && (ctrl_data.buttons & SCE_CTRL_CROSS);
    keys[Control::D] = !modifier_pressed && (ctrl_data.buttons & SCE_CTRL_SQUARE);

    keys[Control::SEVEN] = modifier_pressed && (ctrl_data.buttons & SCE_CTRL_UP);
    keys[Control::EIGHT] = modifier_pressed && (ctrl_data.buttons & SCE_CTRL_RIGHT);
    keys[Control::NINE] = modifier_pressed && (ctrl_data.buttons & SCE_CTRL_DOWN);
    keys[Control::E] = modifier_pressed && (ctrl_data.buttons & SCE_CTRL_LEFT);

    keys[Control::A] = modifier_pressed && (ctrl_data.buttons & SCE_CTRL_TRIANGLE);
    keys[Control::ZERO] = modifier_pressed && (ctrl_data.buttons & SCE_CTRL_CIRCLE);
    keys[Control::B] = modifier_pressed && (ctrl_data.buttons & SCE_CTRL_CROSS);
    keys[Control::F] = modifier_pressed && (ctrl_data.buttons & SCE_CTRL_SQUARE);
}