#include "engine/platform/input.h"
#include <cstring>

namespace ve::platform {

    void Input::AdvanceSnapshot() {
        std::memcpy(m_previous, m_current, sizeof m_current);
    }

    bool Input::IsDown(Key k) const {
        return m_current[static_cast<int>(k)];
    }
    bool Input::WasPressed(Key k) const {
        return m_current[static_cast<int>(k)] && !m_previous[static_cast<int>(k)];
    }
    bool Input::WasReleased(Key k) const {
        return !m_current[static_cast<int>(k)] && m_previous[static_cast<int>(k)];
    }

    void Input::SetKey(Key k, bool down) {
        m_current[static_cast<int>(k)] = down;
    }
}