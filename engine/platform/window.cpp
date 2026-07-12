#include "engine/core/log.h"
#include "engine/platform/window.h"
#include "engine/platform/input.h"
#include <SDL3/SDL.h>

namespace ve::platform {

    namespace {
        // Helper mapping function
        Key MapScancode(SDL_Scancode sc) {
            switch (sc) {
            case SDL_SCANCODE_W:    return Key::W;
            case SDL_SCANCODE_A:    return Key::A;
            case SDL_SCANCODE_S:    return Key::S;
            case SDL_SCANCODE_D:    return Key::D;
            case SDL_SCANCODE_ESCAPE:    return Key::Escape;
            default:    return Key::Count; // unmapped
            }
        }
    }

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

    void Window::PollEvents(Input& input) {

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

            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP: {
                const Key k = MapScancode(event.key.scancode);
                if (k != Key::Count) input.SetKey(k, event.type == SDL_EVENT_KEY_DOWN);
                break;
            }
            default:
                break;
            }
        }
    }
    
    bool Window::ShouldClose() const {
        return m_shouldClose;
    }

    void Window::RequestClose() {
        m_shouldClose = true;
    }

    void* Window::GetNativeHandle() const {
        return SDL_GetPointerProperty(SDL_GetWindowProperties(m_window),
                                      SDL_PROP_WINDOW_WIN32_HWND_POINTER,
                                      nullptr);
    }
}