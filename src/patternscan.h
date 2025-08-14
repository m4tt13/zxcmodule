#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <Psapi.h>
#include <cstdint>
#include <vector>

#define INRANGE(x, a, b) (x >= a && x <= b) 
#define GETBITS(x) (INRANGE((x & (~0x20)), 'A', 'F') ? ((x & (~0x20)) - 'A' + 0xA) : (INRANGE(x, '0', '9') ? x - '0' : 0))
#define GETBYTE(x) (GETBITS(x[0]) << 4 | GETBITS(x[1]))

static std::uintptr_t findPattern(const char* moduleName, const char* pattern) {
	HMODULE module = GetModuleHandleA(moduleName);

	MODULEINFO moduleInfo;
	GetModuleInformation(GetCurrentProcess(), module, &moduleInfo, sizeof(MODULEINFO));

	std::uintptr_t start = reinterpret_cast<std::uintptr_t>(module);
	std::uintptr_t end = start + moduleInfo.SizeOfImage;

	for (std::uintptr_t addr = start; addr < end; addr++) {
		bool found = true;

		int off = 0;
		for (const char* pat = pattern; *pat;) {
			if (*pat == ' ') {
				pat++;
				continue;
			}
			else if (*pat == '?') {
				pat += 2;
				off++;
				continue;
			}

			if (*reinterpret_cast<BYTE*>(addr + off) != GETBYTE(pat)) {
				found = false;
				break;
			}

			pat += 2;
			off++;
		}

		if (found)
			return addr;
	}

	return NULL;
}

