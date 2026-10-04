
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <fstream>

#define GMOD_USE_SOURCESDK
#include "GarrysMod/Lua/Interface.h"
#include "GarrysMod/Lua/Types.h"
using namespace GarrysMod::Lua;

#include "vstdlib.h"
#include "md5.h"
#include "util.h"
#include "convar.h"
#include "spoofedconvar.h"
#include "cusercmd.h"
#include "prediction.h"
#include "simulation.h"
#include "engineclient.h"
#include "inetchannel.h"
#include "clientstate.h"
#include "globalvars.h"
#include "luashared.h"
#include "globals.h"
#include "interfaces.h"
#include "cliententitylist.h"
#include "entity.h"

#include "hooks.h"

#define GET_ENTITY(STACK_POS) \
	LUA->CheckType(STACK_POS, Type::Entity); \
	CBaseEntity *Ent = nullptr; \
	CBaseHandle *hEnt = LUA->GetUserType<CBaseHandle>(STACK_POS, Type::Entity); \
	if (hEnt) Ent = interfaces::entityList->GetClientEntityFromHandle(*hEnt); \
	if (!Ent) LUA->ThrowError("Tried to use a NULL entity!");

#define GET_PLAYER(STACK_POS) \
	GET_ENTITY(STACK_POS) \
	CBasePlayer *Ply = ToBasePlayer(Ent); \
	if (!Ply) LUA->ThrowError("Player entity is NULL or not a player (!?)");

std::vector<std::unique_ptr<SpoofedConVar>> spoofedConVars;

// Engine client 
LUA_FUNCTION(ServerCmd) {
	LUA->CheckString(1);

	interfaces::engineClient->ServerCmd(LUA->GetString(1), LUA->IsType(2, Type::Bool) ? LUA->GetBool(2) : true);

	return 0;
}

LUA_FUNCTION(ClientCmd) {
	LUA->CheckString(1);

	interfaces::engineClient->ClientCmd(LUA->GetString(1));

	return 0;
}

LUA_FUNCTION(GetLocalPlayer) {
	LUA->PushNumber(interfaces::engineClient->GetLocalPlayer());

	return 1;
}

LUA_FUNCTION(GetTime) {
	LUA->PushNumber(interfaces::engineClient->Time());

	return 1;
}

LUA_FUNCTION(GetLastTimeStamp) {
	LUA->PushNumber(interfaces::engineClient->GetLastTimeStamp());

	return 1;
}

LUA_FUNCTION(GetViewAngles) {
	Angle va;
	interfaces::engineClient->GetViewAngles(&va);

	LUA->PushAngle(va);

	return 1;
}

LUA_FUNCTION(SetViewAngles) {
	LUA->CheckType(1, Type::Angle);

	Angle va = LUA->GetAngle(1);
	interfaces::engineClient->SetViewAngles(&va);

	return 0;
}

LUA_FUNCTION(IsBoxVisible) {
	LUA->CheckType(1, Type::Vector);
	LUA->CheckType(2, Type::Vector);

	LUA->PushBool(interfaces::engineClient->IsBoxVisible(LUA->GetVector(1), LUA->GetVector(2)));

	return 1;
}

LUA_FUNCTION(IsBoxInViewCluster) {
	LUA->CheckType(1, Type::Vector);
	LUA->CheckType(2, Type::Vector);

	LUA->PushBool(interfaces::engineClient->IsBoxInViewCluster(LUA->GetVector(1), LUA->GetVector(2)));

	return 1;
}

LUA_FUNCTION(GetGameDirectory) {
	LUA->PushString(interfaces::engineClient->GetGameDirectory());

	return 1;
}

LUA_FUNCTION(WorldToScreenMatrix) {
	VMatrix mat = interfaces::engineClient->WorldToScreenMatrix();
	LUA->PushUserType_Value(mat, Type::Matrix);

	return 1;
}

LUA_FUNCTION(WorldToViewMatrix) {
	VMatrix mat = interfaces::engineClient->WorldToViewMatrix();
	LUA->PushUserType_Value(mat, Type::Matrix);

	return 1;
}

LUA_FUNCTION(IsOccluded) {
	LUA->CheckType(1, Type::Vector);
	LUA->CheckType(2, Type::Vector);

	LUA->PushBool(interfaces::engineClient->IsOccluded(LUA->GetVector(1), LUA->GetVector(2)));

	return 1;
}

LUA_FUNCTION(ExecuteClientCmd) {
	LUA->CheckString(1);

	interfaces::engineClient->ExecuteClientCmd(LUA->GetString(1));

	return 0;
}

LUA_FUNCTION(ClientCmdUnrestricted) {
	LUA->CheckString(1);

	interfaces::engineClient->ClientCmd_Unrestricted(LUA->GetString(1));

	return 0;
}

LUA_FUNCTION(SetRestrictServerCommands) {
	LUA->CheckType(1, Type::Bool);

	interfaces::engineClient->SetRestrictServerCommands(LUA->GetBool(1));

	return 0;
}

LUA_FUNCTION(SetRestrictClientCommands) {
	LUA->CheckType(1, Type::Bool);

	interfaces::engineClient->SetRestrictClientCommands(LUA->GetBool(1));

	return 0;
}

LUA_FUNCTION(RawClientCmdUnrestricted) {
	LUA->CheckString(1);

	interfaces::engineClient->GMOD_RawClientCmd_Unrestricted(LUA->GetString(1));

	return 0;
}

// ClientState
LUA_FUNCTION_GETTER(GetPreviousTick, Number, interfaces::clientState->oldtickcount);
LUA_FUNCTION_GETSET(LastOutgoingCommand, Number, interfaces::clientState->lastoutgoingcommand);
LUA_FUNCTION_GETSET(ChokedCommands, Number, interfaces::clientState->chokedcommands);
LUA_FUNCTION_GETSET(LastCommandAck, Number, interfaces::clientState->last_command_ack);

LUA_FUNCTION(GetInterpolationTime) {
	using GetInterpolationTimeFn = float(__fastcall*)();
	static GetInterpolationTimeFn GetInterpolationTime = (GetInterpolationTimeFn)findPattern("engine.dll", "48 83 EC ?? 48 8B 0D ?? ?? ?? ?? 48 85 C9 75 ?? 48 8B 0D");

	LUA->PushNumber(GetInterpolationTime());

	return 1;
}

// GlobalVars 
LUA_FUNCTION_GETSET(RealTime, Number, interfaces::globalVars->realtime);
LUA_FUNCTION_GETSET(FrameCount, Number, interfaces::globalVars->framecount);
LUA_FUNCTION_GETSET(AbsFrameTime, Number, interfaces::globalVars->absoluteframetime);
LUA_FUNCTION_GETSET(CurTime, Number, interfaces::globalVars->curtime);
LUA_FUNCTION_GETSET(FrameTime, Number, interfaces::globalVars->frametime);
LUA_FUNCTION_GETSET(InterpolationAmount, Number, interfaces::globalVars->interpolation_amount);

