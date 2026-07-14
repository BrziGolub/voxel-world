#include "engine/core/log.h"
#include <fmt/chrono.h>
#include <chrono>
#include <cstdio>

namespace ve::core {

	namespace {

		struct FileState {
			FILE* file;

			FileState() {
				file = fopen("voxel-world.log", "w");
				if(!file) fmt::print("[LOG] failed to open voxel-world.log; file sink disabled\n");
			}

			~FileState() {
				if (file) fclose(file);
			}
		};

		FILE* LogFile() {

			static const FileState file;

			return file.file;
		}
	}

	void LogMessage(const char* level, fmt::string_view fmtStr, fmt::format_args args) {
		auto now = std::chrono::system_clock::now();

		std::string log;
		log += fmt::format("[{:%H:%M:%S}] [{}] ", now, level);
		log += fmt::vformat(fmtStr, args);
		log += '\n';

		// Console logging
		fmt::print("{}", log);

		// File sink logging
		FILE* file = LogFile();
		if (file) {
			fmt::print(file, "{}", log);
			fflush(file);
		}
	}
}