#pragma once

#include "vmt.h"

class CBasePlayerAnimState {
public:
	void Update(float eyeYaw, float eyePitch) noexcept {
		return vmt::call<void>((void*)this, 4, eyeYaw, eyePitch);
	}

public:
	char pad_0000[124]; //0x0000
	float move_x; //0x007C
	float move_y; //0x0080
	char pad_0084[8]; //0x0084
	float m_flEyeYaw; //0x008C
	float m_flEyePitch; //0x0090
	float m_flGoalFeetYaw; //0x0094
	float m_flCurrentFeetYaw; //0x0098 
};