// ConVar 
LUA_FUNCTION(ConVarSetValue) {
	LUA->CheckString(1);
	LUA->CheckNumber(2);

	auto* var = interfaces::cvar->FindVar(LUA->GetString(1));
	if (!var) 
	{
		LUA->PushBool(false);
		return 1;
	}

	var->SetValue(LUA->GetNumber(2));

	LUA->PushBool(true);
	return 1;
}


LUA_FUNCTION(ConVarSetFlags) {
	LUA->CheckString(1);
	LUA->CheckNumber(2);

	auto* var = interfaces::cvar->FindVar(LUA->GetString(1));
	if (!var) 
	{
		LUA->PushBool(false);
		return 1;
	}

	var->SetFlags(LUA->GetNumber(2));

	LUA->PushBool(true);
	return 1;
}

LUA_FUNCTION(SpoofConVar) {
	LUA->CheckString(1);

	auto conVarName = LUA->GetString(1);

	for (auto& spoofedConVar : spoofedConVars) {
		if (strcmp(spoofedConVar->m_szOriginalName, conVarName) == 0)
		{
			LUA->PushBool(true);
			return 1;
		}
	}

	auto* var = interfaces::cvar->FindVar(conVarName);
	if (!var) {
		LUA->PushBool(false);
		return 1;
	}

	auto& spoofedConVar = spoofedConVars.emplace_back(std::make_unique<SpoofedConVar>(var));
	spoofedConVar->m_pOriginalCVar->DisableCallback();

	LUA->PushBool(true);
	return 1;
}

LUA_FUNCTION(SpoofedConVarSetNumber) {
	LUA->CheckString(1);
	LUA->CheckNumber(2);

	auto conVarName = LUA->GetString(1);
	const auto& it = std::find_if(spoofedConVars.begin(), spoofedConVars.end(), [=](const auto& spoofedConVar) {
		return strcmp(spoofedConVar->m_szOriginalName, conVarName) == 0;
		});

	if (it == spoofedConVars.end()) {
		LUA->PushBool(false);
		return 1;
	}

	auto& spoofedConVar = *it;

	spoofedConVar->m_pOriginalCVar->SetValue(LUA->CheckNumber(2));

	LUA->PushBool(true);
	return 1;
}

// CUserCmd 
LUA_FUNCTION(SetCommandNumber) {
	LUA->CheckType(1, Type::UserCmd);
	LUA->CheckNumber(2);

	CUserCmd* cmd = LUA->GetUserType<CUserCmd>(1, Type::UserCmd);
	cmd->command_number = LUA->GetNumber(2);

	return 0;
}

LUA_FUNCTION(SetCommandTick) {
	LUA->CheckType(1, Type::UserCmd);
	LUA->CheckNumber(2);

	CUserCmd* cmd = LUA->GetUserType<CUserCmd>(1, Type::UserCmd);
	cmd->tick_count = LUA->GetNumber(2);

	return 0;
}

LUA_FUNCTION(GetRandomSeed) {
	LUA->CheckType(1, Type::UserCmd);

	CUserCmd* cmd = LUA->GetUserType<CUserCmd>(1, Type::UserCmd);
	LUA->PushNumber(cmd->random_seed);

	return 1;
}

LUA_FUNCTION(SetRandomSeed) {
	LUA->CheckType(1, Type::UserCmd);
	LUA->CheckNumber(2);

	CUserCmd* cmd = LUA->GetUserType<CUserCmd>(1, Type::UserCmd);
	cmd->random_seed = LUA->GetNumber(2);

	return 0;
}

LUA_FUNCTION(GetTyping) {
	LUA->CheckType(1, Type::UserCmd);

	CUserCmd* cmd = LUA->GetUserType<CUserCmd>(1, Type::UserCmd);
	LUA->PushBool(cmd->istyping);

	return 1;
}

LUA_FUNCTION(SetTyping) {
	LUA->CheckType(1, Type::UserCmd);
	LUA->CheckType(2, Type::Bool);

	CUserCmd* cmd = LUA->GetUserType<CUserCmd>(1, Type::UserCmd);
	cmd->istyping = LUA->GetBool(2);

	return 0;
}

LUA_FUNCTION(GetContextMenu) {
	LUA->CheckType(1, Type::UserCmd);

	CUserCmd* cmd = LUA->GetUserType<CUserCmd>(1, Type::UserCmd);
	LUA->PushBool(cmd->context_menu);

	return 1;
}

LUA_FUNCTION(SetContextMenu) {
	LUA->CheckType(1, Type::UserCmd);
	LUA->CheckType(2, Type::Bool);

	CUserCmd* cmd = LUA->GetUserType<CUserCmd>(1, Type::UserCmd);
	cmd->context_menu = LUA->GetBool(2);

	return 0;
}

LUA_FUNCTION(GetContextVector) {
	LUA->CheckType(1, Type::UserCmd);

	CUserCmd* cmd = LUA->GetUserType<CUserCmd>(1, Type::UserCmd);
	LUA->PushVector(cmd->context_normal);

	return 1;
}

LUA_FUNCTION(SetContextVector) {
	LUA->CheckType(1, Type::UserCmd);
	LUA->CheckType(2, Type::Vector);

	CUserCmd* cmd = LUA->GetUserType<CUserCmd>(1, Type::UserCmd);
	cmd->context_normal = LUA->GetVector(2);

	return 0;
}

LUA_FUNCTION(FindCommandNumber) {
	LUA->CheckType(1, Type::UserCmd);
	LUA->CheckNumber(2);

	CUserCmd* cmd = LUA->GetUserType<CUserCmd>(1, Type::UserCmd);
	int seed = LUA->GetNumber(2);
	int cmdNum = cmd->command_number;
	uint32_t uSeed;

	while (true)
	{
		{
			Chocobo1::MD5 md5;
			md5.addData(&cmdNum, sizeof(cmdNum));
			md5.finalize();

			uSeed = *reinterpret_cast<uint32_t*>(md5.toArray().data() + 6);
		}

		if ((uSeed & 255) == (seed & 255))
			break;

		cmdNum++;
	}

	LUA->PushNumber(cmdNum);
	LUA->PushNumber(uSeed);

	return 2;
}

LUA_FUNCTION(MD5PseudoRandom) {
	LUA->CheckNumber(1);

	int seed = LUA->GetNumber(1);

	uint32_t checksum;
	{
		Chocobo1::MD5 md5;
		md5.addData(&seed, sizeof(seed));
		md5.finalize();

		checksum = *reinterpret_cast<uint32_t*>(md5.toArray().data() + 6);
	}
	LUA->PushNumber(checksum);

	return 1;
}

