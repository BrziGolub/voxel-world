#include "engine/core/time.h"
#include "engine/core/log.h"
#include "engine/platform/platform.h"
#include "engine/platform/window.h"
#include "engine/platform/input.h"

int main() {
	// Initialize platform (SDL)
	if (!ve::platform::Init()) return 1;

	// Window setup
	ve::platform::Window window;
	ve::platform::WindowDesc windowDesc;

	// Input
	ve::platform::Input input;

	// Create Window
	if (!window.Create(windowDesc)) {
		ve::platform::Shutdown();
		return 1;
	}

	// Fixed timestep init
	const double tickRate = 60.0;
	const double dt = 1.0 / tickRate;

	double accumulator = 0.0;
	double lastTime = ve::core::NowSeconds();

	// Rate counter
	int tickCount = 0;
	double nextReport = ve::core::NowSeconds() + 1.0; // One second from now

	// Table of key/name pairs
	struct KeyName { ve::platform::Key key; const char* name; };
	const KeyName wasdKeys[] = {
		{ ve::platform::Key::W, "W" },
		{ ve::platform::Key::A, "A" },
		{ ve::platform::Key::S, "S" },
		{ ve::platform::Key::D, "D" }
	};

	// Frame Loop
	while (!window.ShouldClose()) {

		const double now = ve::core::NowSeconds();
		double frameTime = now - lastTime;
		lastTime = now;

		if (frameTime > 0.25) frameTime = 0.25;

		accumulator += frameTime;

		window.PollEvents(input);

		while (accumulator >= dt) {

			// Input queries
			if (input.WasPressed(ve::platform::Key::Escape)) window.RequestClose();
			for (const auto& [key, name] : wasdKeys) {
				if (input.WasPressed(key)) VE_LOG_INFO("tick: {}: {} pressed", tickCount, name);
				if (input.WasReleased(key)) VE_LOG_INFO("tick: {}: {} released", tickCount, name);
			}

			++tickCount;
			input.AdvanceSnapshot(); // consume: this tick's edges become "previous"
			accumulator -= dt;
		}

		if (now >= nextReport) { // Once per second
			VE_LOG_INFO("ticks/sec: {}", tickCount); // ...report and reset
			tickCount = 0;
			while (nextReport <= now) nextReport += 1.0; // Skip deadlines lost to a stall
		}
	}

	// Free resources
	window.Destroy();
	ve::platform::Shutdown();
	return 0;
}