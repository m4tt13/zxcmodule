
#pragma once

#include <cstdint>

#include "angle.h"
#include "gamemovement.h"
 
class CUserCmd;
class Prediction {
public:
	Prediction();
	~Prediction() = default;

	void Start(CUserCmd* cmd);
	void Finish();

private:
	CMoveData _moveData;

	double _oldCurTime;
	float _oldFrameTime;
};

extern Prediction g_prediction;
