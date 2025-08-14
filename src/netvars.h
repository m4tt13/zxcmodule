#pragma once

#include "recv.h"
#include <unordered_map>
#include <string>

struct netvar_t {
	SendPropType type;
	int offset;
};

namespace netvars {
	extern std::unordered_map<std::string, netvar_t> netvars;
	extern void init();
} 