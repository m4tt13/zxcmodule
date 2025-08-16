
#include "prediction.h"
#include "interfaces.h"
#include "cliententitylist.h"
#include "engineclient.h"
#include "globalvars.h"
#include "entity.h"
#include "cusercmd.h"
#include "cprediction.h"
#include "gamemovement.h"
#include "patternscan.h"
#include "util.h"

Prediction::Prediction() : _moveData {0}, _oldCurTime(0.f), _oldFrameTime(0.f), _predictionRandomSeed(nullptr), _predictionPlayer(nullptr) {
	_predictionRandomSeed = reinterpret_cast<int*>(getAbsAddr(findPattern("client.dll", "0F B6 1D ?? ?? ?? ?? 0F 29 74 24")));
	_predictionPlayer = reinterpret_cast<CBasePlayer**>(getAbsAddr(findPattern("client.dll", "48 89 3D ?? ?? ?? ?? 66 0F 6E 87")));
}

void Prediction::Start(CUserCmd* cmd) {
	CBasePlayer* localPlayer = reinterpret_cast<CBasePlayer*>(interfaces::entityList->GetClientEntity(interfaces::engineClient->GetLocalPlayer()));

	localPlayer->GetCurrentCommand() = cmd;
	*_predictionRandomSeed = cmd->random_seed;
	*_predictionPlayer = localPlayer;

	_oldCurTime = interfaces::globalVars->curtime;
	_oldFrameTime = interfaces::globalVars->frametime;

	interfaces::globalVars->curtime = localPlayer->m_nTickBase() * interfaces::globalVars->interval_per_tick;
	interfaces::globalVars->frametime = interfaces::globalVars->interval_per_tick;

	bool OldIsFirstTimePredicted = interfaces::prediction->GetIsFirstTimePredicted();
	bool OldInPrediction = interfaces::prediction->GetInPrediction();

	interfaces::prediction->GetIsFirstTimePredicted() = false;
	interfaces::prediction->GetInPrediction() = true;

	interfaces::prediction->SetLocalViewAngles(cmd->viewangles);
	interfaces::gameMovement->StartTrackPredictionErrors(localPlayer);

	memset(&_moveData, 0, sizeof(_moveData));

	interfaces::prediction->SetupMove(localPlayer, cmd, interfaces::moveHelper, &_moveData);
	interfaces::gameMovement->ProcessMovement(localPlayer, &_moveData);
	interfaces::prediction->FinishMove(localPlayer, cmd, &_moveData);

	interfaces::prediction->GetIsFirstTimePredicted() = OldIsFirstTimePredicted;
	interfaces::prediction->GetInPrediction() = OldInPrediction;
}

void Prediction::Finish() {
	CBasePlayer* localPlayer = reinterpret_cast<CBasePlayer*>(interfaces::entityList->GetClientEntity(interfaces::engineClient->GetLocalPlayer()));

	interfaces::gameMovement->FinishTrackPredictionErrors(localPlayer);

	interfaces::globalVars->curtime = _oldCurTime;
	interfaces::globalVars->frametime = _oldFrameTime;

	localPlayer->GetCurrentCommand() = nullptr;
	*_predictionRandomSeed = -1;
	*_predictionPlayer = nullptr;
}

Prediction g_prediction;
