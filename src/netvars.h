#pragma once

#include "recv.h"
#include <unordered_map>
#include <string>

namespace netvars {
	extern std::unordered_map<std::string, int> netvars;
	extern void init();
} 