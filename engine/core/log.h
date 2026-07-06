#pragma once
#include <fmt/core.h>

namespace ve::core
{
	void LogMessage(const char *level, fmt::string_view fmtStr, fmt::format_args args);

	template <typename... Args>
	void LogInfo(fmt::format_string<Args...> fmtStr, Args &&...args)
	{
		LogMessage("INFO", fmtStr.str, fmt::make_format_args(args...));
	}
}

#define VE_LOG_INFO(...) ::ve::core::LogInfo(__VA_ARGS__)