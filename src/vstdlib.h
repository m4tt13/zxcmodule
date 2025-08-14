
#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdint>

namespace vstdlib {
	static void RandomSeed(uint32_t seed) {
		using RandomSeedFn = float(__fastcall*)(uint32_t);
		static RandomSeedFn func = reinterpret_cast<RandomSeedFn>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomSeed"));
		func(seed);
	}


	static float RandomFloat(float min, float max) {
		using RandomFloatFn = float(__fastcall*)(float, float);
		static RandomFloatFn func = reinterpret_cast<RandomFloatFn>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomFloat"));
		return func(min, max);
	}
}
 