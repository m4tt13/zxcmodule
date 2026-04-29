#pragma once

#include "util.h"

struct model_t;

class IVModelInfo {
public:
	VPROXY(GetModelName, 3, const char*, (const model_t *model), model);
};