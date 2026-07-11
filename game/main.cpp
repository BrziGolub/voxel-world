#include "engine/core/time.h"
#include "engine/core/log.h"
#include "engine/platform/platform.h"
#include "engine/platform/window.h"

int main() {
	// Initialize platform (SDL)
	if (!ve::platform::Init()) return 1;

	// Window setup
	ve::platform::Window window;
	ve::platform::WindowDesc windowDesc;

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

	// Main Game Loop
	while (!window.ShouldClose()) {

		const double now = ve::core::NowSeconds();
		double frameTime = now - lastTime;
		lastTime = now;

		if (frameTime > 0.25) frameTime = 0.25;

		accumulator += frameTime;

		window.PollEvents();

		while (accumulator >= dt) {
			++tickCount;
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