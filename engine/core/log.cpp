#include "engine/core/log.h"
#include <fmt/chrono.h>
#include <chrono>

namespace ve::core {
	void LogMessage(const char* level, fmt::string_view fmtStr, fmt::format_args args) {
		auto now = std::chrono::system_clock::now();
		fmt::print("[{:%H:%M:%S}] [{}] ", now, level);
		fmt::vprint(fmtStr, args);
		fmt::print("\n");
	}
}