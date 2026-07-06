#include "engine/platform/platform.h"

int main() {
	// Initialize platform (SDL)
	if (!ve::platform::Init()) return 1;
	ve::platform::Shutdown();
	return 0;
}