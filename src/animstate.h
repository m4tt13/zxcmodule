#pragma once

#include "util.h"

class CBasePlayerAnimState {
public:
	VPROXY(Update, 4, void, (float eyeYaw, float eyePitch), eyeYaw, eyePitch);

public:
	char pad_0000[120]; //0x0000
	float m_flEyeYaw; //0x0078
	float m_flEyePitch; //0x007C
	float m_flGoalFeetYaw; //0x0080
	float m_flCurrentFeetYaw; //0x0084 
};