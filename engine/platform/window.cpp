/*
Functions to look up for in docs:
    SDL_CreateWindow
    SDL_DestroyWindow
    SDL_PollEvent
        with SDL_Event: SDL_EVENT_QUIT and SDL_EVENT_WINDOW_CLOSE_REQUESTED
        Drain poll events in while -> (SDL_PollEvent(&e))
    GetNativeHandle: search for SDL_PROP_WINDOW_WIN32_HWND_POINTER
*/

#include "engine/platform/window.h"
#include "engine/core/log.h"
#include <SDL3/SDL.h>

namespace ve::platform {

    Window::~Window() {
        Destroy();
    }

    bool Window::Create(const WindowDesc& desc) {

        if (m_window) return false;

        m_window = SDL_CreateWindow(desc.title, static_cast<int>(desc.width), static_cast<int>(desc.height), 0);

        if (!m_window) {
            VE_LOG_INFO("Window Create failed: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    void Window::Destroy() {
        if (m_window) {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
        }
    }

    void Window::PollEvents() {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                m_shouldClose = true;
                break;

            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                m_shouldClose = true;
                break;
            
            default:
                break;
            }
        }
    }
    
    bool Window::ShouldClose() const {
        return m_shouldClose;
    }

    void* Window::GetNativeHandle() const {
        return SDL_GetPointerProperty(SDL_GetWindowProperties(m_window),
                                      SDL_PROP_WINDOW_WIN32_HWND_POINTER,
                                      nullptr);
    }
}