LUA_FUNCTION(PredictSpread) {
	LUA->CheckType(1, Type::UserCmd);
	LUA->CheckType(2, Type::Vector);

	CUserCmd* cmd = LUA->GetUserType<CUserCmd>(1, Type::UserCmd);
	Vector spread = LUA->GetVector(2);

	uint32_t seed;
	{
		Chocobo1::MD5 md5;
		md5.addData(&cmd->command_number, sizeof(cmd->command_number));
		md5.finalize();

		seed = *reinterpret_cast<uint32_t*>(md5.toArray().data() + 6);
	}

	vstdlib::RandomSeed(seed & 0xFF);

	static ConVar* ai_shot_bias_min = interfaces::cvar->FindVar("ai_shot_bias_min");
	static ConVar* ai_shot_bias_max = interfaces::cvar->FindVar("ai_shot_bias_max");

	float x, y, z;

	float bias = 1.0f;

	if (bias > 1.0)
		bias = 1.0;
	else if (bias < 0.0)
		bias = 0.0;

	float shotBiasMin = ai_shot_bias_min->GetFloat();
	float shotBiasMax = ai_shot_bias_max->GetFloat();

	float shotBias = ((shotBiasMax - shotBiasMin) * bias) + shotBiasMin;

	float flatness = (fabsf(shotBias) * 0.5);

	do
	{
		float r1 = vstdlib::RandomFloat(-1, 1);
		float r2 = vstdlib::RandomFloat(-1, 1);
		float r3 = vstdlib::RandomFloat(-1, 1);
		float r4 = vstdlib::RandomFloat(-1, 1);

		x = r2 * flatness + r1 * (1 - flatness);
		y = r4 * flatness + r3 * (1 - flatness);
		if (shotBias < 0)
		{
			x = (x >= 0) ? 1.0 - x : -1.0 - x;
			y = (y >= 0) ? 1.0 - y : -1.0 - y;
		}
		z = x*x + y*y;
	} while (z > 1);

	Vector spreadDir = Vector(1.f, -spread.x * x, spread.y * y);

	LUA->PushVector(spreadDir);

	return 1;
}

// Prediction 
LUA_FUNCTION(StartPrediction) {
	LUA->CheckType(1, Type::UserCmd);

	g_prediction.Start(LUA->GetUserType<CUserCmd>(1, Type::UserCmd));

	return 0;
}

LUA_FUNCTION(FinishPrediction) {
	g_prediction.Finish();

	return 0;
}

LUA_FUNCTION(RunPrediction) {
	using CL_RunPredictionFn = void(__fastcall*)(int);
	static CL_RunPredictionFn CL_RunPrediction = (CL_RunPredictionFn)findPattern("engine.dll", "48 83 EC ?? 83 3D ?? ?? ?? ?? ?? 75 ?? 83 3D");

	CL_RunPrediction(2);

	return 0;
}

// Simulation 
LUA_FUNCTION(StartSimulation) {
	GET_PLAYER(1);

	g_simulation.Start(Ply);

	return 0;
}

LUA_FUNCTION(SimulateTick) {
	g_simulation.SimulateTick();

	return 0;
}

LUA_FUNCTION(GetSimulationData) {
	LUA->PushUserType(&g_simulation.GetMoveData(), Type::MoveData);

	return 1;
}

LUA_FUNCTION(FinishSimulation) {
	g_simulation.Finish();

	return 0;
}

// Globals 
LUA_FUNCTION_GETTER(GetBSendPacket, Bool, globals::bSendPacket);
LUA_FUNCTION_BSETTER(SetBSendPacket, globals::bSendPacket);

LUA_FUNCTION_BSETTER(SetInterpolation, globals::shouldInterpolate);
LUA_FUNCTION_BSETTER(SetSequenceInterpolation, globals::shouldInterpolateSequences);
LUA_FUNCTION_BSETTER(EnableAnimFix, globals::shouldFixAnimations);

LUA_FUNCTION(LoopMove) {
	globals::bLoopMove = true;

	return 0;
}

LUA_FUNCTION(SetCustomDisconnect) {
	if (LUA->IsType(1, Type::String)) {
		globals::bCustomDisconnect = true;
		globals::customDisconnect = LUA->GetString(1);
	}
	else {
		globals::bCustomDisconnect = false;
	}

	return 0;
}

LUA_FUNCTION(DrawModelExecute) {
	detours::callDMEViaContext();

	return 0;
}

// Win API
LUA_FUNCTION(GetClipboardText) {
	if (!OpenClipboard(nullptr)) {
		LUA->PushBool(false);
		return 1;
	}

	HANDLE clipboardHandle = GetClipboardData(CF_TEXT);
	if (clipboardHandle == nullptr) {
		CloseClipboard();
		LUA->PushBool(false);
		return 1;
	}

	char* clipboardText = static_cast<char*>(GlobalLock(clipboardHandle));
	if (clipboardText == nullptr) {
		CloseClipboard();
		LUA->PushBool(false);
		return 1;
	}

	LUA->PushString(clipboardText);

	GlobalUnlock(clipboardHandle);
	CloseClipboard();

	return 1;
}

LUA_FUNCTION(ExcludeFromCapture) {
	LUA->CheckType(1, Type::Bool);

	HWND hWnd = FindWindowA("Valve001", nullptr);

	if (LUA->GetBool(1)) {
		SetWindowDisplayAffinity(hWnd, WDA_EXCLUDEFROMCAPTURE);
	}
	else
	{
		SetWindowDisplayAffinity(hWnd, !WDA_EXCLUDEFROMCAPTURE);
	}

	return 0;
}

// File 
LUA_FUNCTION(Read) {
	LUA->CheckString(1);
	
	const char* path = LUA->GetString();
	std::ifstream file;
	file.open(path, std::ios_base::binary | std::ios_base::ate);
	if (!file.good()) {
		LUA->PushBool(false);
		return 1;
	}

	auto fileSize = file.tellg();

	// Prevent heap corruption
	if (fileSize == 0) {
		LUA->PushBool(true);
		LUA->PushString("");
		return 2;
	}

	char* buffer = new char[fileSize] {0};
	file.seekg(std::ios::beg);
	file.read(buffer, fileSize);
	file.close();

	LUA->PushBool(true);
	LUA->PushString(buffer, fileSize);

	delete[] buffer;

	return 2;
}

LUA_FUNCTION(Write) {
	LUA->CheckString(1);
	LUA->CheckString(2);

	const char* path = LUA->GetString(1);
	std::ofstream file;
	file.open(path, std::ios_base::binary);
	if (!file.good()) {
		LUA->PushBool(false);
		return 1;
	}

	unsigned int dataSize = 0;
	const char* data = LUA->GetString(2, &dataSize);
	file.write(data, dataSize);
	file.close();

	LUA->PushBool(true);

	return 1;
}


