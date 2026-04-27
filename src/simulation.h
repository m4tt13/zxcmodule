#pragma once

#include "angle.h"

#include "gamemovement.h"
#include "cusercmd.h"

class CBasePlayer;
class MovementSimulation {
public:
	MovementSimulation();
	~MovementSimulation() = default;

	void Start(CBasePlayer* player);
	void SimulateTick();
	void Finish();

	CMoveData& GetMoveData() { return _moveData; }

private:
	void SetupMoveData(CBasePlayer* player);
	void Store(CBasePlayer* player);
	void Restore(CBasePlayer* player);

private:
	CBasePlayer* _player;
	CMoveData _moveData;

	bool _oldInPrediction;
	bool _oldFirstTimePredicted;
	float _oldFrameTime;

	CUserCmd _dummyCmd;
};

extern MovementSimulation g_simulation;
