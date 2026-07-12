#pragma once
#include <cstdint>

namespace ve::platform {

    enum class Key : std::uint8_t {
        W, A, S, D,
        Escape,
        Count // not a key; array size trick
    };

    class Input {
        public:
        void AdvanceSnapshot(); // current -> previous

        bool IsDown(Key k) const;
        bool WasPressed(Key k) const; // edge: down now, up last frame
        bool WasReleased(Key k) const; // edge: up now, down last frame

        void SetKey(Key k, bool down); // fed by Window during event polling

        private:
        bool m_current[static_cast<int>(Key::Count)] = {};
        bool m_previous[static_cast<int>(Key::Count)] = {};
    };
}