// NetChannel
LUA_FUNCTION(NetSetConVar) {
	LUA->CheckString(1);
	LUA->CheckString(2);

	const char* conVar = LUA->GetString(1);
	const char* value = LUA->GetString(2);
	bool reliable = LUA->IsType(3, Type::Bool) ? LUA->GetBool(3) : true;

	INetChannel* netChan = interfaces::engineClient->GetNetChannel();

	uint8_t msgBuf[1024];
	NetMessageWriteable netMsg(NetMessage::net_SetConVar, msgBuf, sizeof(msgBuf));
	netMsg.SetReliable(reliable);
	netMsg.write.WriteUInt(static_cast<uint32_t>(NetMessage::net_SetConVar), NET_MESSAGE_BITS);
	netMsg.write.WriteByte(1);
	netMsg.write.WriteString(conVar);
	netMsg.write.WriteString(value);

	netChan->SendNetMsg(netMsg, reliable);

	return 0;
}

LUA_FUNCTION(SendAchievement) {
	LUA->CheckNumber(1);

	static unsigned char payload_bytes[] = { 
			0x00, 0x41, 0x63, 0x68, 0x69, 0x65, 0x76, 0x65, 0x6d, 0x65, 
			0x6e, 0x74, 0x45, 0x61, 0x72, 0x6e, 0x65, 0x64, 0x00, 0x02, 
			0x61, 0x63, 0x68, 0x69, 0x65, 0x76, 0x65, 0x6d, 0x65, 0x6e, 
			0x74, 0x49, 0x44, 0x00, 0x44, 0x33, 0x22, 0x11, 0x08, 0x08 };

	*(int*)(payload_bytes + 34) = LUA->GetNumber(1);

	INetChannel* netChan = interfaces::engineClient->GetNetChannel();

	uint8_t msgBuf[1024];
	NetMessageWriteable netMsg(NetMessage::clc_CmdKeyValues, msgBuf, sizeof(msgBuf));
	netMsg.write.WriteUInt(static_cast<uint32_t>(NetMessage::clc_CmdKeyValues), NET_MESSAGE_BITS);
	netMsg.write.WriteLong(sizeof(payload_bytes));
	netMsg.write.WriteBits(payload_bytes, sizeof(payload_bytes) * 8);

	netChan->SendNetMsg(netMsg);

	return 0;
}

LUA_FUNCTION(NetDisconnect) {
	LUA->CheckString(1);

	const char* str = LUA->GetString(1);

	INetChannel* netChan = interfaces::engineClient->GetNetChannel();

	uint8_t msgBuf[1024];
	NetMessageWriteable netMsg(NetMessage::net_Disconnect, msgBuf, sizeof(msgBuf));
	netMsg.write.WriteUInt(static_cast<uint32_t>(NetMessage::net_Disconnect), NET_MESSAGE_BITS);
	netMsg.write.WriteString(str);

	netChan->SendNetMsg(netMsg);

	return 0;
}

LUA_FUNCTION(GetLatency) {
	LUA->CheckNumber(1);

	INetChannel* netChan = interfaces::engineClient->GetNetChannel();

	LUA->PushNumber(netChan->GetLatency(LUA->GetNumber(1)));

	return 1;
}

LUA_FUNCTION(GetAvgLatency) {
	LUA->CheckNumber(1);

	INetChannel* netChan = interfaces::engineClient->GetNetChannel();

	LUA->PushNumber(netChan->GetAvgLatency(LUA->GetNumber(1)));

	return 1;
}

LUA_FUNCTION(GetAvgLoss) {
	LUA->CheckNumber(1);

	LUA->PushNumber(interfaces::engineClient->GetNetChannel()->GetAvgLoss(LUA->GetNumber(1)));

	return 1;
}

LUA_FUNCTION(GetAvgChoke) {
	LUA->CheckNumber(1);

	LUA->PushNumber(interfaces::engineClient->GetNetChannel()->GetAvgChoke(LUA->GetNumber(1)));

	return 1;
}

LUA_FUNCTION(GetAvgData) {
	LUA->CheckNumber(1);

	LUA->PushNumber(interfaces::engineClient->GetNetChannel()->GetAvgData(LUA->GetNumber(1)));

	return 1;
}

LUA_FUNCTION(GetAvgPackets) {
	LUA->CheckNumber(1);

	LUA->PushNumber(interfaces::engineClient->GetNetChannel()->GetAvgPackets(LUA->GetNumber(1)));

	return 1;
}

LUA_FUNCTION(GetTotalData) {
	LUA->CheckNumber(1);

	LUA->PushNumber(interfaces::engineClient->GetNetChannel()->GetTotalData(LUA->GetNumber(1)));

	return 1;
} 

LUA_FUNCTION(GetSequenceNr) {
	LUA->CheckNumber(1);

	LUA->PushNumber(interfaces::engineClient->GetNetChannel()->GetSequenceNr(LUA->GetNumber(1)));

	return 1;
}

LUA_FUNCTION(IsValidPacket) {
	LUA->CheckNumber(1);
	LUA->CheckNumber(2);

	LUA->PushBool(interfaces::engineClient->GetNetChannel()->IsValidPacket(LUA->GetNumber(1), LUA->GetNumber(2)));

	return 1;
}

LUA_FUNCTION(GetPacketTime) {
	LUA->CheckNumber(1);
	LUA->CheckNumber(2);

	LUA->PushNumber(interfaces::engineClient->GetNetChannel()->GetPacketTime(LUA->GetNumber(1), LUA->GetNumber(2)));

	return 1;
}

LUA_FUNCTION(GetPacketBytes) {
	LUA->CheckNumber(1);
	LUA->CheckNumber(2);
	LUA->CheckNumber(3);

	LUA->PushNumber(interfaces::engineClient->GetNetChannel()->GetPacketBytes(LUA->GetNumber(1), LUA->GetNumber(2), LUA->GetNumber(3)));

	return 1;
}

LUA_FUNCTION(GetStreamProgress) {
	LUA->CheckNumber(1);

	int total, received;
	interfaces::engineClient->GetNetChannel()->GetStreamProgress(LUA->GetNumber(1), &total, &received);

	LUA->PushNumber(total);
	LUA->PushNumber(received);

	return 2;
}

LUA_FUNCTION(GetCommandInterpolationAmount) {
	LUA->CheckNumber(1);
	LUA->CheckNumber(2);

	LUA->PushNumber(interfaces::engineClient->GetNetChannel()->GetCommandInterpolationAmount(LUA->GetNumber(1), LUA->GetNumber(2)));

	return 1;
}

LUA_FUNCTION(GetPacketResponseLatency) {
	LUA->CheckNumber(1);
	LUA->CheckNumber(2);

	int latency, choke;
	interfaces::engineClient->GetNetChannel()->GetPacketResponseLatency(LUA->GetNumber(1), LUA->GetNumber(2), &latency, &choke);

	LUA->PushNumber(latency);
	LUA->PushNumber(choke);

	return 2;
}

