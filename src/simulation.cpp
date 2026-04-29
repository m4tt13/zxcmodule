
#include "globalvars.h"
#include "interfaces.h"
#include "entity.h"
#include "cprediction.h"
#include "predictioncopy.h"
#include "simulation.h"

MovementSimulation::MovementSimulation() : _player(nullptr), _moveData{0}, _oldInPrediction(false), _oldFirstTimePredicted(false), _oldFrameTime(0.0f) {
}

void MovementSimulation::Start(CBasePlayer* player) {
	_player = player;

	player->GetCurrentCommand() = &_dummyCmd;
	player->SetPredictionPlayer(player->GetClientUnknown()->GetRefEHandle());

	Store(player);

	_oldInPrediction = interfaces::prediction->InPrediction();
	_oldFirstTimePredicted = interfaces::prediction->IsFirstTimePredicted();
	_oldFrameTime = interfaces::globalVars->frametime;

	// the hacks that make it work
	player->m_bDucked() = player->IsDucking();
	player->m_bDucking() = false;
	player->m_bInDuckJump() = false;
	player->m_flDucktime() = 0.0f;
	player->m_flDuckJumpTime() = 0.0f;
	player->m_flJumpTime() = 0.0f;
	player->m_hGroundEntity() = player->IsOnGround() ? interfaces::entityList->GetClientEntity(0)->GetClientUnknown()->GetRefEHandle() : INVALID_EHANDLE_INDEX;

	if (player->IsOnGround())
		player->GetAbsOrigin().z += 0.03125f; //to prevent getting stuck in the ground

	// Setup move data
	SetupMoveData(player);
}

void MovementSimulation::SimulateTick() {
	if (!_player)
		return;

	interfaces::prediction->GetInPrediction() = true;
	interfaces::prediction->GetIsFirstTimePredicted() = false;
	interfaces::globalVars->frametime = interfaces::prediction->GetEnginePaused() ? 0.0f : interfaces::globalVars->interval_per_tick;

	interfaces::gameMovement->ProcessMovement(_player, &_moveData);
}

void MovementSimulation::Finish() {
	if (!_player)
		return;

	_player->GetCurrentCommand() = nullptr;
	_player->SetPredictionPlayer(INVALID_EHANDLE_INDEX);

	Restore(_player);

	interfaces::prediction->GetInPrediction() = _oldInPrediction;
	interfaces::prediction->GetIsFirstTimePredicted() = _oldFirstTimePredicted;
	interfaces::globalVars->frametime = _oldFrameTime;

	_player = nullptr;

	memset(&_moveData, 0, sizeof(_moveData));
} 

void MovementSimulation::SetupMoveData(CBasePlayer* player) {
	_moveData.m_bFirstRunOfFunctions = false;
	_moveData.m_bGameCodeMovedPlayer = false;
	_moveData.m_nPlayerHandle = player->GetClientUnknown()->GetRefEHandle();
	_moveData.m_vecVelocity = player->GetVelocity();
	_moveData.m_vecAbsOrigin = player->GetAbsOrigin();
	_moveData.m_vecViewAngles = { player->EyePitch(), player->EyeYaw(), 0.0f };
	_moveData.m_vecAngles = _moveData.m_vecViewAngles;
	_moveData.m_vecOldAngles = _moveData.m_vecViewAngles;
	_moveData.m_nButtons = player->IsDucking() ? CUserCmd::IN_DUCK : 0;
	_moveData.m_nOldButtons = _moveData.m_nButtons;
	_moveData.m_flClientMaxSpeed = player->m_flMaxspeed();

	Vector forward = _moveData.m_vecViewAngles.Forward();
	forward.z = 0.0f;
	forward.Normalize();

	Vector right = _moveData.m_vecViewAngles.Right();
	right.z = 0.0f;
	right.Normalize();

	float divisor = forward.x * right.y - right.x * forward.y;
	_moveData.m_flForwardMove = (_moveData.m_vecVelocity.x * right.y - right.x * _moveData.m_vecVelocity.y) / divisor;
	_moveData.m_flSideMove = (forward.x * _moveData.m_vecVelocity.y - _moveData.m_vecVelocity.x * forward.y) / divisor;
}

void MovementSimulation::Store(CBasePlayer* player) {
	player->AllocateIntermediateData();

	assert(player->m_pOriginalData());
	CPredictionCopy copyHelper( PC_EVERYTHING, player->m_pOriginalData(), PC_DATA_PACKED, player, PC_DATA_NORMAL );
	copyHelper.TransferData("MovementSimulationStore", player->GetClientNetworkable()->entIndex(), player->GetPredDescMap());
}

void MovementSimulation::Restore(CBasePlayer* player) {
	assert(player->m_pOriginalData());
	CPredictionCopy copyHelper( PC_EVERYTHING, player, PC_DATA_NORMAL, player->m_pOriginalData(), PC_DATA_PACKED );
	copyHelper.TransferData("MovementSimulationRestore", player->GetClientNetworkable()->entIndex(), player->GetPredDescMap());
}

MovementSimulation g_simulation;