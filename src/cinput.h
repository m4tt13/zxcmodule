#pragma once

#include "cusercmd.h"

#include <cstdint>

#define MULTIPLAYER_BACKUP 90

class CInput {
public:
	CUserCmd* GetCommands() {
		return *reinterpret_cast<CUserCmd**>(reinterpret_cast<std::uintptr_t>(this) + 0xD0);
	}

	CUserCmd* GetCommand(int seq) {
		CUserCmd* verifiedCmd = reinterpret_cast<CUserCmd*>(reinterpret_cast<std::uintptr_t>(GetCommands()) + 324 * (seq % MULTIPLAYER_BACKUP));
		return verifiedCmd;
	}

	CVerifiedUserCmd* GetVerifiedCommands() {
		return *reinterpret_cast<CVerifiedUserCmd**>(reinterpret_cast<std::uintptr_t>(this) + 0xD8);
	}

	CVerifiedUserCmd* GetVerifiedCommand(int seq) {
		CVerifiedUserCmd* verifiedCmd = reinterpret_cast<CVerifiedUserCmd*>(reinterpret_cast<std::uintptr_t>(GetVerifiedCommands()) + 328 * (seq % MULTIPLAYER_BACKUP));
		return verifiedCmd;
	}
};