LUA_FUNCTION(GetRemoteFramerate) {
	float frameTime, stdDeviation;
	interfaces::engineClient->GetNetChannel()->GetRemoteFramerate(&frameTime, &stdDeviation);

	LUA->PushNumber(frameTime);
	LUA->PushNumber(stdDeviation);

	return 2;
}

LUA_FUNCTION(SetDataRate) {
	LUA->CheckNumber(1);

	interfaces::engineClient->GetNetChannel()->SetDataRate(LUA->GetNumber(1));

	return 0;
}

LUA_FUNCTION(SetTimeout) {
	LUA->CheckNumber(1);

	interfaces::engineClient->GetNetChannel()->SetTimeout(LUA->GetNumber(1));

	return 0;
}

LUA_FUNCTION(SetChallengeNr) {
	LUA->CheckNumber(1);

	interfaces::engineClient->GetNetChannel()->SetChallengeNr(LUA->GetNumber(1));

	return 0;
}

LUA_FUNCTION(NetShutdown) {
	LUA->CheckString(1);

	interfaces::engineClient->GetNetChannel()->Shutdown(LUA->GetString(1));

	return 0;
}

LUA_FUNCTION(SendFile) {
	LUA->CheckString(1);
	LUA->CheckNumber(2);

	const char* str = LUA->GetString(1);
	INetChannel* netChan = interfaces::engineClient->GetNetChannel();

	LUA->PushBool(netChan->SendFile(str, LUA->GetNumber(2)));

	return 1;
}

LUA_FUNCTION(Transmit) {
	INetChannel* netChan = interfaces::engineClient->GetNetChannel();

	LUA->PushBool(netChan->Transmit(LUA->IsType(1, Type::Bool) ? LUA->GetBool(1) : false));

	return 1;
}

LUA_FUNCTION(SetFileTransmissionMode) {
	LUA->CheckType(1, Type::Bool);

	interfaces::engineClient->GetNetChannel()->SetFileTransmissionMode(LUA->GetBool(1));

	return 0;
}

LUA_FUNCTION(SetCompressionMode) {
	LUA->CheckType(1, Type::Bool);

	interfaces::engineClient->GetNetChannel()->SetCompressionMode(LUA->GetBool(1));

	return 0;
}

LUA_FUNCTION(RequestFile) {
	LUA->CheckNumber(1);
	LUA->CheckNumber(2);

	INetChannel* netChan = interfaces::engineClient->GetNetChannel();

	LUA->PushNumber(netChan->RequestFile(LUA->GetNumber(1), LUA->GetNumber(2)));

	return 1;
} 

LUA_FUNCTION(SetMaxBufferSize) {
	LUA->CheckType(1, Type::Bool);
	LUA->CheckNumber(2);

	interfaces::engineClient->GetNetChannel()->SetMaxBufferSize(LUA->GetBool(1), LUA->GetNumber(2), LUA->IsType(3, Type::Bool) ? LUA->GetBool(3) : false);

	return 0;
}

LUA_FUNCTION(GetNumBitsWritten) {
	LUA->CheckType(1, Type::Bool);

	LUA->PushNumber(interfaces::engineClient->GetNetChannel()->GetNumBitsWritten(LUA->GetBool(1)));

	return 1;
}

LUA_FUNCTION(SetNetInterpolationAmount) {
	LUA->CheckNumber(1);

	interfaces::engineClient->GetNetChannel()->SetInterpolationAmount(LUA->GetNumber(1));

	return 0;
}

LUA_FUNCTION(SetRemoteFramerate) {
	LUA->CheckNumber(1);
	LUA->CheckNumber(2);

	interfaces::engineClient->GetNetChannel()->SetRemoteFramerate(LUA->GetNumber(1), LUA->GetNumber(2));

	return 0;
}

LUA_FUNCTION(SetMaxRoutablePayloadSize) {
	LUA->CheckNumber(1);

	interfaces::engineClient->GetNetChannel()->SetMaxRoutablePayloadSize(LUA->GetNumber(1));

	return 0;
}

LUA_FUNCTION_GETTER(GetNetName, String, interfaces::engineClient->GetNetChannel()->GetName())
LUA_FUNCTION_GETTER(GetNetAddress, String, interfaces::engineClient->GetNetChannel()->GetAddress())
LUA_FUNCTION_GETTER(GetNetTime, Number, interfaces::engineClient->GetNetChannel()->GetTime())
LUA_FUNCTION_GETTER(GetTimeConnected, Number, interfaces::engineClient->GetNetChannel()->GetTimeConnected())
LUA_FUNCTION_GETTER(GetBufferSize, Number, interfaces::engineClient->GetNetChannel()->GetBufferSize())
LUA_FUNCTION_GETTER(GetDataRate, Number, interfaces::engineClient->GetNetChannel()->GetDataRate())
LUA_FUNCTION_GETTER(IsLoopback, Bool, interfaces::engineClient->GetNetChannel()->IsLoopback())
LUA_FUNCTION_GETTER(IsTimingOut, Bool, interfaces::engineClient->GetNetChannel()->IsTimingOut())
LUA_FUNCTION_GETTER(IsPlayback, Bool, interfaces::engineClient->GetNetChannel()->IsPlayback())
LUA_FUNCTION_GETTER(GetTimeSinceLastReceived, Number, interfaces::engineClient->GetNetChannel()->GetTimeSinceLastReceived())
LUA_FUNCTION_GETTER(GetTimeoutSeconds, Number, interfaces::engineClient->GetNetChannel()->GetTimeoutSeconds())
LUA_FUNCTION_GETTER(GetChallengeNr, Number, interfaces::engineClient->GetNetChannel()->GetChallengeNr())
LUA_FUNCTION_GETTER(CanPacket, Bool, interfaces::engineClient->GetNetChannel()->CanPacket())
LUA_FUNCTION_GETTER(IsOverflowed, Bool, interfaces::engineClient->GetNetChannel()->IsOverflowed())
LUA_FUNCTION_GETTER(IsTimedOut, Bool, interfaces::engineClient->GetNetChannel()->IsTimedOut())
LUA_FUNCTION_GETTER(HasPendingReliableData, Bool, interfaces::engineClient->GetNetChannel()->HasPendingReliableData())
LUA_FUNCTION_GETTER(IsNull, Bool, interfaces::engineClient->GetNetChannel()->IsNull())
LUA_FUNCTION_GETTER(GetMaxRoutablePayloadSize, Number, interfaces::engineClient->GetNetChannel()->GetMaxRoutablePayloadSize())

