
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

Prediction::Prediction() : _moveData {0}, _oldCurTime(0.0), _oldFrameTime(0.0f) {
}

void Prediction::Start(CUserCmd* cmd) {
	CBasePlayer* localPlayer = reinterpret_cast<CBasePlayer*>(interfaces::entityList->GetClientEntity(interfaces::engineClient->GetLocalPlayer()));

	localPlayer->GetCurrentCommand() = cmd;
	localPlayer->SetPredictionRandomSeed( cmd->random_seed );
	localPlayer->SetPredictionPlayer( localPlayer->GetClientUnknown()->GetRefEHandle() );

	_oldCurTime = interfaces::globalVars->curtime;
	_oldFrameTime = interfaces::globalVars->frametime;

	interfaces::globalVars->curtime = localPlayer->m_nTickBase() * interfaces::globalVars->interval_per_tick;
	interfaces::globalVars->frametime = interfaces::prediction->GetEnginePaused() ? 0.0f : interfaces::globalVars->interval_per_tick;

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
	localPlayer->SetPredictionRandomSeed( -1 );
	localPlayer->SetPredictionPlayer( INVALID_EHANDLE_INDEX );
}

Prediction g_prediction;
