#pragma once

namespace oak {

	struct DebugVars {
		float dt = 0.0f;
		float fps = 0.0f;
		size_t usedMemory = 0;
		size_t allocatedMemory = 0;
		int freelistLength = 0;
	};

	extern DebugVars debugVars;

}