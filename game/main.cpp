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

	// Main Game Loop
	while (!window.ShouldClose()) {
		window.PollEvents();
	}

	// Free resources
	window.Destroy();
	ve::platform::Shutdown();
	return 0;
}