LUA_FUNCTION_GETSET(OutSequenceNr, Number, interfaces::engineClient->GetNetChannel()->m_nOutSequenceNr);
LUA_FUNCTION_GETSET(InSequenceNr, Number, interfaces::engineClient->GetNetChannel()->m_nInSequenceNr);
LUA_FUNCTION_GETSET(OutSequenceNrAck, Number, interfaces::engineClient->GetNetChannel()->m_nOutSequenceNrAck);
LUA_FUNCTION_GETSET(OutReliableState, Number, interfaces::engineClient->GetNetChannel()->m_nOutReliableState);
LUA_FUNCTION_GETSET(InReliableState, Number, interfaces::engineClient->GetNetChannel()->m_nInReliableState);
LUA_FUNCTION_GETSET(ChokedPackets, Number, interfaces::engineClient->GetNetChannel()->m_nChokedPackets);
LUA_FUNCTION_GETSET(PacketDrop, Number, interfaces::engineClient->GetNetChannel()->m_PacketDrop);

// Entity 
LUA_FUNCTION(GetNetworkedVarInt) {
	GET_ENTITY(1);
	LUA->CheckString(2);
	LUA->CheckString(3);

	std::string key(LUA->GetString(2) + std::string("->") + LUA->GetString(3));
	const auto& it = netvars::netvars.find(key);

	if (it == netvars::netvars.end()) {
		LUA->PushNil();
		return 1;
	}

	LUA->PushNumber(*reinterpret_cast<int32_t*>(reinterpret_cast<std::uintptr_t>(Ent) + it->second));

	return 1;
}

LUA_FUNCTION(GetNetworkedVarFloat) {
	GET_ENTITY(1);
	LUA->CheckString(2);
	LUA->CheckString(3);

	std::string key(LUA->GetString(2) + std::string("->") + LUA->GetString(3));
	const auto& it = netvars::netvars.find(key);

	if (it == netvars::netvars.end()) {
		LUA->PushNil();
		return 1;
	}

	LUA->PushNumber(*reinterpret_cast<float*>(reinterpret_cast<std::uintptr_t>(Ent) + it->second));

	return 1;
}

LUA_FUNCTION(GetNetworkedVarBool) {
	GET_ENTITY(1);
	LUA->CheckString(2);
	LUA->CheckString(3);

	std::string key(LUA->GetString(2) + std::string("->") + LUA->GetString(3));
	const auto& it = netvars::netvars.find(key);

	if (it == netvars::netvars.end()) {
		LUA->PushNil();
		return 1;
	}

	LUA->PushBool(*reinterpret_cast<bool*>(reinterpret_cast<std::uintptr_t>(Ent) + it->second));

	return 1;
}

LUA_FUNCTION(GetNetworkedVarString) {
	GET_ENTITY(1);
	LUA->CheckString(2);
	LUA->CheckString(3);

	std::string key(LUA->GetString(2) + std::string("->") + LUA->GetString(3));
	const auto& it = netvars::netvars.find(key);

	if (it == netvars::netvars.end()) {
		LUA->PushNil();
		return 1;
	}

	LUA->PushString(*reinterpret_cast<const char**>(reinterpret_cast<std::uintptr_t>(Ent) + it->second));

	return 1;
}

LUA_FUNCTION(GetNetworkedVarVector) {
	GET_ENTITY(1);
	LUA->CheckString(2);
	LUA->CheckString(3);

	std::string key(LUA->GetString(2) + std::string("->") + LUA->GetString(3));
	const auto& it = netvars::netvars.find(key);

	if (it == netvars::netvars.end()) {
		LUA->PushNil();
		return 1;
	}

	LUA->PushVector(*reinterpret_cast<Vector*>(reinterpret_cast<std::uintptr_t>(Ent) + it->second));

	return 1;
}

LUA_FUNCTION(GetNetworkedVarAngle) {
	GET_ENTITY(1);
	LUA->CheckString(2);
	LUA->CheckString(3);

	std::string key(LUA->GetString(2) + std::string("->") + LUA->GetString(3));
	const auto& it = netvars::netvars.find(key);

	if (it == netvars::netvars.end()) {
		LUA->PushNil();
		return 1;
	}

	LUA->PushAngle(*reinterpret_cast<Angle*>(reinterpret_cast<std::uintptr_t>(Ent) + it->second));

	return 1;
}

LUA_FUNCTION(GetNetworkedVarEntity) {
	GET_ENTITY(1);
	LUA->CheckString(2);
	LUA->CheckString(3);

	std::string key(LUA->GetString(2) + std::string("->") + LUA->GetString(3));
	const auto& it = netvars::netvars.find(key);

	if (it == netvars::netvars.end()) {
		LUA->PushNil();
		return 1;
	}

	CBaseEntity* ent = interfaces::entityList->GetClientEntityFromHandle(*reinterpret_cast<CBaseHandle*>(reinterpret_cast<std::uintptr_t>(Ent) + it->second));

	if (ent) {
		ent->PushEntity();
	}
	else {
		LUA->PushSpecial(SPECIAL_GLOB);
		LUA->GetField(-1, "NULL");
		LUA->Remove(-2);
	}

	return 1;
}

LUA_FUNCTION(GetSimulationTime) {
	GET_ENTITY(1);

	LUA->PushNumber(Ent->m_flSimulationTime());

	return 1;
}

LUA_FUNCTION(InvalidateBoneCache) {
	GET_ENTITY(1);

	CBaseAnimating *Anim = Ent->GetBaseAnimating();

	if (Anim) {
		using Studio_GetBoneCacheFn = void*(__fastcall*)(void*);
		static Studio_GetBoneCacheFn Studio_GetBoneCache = (Studio_GetBoneCacheFn)findPattern("client.dll", "48 89 5C 24 ?? 57 48 83 EC ?? 48 8B F9 FF 15 ?? ?? ?? ?? 8B 15 ?? ?? ?? ?? 48 8B D8 3B C2 74 ?? 45 33 C0 48 8D 0D ?? ?? ?? ?? 8B D3 FF 15 ?? ?? ?? ?? 84 C0 75 ?? F3 90 45 33 C0 48 8D 0D ?? ?? ?? ?? 8B D3 FF 15 ?? ?? ?? ?? EB ?? 90 8B 05 ?? ?? ?? ?? FF C0 89 05 ?? ?? ?? ?? 48 8B D7 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 15");

		void *pcache = Studio_GetBoneCache(Anim->m_hitboxBoneCacheHandle());

		if (pcache)
			*(double*)pcache = -1.0;
	}

	return 0;
}

LUA_FUNCTION(GetTargetLowerBodyYaw) { 
	GET_PLAYER(1);

	CBasePlayerAnimState* animState = Ply->GetAnimState();

	LUA->PushNumber(animState->m_flGoalFeetYaw);

	return 1;
}

LUA_FUNCTION(GetCurrentLowerBodyYaw) {
	GET_PLAYER(1);

	CBasePlayerAnimState* animState = Ply->GetAnimState();

	LUA->PushNumber(animState->m_flCurrentFeetYaw);

	return 1;
}

