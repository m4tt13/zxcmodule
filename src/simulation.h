#pragma once

#include <vector>

#include "angle.h"

#include "gamemovement.h"
#include "cusercmd.h"

struct datamap_t;
class CBasePlayer;
class MovementSimulation {
public:
	MovementSimulation();
	~MovementSimulation() = default;

	void Start(CBasePlayer* player);
	void SimulateTick();
	void Finish();
	void DestroyBackupData();

	CMoveData& GetMoveData() { return _moveData; }

private:
	int CollectDataTableOffsets_R(datamap_t *map);
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

	unsigned char* _backup;
	std::vector<int> _dataTableOffsets;
};

extern MovementSimulation g_simulation;
