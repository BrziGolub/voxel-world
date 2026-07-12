#pragma once
#include <cstdint>

struct SDL_Window;

namespace ve::platform {

    class Input;

    struct WindowDesc {
        const char* title = "voxel-world";
        std::uint32_t width = 1280;
        std::uint32_t height = 720;
    };

    class Window {
        public:
        Window() = default;
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        bool Create(const WindowDesc& desc);
        void Destroy();

        void PollEvents(Input& input); // Drain SDL's event queue
        bool ShouldClose() const; // Did the user ask to quit?
        void RequestClose(); // Setter function for quitting

        void* GetNativeHandle() const; // HWND, opaquely - for RHI later

        private:
        SDL_Window* m_window = nullptr;
        bool m_shouldClose = false;
    };

}