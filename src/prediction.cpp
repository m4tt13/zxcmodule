
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

Prediction::Prediction() : _moveData {0}, _oldCurTime(0.f), _oldFrameTime(0.f), _predictionRandomSeed(nullptr) {
	_predictionRandomSeed = reinterpret_cast<int*>(getAbsAddr(findPattern("client.dll", "0F B6 1D ?? ?? ?? ?? 0F 29 74 24")));
}

float Prediction::GetServerTime(CUserCmd* cmd) const {
	CBasePlayer* localPlayer = reinterpret_cast<CBasePlayer*>(interfaces::entityList->GetClientEntity(interfaces::engineClient->GetLocalPlayer()));

	static CUserCmd* lastCmd = nullptr;

	if (cmd)
		lastCmd = cmd;

	if (!localPlayer) {
		return interfaces::globalVars->curtime;
	}

	int tickBase = localPlayer->m_nTickBase();

	if (lastCmd && !lastCmd->hasbeenpredicted)
		tickBase++;

	return tickBase * interfaces::globalVars->interval_per_tick;
}

void Prediction::Start(CUserCmd* cmd) {
	CBasePlayer* localPlayer = reinterpret_cast<CBasePlayer*>(interfaces::entityList->GetClientEntity(interfaces::engineClient->GetLocalPlayer()));

	localPlayer->GetCurrentCommand() = cmd;
	*_predictionRandomSeed = cmd->random_seed;

	_oldCurTime = interfaces::globalVars->curtime;
	_oldFrameTime = interfaces::globalVars->frametime;
	// _oldVelocity = localPlayer->GetVelocity();

	interfaces::globalVars->curtime = localPlayer->m_nTickBase() * interfaces::globalVars->interval_per_tick;
	interfaces::globalVars->frametime = interfaces::globalVars->interval_per_tick;

	interfaces::prediction->SetLocalViewAngles(cmd->viewangles);
	interfaces::gameMovement->StartTrackPredictionErrors(localPlayer);

	memset(&_moveData, 0, sizeof(_moveData));

	interfaces::prediction->SetupMove(localPlayer, cmd, interfaces::moveHelper, &_moveData);
	interfaces::gameMovement->ProcessMovement(localPlayer, &_moveData);
	interfaces::prediction->FinishMove(localPlayer, cmd, &_moveData);



	if (interfaces::globalVars->frametime > 0.f)
		localPlayer->m_nTickBase()++;
}

void Prediction::Finish() {
	CBasePlayer* localPlayer = reinterpret_cast<CBasePlayer*>(interfaces::entityList->GetClientEntity(interfaces::engineClient->GetLocalPlayer()));

	interfaces::gameMovement->FinishTrackPredictionErrors(localPlayer);

	interfaces::globalVars->curtime = _oldCurTime;
	interfaces::globalVars->frametime = _oldFrameTime;
	// localPlayer->GetVelocity() = _oldVelocity;

	*_predictionRandomSeed = -1;
	localPlayer->GetCurrentCommand() = nullptr;
}

Prediction g_prediction;
