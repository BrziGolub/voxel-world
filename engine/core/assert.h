#pragma once
#include "engine/core/log.h"

#if VE_ASSERTS_ENABLED

namespace ve::core {

    template <typename... Args>
    void ReportAssertFailure(const char* file, int line, const char* condStr, fmt::format_string<Args...> fmtStr, Args&&...args) {
        LogMessage("ASSERT", "{}({}): assertion '{}' failed", fmt::make_format_args(file, line, condStr));
        LogMessage("ASSERT", fmtStr.str, fmt::make_format_args(args...));
    }
}

#if defined(_MSC_VER)
#define VE_DEBUG_BREAK() __debugbreak()
#elif defined(__clang__)
#define VE_DEBUG_BREAK() __builtin_debugtrap()
#elif defined(__GNUC__)
#define VE_DEBUG_BREAK() __builtin_trap()
#else
#error "VE_DEBUG_BREAK not implemented for this compiler"
#endif

#define VE_ASSERT(cond, ...)\
    do {\
        if (!(cond)) {\
            ::ve::core::ReportAssertFailure(__FILE__, __LINE__, #cond, __VA_ARGS__);\
            VE_DEBUG_BREAK();\
        }\
    } while (0)

#else

#define VE_ASSERT(cond, ...) ((void)sizeof(cond))

#endif