LUA_FUNCTION(SetTargetLowerBodyYaw) {
	GET_PLAYER(1);
	LUA->CheckNumber(2);

	CBasePlayerAnimState* animState = Ply->GetAnimState();

	animState->m_flGoalFeetYaw = LUA->GetNumber(2);

	return 0;
}

LUA_FUNCTION(SetCurrentLowerBodyYaw) {
	GET_PLAYER(1);
	LUA->CheckNumber(2);

	CBasePlayerAnimState* animState = Ply->GetAnimState();

	animState->m_flCurrentFeetYaw = LUA->GetNumber(2);

	return 0;
}

LUA_FUNCTION(UpdateClientAnimation) {
	GET_ENTITY(1);

	CBaseAnimating *Anim = Ent->GetBaseAnimating();

	if (Anim)
		Anim->UpdateClientsideAnimation();

	return 0;
}

LUA_FUNCTION(UpdateAnimations) {
	GET_PLAYER(1);
	LUA->CheckNumber(2);
	LUA->CheckNumber(3);

	CBasePlayerAnimState* animState = Ply->GetAnimState();

	animState->Update( LUA->GetNumber(2), LUA->GetNumber(3) );

	return 0;
}

LUA_FUNCTION(GetTickBase) {
	GET_PLAYER(1);

	LUA->PushNumber(Ply->m_nTickBase());

	return 1;
}

LUA_FUNCTION(SetTickBase) {
	GET_PLAYER(1);
	LUA->CheckNumber(2);

	Ply->m_nTickBase() = static_cast<int>( LUA->GetNumber(2) );

	return 0;
}

// Legacy 

LUA_FUNCTION(PushSpecial) {
	LUA->CheckNumber(1);

	LUA->PushSpecial(LUA->GetNumber(1));

	return 1;
}

// Api

auto PushApiFunction = [&](const char* name, CFunc func) {
	interfaces::clientLua->PushCFunction(func);
	interfaces::clientLua->SetField(-2, name);
};

