#pragma once

#include "angle.h"

#include "gamemovement.h"
#include "cusercmd.h"

class CBaseEntity;
class PlayerDataBackup {
public:
	void Store(CBasePlayer* player);
	void Restore(CBasePlayer* player);

private:
	Vector m_vecOrigin;
	Vector m_vecVelocity;
	Vector m_vecBaseVelocity;
	Vector m_vecViewOffset;
	CBaseEntity* m_hGroundEntity;
	int m_fFlags;
	float m_flDucktime;
	float m_flDuckJumpTime;
	bool m_bDucked;
	bool m_bDucking;
	bool m_bInDuckJump;
	float m_flModelScale;
};

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

private:
	CBasePlayer* _player;
	CMoveData _moveData;

	bool _oldInPrediction;
	bool _oldFirstTimePredicted;
	float _oldFrameTime;

	PlayerDataBackup _playerDataBackup;

	CUserCmd _dummyCmd;
};

extern MovementSimulation g_simulation;
