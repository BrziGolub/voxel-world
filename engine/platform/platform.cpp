#include "engine/core/log.h"
#include <SDL3/SDL.h>

namespace ve::platform {
    bool Init() {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            VE_LOG_INFO("SDL Init failed: {}", SDL_GetError());
            return false;
        }
        const int v = SDL_GetVersion();
        VE_LOG_INFO("SDL {}.{}.{} initialized",
                    SDL_VERSIONNUM_MAJOR(v),
                    SDL_VERSIONNUM_MINOR(v),
                    SDL_VERSIONNUM_MICRO(v));
        return true;
    }

    void Shutdown() {
        SDL_Quit();
    }
}