#define WIN32_LEAN_AND_MEAN // Skip rarely-used headers: faster compiles
#define NOMINMAX // windows.h defines min/max as MACROS, breaking std::min/max

#include <windows.h>

// Empty namespace in C++ means, this part is private to this .cpp file
namespace {
    struct ClockState {
        double invFrequency; // Inverse frequency. Always precompute inverses of constants when working in per-frame code
        long long start;

        ClockState() {
            LARGE_INTEGER li;
            QueryPerformanceFrequency(&li);
            invFrequency = 1.0 / double(li.QuadPart);
            QueryPerformanceCounter(&li);
            start = li.QuadPart;
        }
    };
}

namespace ve::core {

    double NowSeconds() {

        static const ClockState clock; // Constructor runs on first call, once
        
        LARGE_INTEGER li;
        QueryPerformanceCounter(&li);

        return double(li.QuadPart - clock.start) * clock.invFrequency;
    }
}