GMOD_MODULE_OPEN() {
	interfaces::clientLua = LUA;

	interfaces::init();
	netvars::init();
	detours::hook();
	detours::postInit();

	LUA->PushSpecial(SPECIAL_GLOB);
	LUA->CreateTable();
		PushApiFunction("ServerCmd", ServerCmd);
		PushApiFunction("ClientCmd", ClientCmd);
		PushApiFunction("GetLocalPlayer", GetLocalPlayer);
		PushApiFunction("GetTime", GetTime);
		PushApiFunction("GetLastTimeStamp", GetLastTimeStamp);
		PushApiFunction("GetViewAngles", GetViewAngles);
		PushApiFunction("SetViewAngles", SetViewAngles);
		PushApiFunction("IsBoxVisible", IsBoxVisible);
		PushApiFunction("IsBoxInViewCluster", IsBoxInViewCluster);
		PushApiFunction("GetGameDirectory", GetGameDirectory);
		PushApiFunction("WorldToScreenMatrix", WorldToScreenMatrix);
		PushApiFunction("WorldToViewMatrix", WorldToViewMatrix);
		PushApiFunction("IsOccluded", IsOccluded);
		PushApiFunction("ExecuteClientCmd", ExecuteClientCmd);
		PushApiFunction("ClientCmdUnrestricted", ClientCmdUnrestricted);
		PushApiFunction("SetRestrictServerCommands", SetRestrictServerCommands);
		PushApiFunction("SetRestrictClientCommands", SetRestrictClientCommands);
		PushApiFunction("RawClientCmdUnrestricted", RawClientCmdUnrestricted);

		PushApiFunction("GetPreviousTick", GetPreviousTick);
		PushApiFunction("GetLastOutgoingCommand", GetLastOutgoingCommand);
		PushApiFunction("SetLastOutgoingCommand", SetLastOutgoingCommand);
		PushApiFunction("GetChokedCommands", GetChokedCommands);
		PushApiFunction("SetChokedCommands", SetChokedCommands);
		PushApiFunction("GetLastCommandAck", GetLastCommandAck);
		PushApiFunction("SetLastCommandAck", SetLastCommandAck);
		PushApiFunction("GetInterpolationTime", GetInterpolationTime);

		PushApiFunction("GetRealTime", GetRealTime);
		PushApiFunction("SetRealTime", SetRealTime);
		PushApiFunction("GetFrameTime", GetFrameTime);
		PushApiFunction("SetFrameTime", SetFrameTime);
		PushApiFunction("GetAbsFrameTime", GetAbsFrameTime);
		PushApiFunction("SetAbsFrameTime", SetAbsFrameTime);
		PushApiFunction("GetCurTime", GetCurTime);
		PushApiFunction("SetCurTime", SetCurTime);
		PushApiFunction("GetFrameCount", GetFrameCount);
		PushApiFunction("SetFrameCount", SetFrameCount);
		PushApiFunction("GetInterpolationAmount", GetInterpolationAmount);
		PushApiFunction("SetInterpolationAmount", SetInterpolationAmount);

		PushApiFunction("ConVarSetValue", ConVarSetValue);
		PushApiFunction("ConVarSetFlags", ConVarSetFlags);
		PushApiFunction("SpoofConVar", SpoofConVar);
		PushApiFunction("SpoofedConVarSetNumber", SpoofedConVarSetNumber);

		PushApiFunction("SetCommandNumber", SetCommandNumber);
		PushApiFunction("SetCommandTick", SetCommandTick);
		PushApiFunction("GetRandomSeed", GetRandomSeed);
		PushApiFunction("SetRandomSeed", SetRandomSeed);
		PushApiFunction("GetTyping", GetTyping);
		PushApiFunction("SetTyping", SetTyping);
		PushApiFunction("GetContextMenu", GetContextMenu);
		PushApiFunction("SetContextMenu", SetContextMenu);
		PushApiFunction("GetContextVector", GetContextVector);
		PushApiFunction("SetContextVector", SetContextVector);
		PushApiFunction("FindCommandNumber", FindCommandNumber);
		PushApiFunction("MD5PseudoRandom", MD5PseudoRandom);
		PushApiFunction("PredictSpread", PredictSpread);

		PushApiFunction("StartPrediction", StartPrediction);
		PushApiFunction("FinishPrediction", FinishPrediction);
		PushApiFunction("RunPrediction", RunPrediction);

		PushApiFunction("StartSimulation", StartSimulation);
		PushApiFunction("SimulateTick", SimulateTick);
		PushApiFunction("GetSimulationData", GetSimulationData);
		PushApiFunction("FinishSimulation", FinishSimulation);

		PushApiFunction("GetBSendPacket", GetBSendPacket);
		PushApiFunction("SetBSendPacket", SetBSendPacket);
		PushApiFunction("SetInterpolation", SetInterpolation);
		PushApiFunction("SetSequenceInterpolation", SetSequenceInterpolation);
		PushApiFunction("EnableAnimFix", EnableAnimFix);
		PushApiFunction("LoopMove", LoopMove);
		PushApiFunction("SetCustomDisconnect", SetCustomDisconnect);
		PushApiFunction("DrawModelExecute", DrawModelExecute);

		PushApiFunction("GetClipboardText", GetClipboardText);
		PushApiFunction("ExcludeFromCapture", ExcludeFromCapture);

		//PushApiFunction("Read", Read);
		//PushApiFunction("Write", Write);

		PushApiFunction("NetSetConVar", NetSetConVar);	
		PushApiFunction("SendAchievement", SendAchievement);	
		PushApiFunction("NetDisconnect", NetDisconnect);
		PushApiFunction("GetLatency", GetLatency);
		PushApiFunction("GetAvgLatency", GetAvgLatency);
		PushApiFunction("GetAvgLoss", GetAvgLoss);
		PushApiFunction("GetAvgChoke", GetAvgChoke);
		PushApiFunction("GetAvgData", GetAvgData);
		PushApiFunction("GetAvgPackets", GetAvgPackets);
		PushApiFunction("GetTotalData", GetTotalData);
		PushApiFunction("GetSequenceNr", GetSequenceNr);
		PushApiFunction("IsValidPacket", IsValidPacket);
		PushApiFunction("GetPacketTime", GetPacketTime);
		PushApiFunction("GetPacketBytes", GetPacketBytes);
		PushApiFunction("GetStreamProgress", GetStreamProgress);
		PushApiFunction("GetCommandInterpolationAmount", GetCommandInterpolationAmount);
		PushApiFunction("GetPacketResponseLatency", GetPacketResponseLatency);
		PushApiFunction("GetRemoteFramerate", GetRemoteFramerate);
		PushApiFunction("SetDataRate", SetDataRate);
		PushApiFunction("SetTimeout", SetTimeout);
		PushApiFunction("SetChallengeNr", SetChallengeNr);
		PushApiFunction("NetShutdown", NetShutdown);
		PushApiFunction("SendFile", SendFile);
		PushApiFunction("Transmit", Transmit);
		PushApiFunction("SetFileTransmissionMode", SetFileTransmissionMode);
		PushApiFunction("SetCompressionMode", SetCompressionMode);
		PushApiFunction("RequestFile", RequestFile);
		PushApiFunction("SetMaxBufferSize", SetMaxBufferSize);
		PushApiFunction("GetNumBitsWritten", GetNumBitsWritten);
		PushApiFunction("SetNetInterpolationAmount", SetNetInterpolationAmount);
		PushApiFunction("SetRemoteFramerate", SetRemoteFramerate);
		PushApiFunction("SetMaxRoutablePayloadSize", SetMaxRoutablePayloadSize);
		PushApiFunction("GetNetName", GetNetName);
		PushApiFunction("GetNetAddress", GetNetAddress);
		PushApiFunction("GetNetTime", GetNetTime);
		PushApiFunction("GetTimeConnected", GetTimeConnected);
		PushApiFunction("GetBufferSize", GetBufferSize);
		PushApiFunction("GetDataRate", GetDataRate);
		PushApiFunction("IsLoopback", IsLoopback);
		PushApiFunction("IsTimingOut", IsTimingOut);
		PushApiFunction("IsPlayback", IsPlayback);
		PushApiFunction("GetTimeSinceLastReceived", GetTimeSinceLastReceived);
		PushApiFunction("GetTimeoutSeconds", GetTimeoutSeconds);
		PushApiFunction("GetChallengeNr", GetChallengeNr);
		PushApiFunction("CanPacket", CanPacket);
		PushApiFunction("IsOverflowed", IsOverflowed);
		PushApiFunction("IsTimedOut", IsTimedOut);
		PushApiFunction("HasPendingReliableData", HasPendingReliableData);
		PushApiFunction("IsNull", IsNull);
		PushApiFunction("GetMaxRoutablePayloadSize", GetMaxRoutablePayloadSize);
		PushApiFunction("GetOutSequenceNr", GetOutSequenceNr);
		PushApiFunction("SetOutSequenceNr", SetOutSequenceNr);
		PushApiFunction("GetInSequenceNr", GetInSequenceNr);
		PushApiFunction("SetInSequenceNr", SetInSequenceNr);
		PushApiFunction("GetOutSequenceNrAck", GetOutSequenceNrAck);
		PushApiFunction("SetOutSequenceNrAck", SetOutSequenceNrAck);
		PushApiFunction("GetOutReliableState", GetOutReliableState);
		PushApiFunction("SetOutReliableState", SetOutReliableState);
		PushApiFunction("GetInReliableState", GetInReliableState);
		PushApiFunction("SetInReliableState", SetInReliableState);
		PushApiFunction("GetChokedPackets", GetChokedPackets);
		PushApiFunction("SetChokedPackets", SetChokedPackets);
		PushApiFunction("GetPacketDrop", GetPacketDrop);
		PushApiFunction("SetPacketDrop", SetPacketDrop);

		PushApiFunction("GetNetworkedVarInt", GetNetworkedVarInt);
		PushApiFunction("GetNetworkedVarFloat", GetNetworkedVarFloat);
		PushApiFunction("GetNetworkedVarBool", GetNetworkedVarBool);
		PushApiFunction("GetNetworkedVarString", GetNetworkedVarString);
		PushApiFunction("GetNetworkedVarVector", GetNetworkedVarVector);
		PushApiFunction("GetNetworkedVarAngle", GetNetworkedVarAngle);
		PushApiFunction("GetNetworkedVarEntity", GetNetworkedVarEntity);
		PushApiFunction("GetSimulationTime", GetSimulationTime);
		PushApiFunction("InvalidateBoneCache", InvalidateBoneCache);
		PushApiFunction("GetTargetLowerBodyYaw", GetTargetLowerBodyYaw);
		PushApiFunction("GetCurrentLowerBodyYaw", GetCurrentLowerBodyYaw);
		PushApiFunction("SetTargetLowerBodyYaw", SetTargetLowerBodyYaw);
		PushApiFunction("SetCurrentLowerBodyYaw", SetCurrentLowerBodyYaw);
		PushApiFunction("UpdateClientAnimation", UpdateClientAnimation);
		PushApiFunction("UpdateAnimations", UpdateAnimations);
		PushApiFunction("GetTickBase", GetTickBase);
		PushApiFunction("SetTickBase", SetTickBase);

		PushApiFunction("PushSpecial", PushSpecial);
	LUA->SetField(-2, "ded");
	LUA->Pop(1);

	return 0;
}

GMOD_MODULE_CLOSE() {
	detours::unHook();
	spoofedConVars.clear();
	g_simulation.DestroyBackupData();

	return 0;
}

