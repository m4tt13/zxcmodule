#include "globals.h"
#include "luashared.h"
#include "patternscan.h"
#include "vmt.h"
#include "util.h"
#include "interfaces.h"
#include "helpers.h"

#include "hooks.h"

#include "clientmodeshared.h"
#include "effects.h"
#include "chlclient.h"
#include "cprediction.h"
#include "cusercmd.h"
#include "cinput.h"
#include "prediction.h"
#include "engineclient.h"
#include "entity.h"
#include "inetchannel.h"
#include "globalvars.h"
#include "clientstate.h"
#include "isurface.h"
#include "modelrender.h"

#include "minhook/minhook.h"

extern "C" void CreateMoveHookFuncNaked();

class CStudioHdr;

namespace detours {
	bool luaInit = false;

	auto SeqChangePattern = findPattern("client.dll", "48 85 D2 0F 84 ?? ?? ?? ?? 48 89 6C 24 ?? 48 89 74 24 ?? 48 89 7C 24");
	auto CL_MovePattern = findPattern("engine.dll", "40 55 53 48 8D AC 24 ?? ?? ?? ?? B8 ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 2B E0 0F 29 B4 24");

	ClientEffectCallback impactFunctionOriginal = nullptr;
	RecvVarProxyFn simTimeProxyFuncOriginal = nullptr;
	RecvProp* simTimeRecvProp = nullptr;
	CBasePlayer* localPlayer = nullptr;

	// Pre CreateMove
	using PreCreateMoveFn = bool(__fastcall*)(ClientModeShared* self, float flInputSampleTime, CUserCmd* cmd);
	PreCreateMoveFn preCreateMoveOriginal = nullptr;

	bool __fastcall ClientModeCreateMoveHookFunc(ClientModeShared* self, float flInputSampleTime, CUserCmd* cmd) {
		if (!cmd->command_number)
			return preCreateMoveOriginal(self, flInputSampleTime, cmd);

		if (luaInit) {
			auto* lua = interfaces::clientLua;

			if (LuaHelpers::PushHookRun(lua, "PreCreateMove" ) != 0) {
				lua->PushUserType(cmd, Type::UserCmd);

				LuaHelpers::CallHookRun(lua, 1, 0);
			}
		}

		return preCreateMoveOriginal(self, flInputSampleTime, cmd);
	}

	// Post CreateMove
	using PostCreateMoveFn = void(__fastcall*)(CHLClient* self, int sequence_number, float input_sample_frametime, bool active);
	PostCreateMoveFn postCreateMoveOriginal = nullptr;

	void __fastcall CreateMoveHookFunc(CHLClient* self, int sequence_number, float input_sample_frametime, bool active) {
		CVerifiedUserCmd* vcmd = interfaces::input->GetVerifiedCommand(sequence_number);
		CUserCmd* cmd = interfaces::input->GetCommand(sequence_number);

		postCreateMoveOriginal(self, sequence_number, input_sample_frametime, active);

		if (luaInit) {
			auto* lua = interfaces::clientLua;

			if (LuaHelpers::PushHookRun(lua, "PostCreateMove" ) != 0) {
				lua->PushUserType(cmd, Type::UserCmd);

				LuaHelpers::CallHookRun(lua, 1, 0);
			}
		}

		vcmd->cmd = *cmd;
		vcmd->crc = cmd->GetChecksum();
	}

	// Frame stage notify 
	using FrameStageFn = void(__fastcall*)(CHLClient* self, int stage);
	FrameStageFn FrameStageOriginal = nullptr;
	
	void __fastcall FrameStageNotifyHookFunc(CHLClient* self, int stage) {
		if (!luaInit && stage == ClientFrameStage_t::FRAME_START) {
			interfaces::clientLua = interfaces::luaShared->GetLuaInterface(static_cast<int>(LuaInterface::CLIENT));
			
			luaInit = true;
		}

		if (luaInit) {
			auto* lua = interfaces::clientLua;

			if (LuaHelpers::PushHookRun(lua, "PreFrameStageNotify" ) != 0) {
				lua->PushNumber(stage);

				LuaHelpers::CallHookRun(lua, 1, 0);
			}
		}

		FrameStageOriginal(self, stage);

		if (luaInit) {
			auto* lua = interfaces::clientLua;

			if (LuaHelpers::PushHookRun(lua, "PostFrameStageNotify" ) != 0) {
				lua->PushNumber(stage);

				LuaHelpers::CallHookRun(lua, 1, 0);
			}
		}
	}

	// GetUserCmd
	using GetUserCmdFn = CUserCmd*(__fastcall*)(CInput* self, int sequence_number);
	GetUserCmdFn GetUserCmdOriginal = nullptr;

	CUserCmd* __fastcall GetUserCmdHookFunc(CInput* self, int sequence_number) {
		return self->GetCommand(sequence_number);
	}

	// RunCommand 
	using RunCommandFn = void(__fastcall*)(CPrediction* self, CBasePlayer* player, CUserCmd* ucmd, IMoveHelper* moveHelper);
	RunCommandFn RunCommandOriginal = nullptr;
	
	void __fastcall RunCommandHookFunc(CPrediction* self, CBasePlayer* player, CUserCmd* ucmd, IMoveHelper* moveHelper) {
		if (luaInit) {
			auto* lua = interfaces::clientLua;

			if (LuaHelpers::PushHookRun(lua, "PreRunCommand" ) != 0) {
				player->PushEntity();
				lua->PushUserType(ucmd, Type::UserCmd);

				LuaHelpers::CallHookRun(lua, 2, 0);
			}
		}

		RunCommandOriginal(self, player, ucmd, moveHelper);

		if (luaInit) {
			auto* lua = interfaces::clientLua;

			if (LuaHelpers::PushHookRun(lua, "PostRunCommand" ) != 0) {
				player->PushEntity();
				lua->PushUserType(ucmd, Type::UserCmd);

				LuaHelpers::CallHookRun(lua, 2, 0);
			}
		}
	}

	// DrawModelEx 
	using DrawModelExecuteFn = void(__fastcall*)(CModelRender* self, const DrawModelState_t* state, ModelRenderInfo_t* pInfo, matrix3x4_t* pCustomBoneToWorld);
	DrawModelExecuteFn DrawModelExecuteOriginal = nullptr;

	struct DMEContext {
		bool in_hook = false;
		CModelRender* self = nullptr;
		const DrawModelState_t* state = nullptr;
		ModelRenderInfo_t* pInfo = nullptr;
		matrix3x4_t* pCustomBoneToWorld = nullptr;
	} dmeContext;

	void callDMEViaContext()
	{
		if (dmeContext.in_hook)
			DrawModelExecuteOriginal(dmeContext.self, dmeContext.state, dmeContext.pInfo, dmeContext.pCustomBoneToWorld);
	}

	void __fastcall DrawModelExecuteHookFunc(CModelRender* self, const DrawModelState_t* state, ModelRenderInfo_t* pInfo, matrix3x4_t* pCustomBoneToWorld) {

		dmeContext.in_hook = true;
		dmeContext.self = self;
		dmeContext.state = state;
		dmeContext.pInfo = pInfo;
		dmeContext.pCustomBoneToWorld = pCustomBoneToWorld;

		if (pInfo->entity_index) {
			if (luaInit) {
				auto* lua = interfaces::clientLua;

				if (LuaHelpers::PushHookRun(lua, "PreDrawModelExecute" ) != 0) {
					lua->PushNumber(pInfo->entity_index);
					lua->PushNumber(pInfo->flags);

					LuaHelpers::CallHookRun(lua, 2, 0);
				}
			}
		}

		DrawModelExecuteOriginal(self, state, pInfo, pCustomBoneToWorld);

		if (pInfo->entity_index) {
			if (luaInit) {
				auto* lua = interfaces::clientLua;

				if (LuaHelpers::PushHookRun(lua, "PostDrawModelExecute" ) != 0) {
					lua->PushNumber(pInfo->entity_index);
					lua->PushNumber(pInfo->flags);

					LuaHelpers::CallHookRun(lua, 2, 0);
				}
			}
		}

		dmeContext.in_hook = false;
	}

	// SendNetMsg 
	using SendNetMsgFn = bool(__fastcall*)(INetChannel* self, INetMessage& msg, bool bForceReliable, bool bVoice);
	SendNetMsgFn SendNetMsgOriginal = nullptr;

	void SendMove() {
		uint8_t data[MAX_CMD_BUFFER];

		int nextCommandNr = interfaces::clientState->lastoutgoingcommand + interfaces::clientState->chokedcommands + 1;

		CLC_Move moveMsg;
		moveMsg.Init();

		moveMsg.m_DataOut.StartWriting(data, sizeof(data));

		// How many real new commands have queued up
		moveMsg.m_nNewCommands = 1 + interfaces::clientState->chokedcommands;
		moveMsg.m_nNewCommands = std::clamp(moveMsg.m_nNewCommands, 0, MAX_NEW_COMMANDS);

		int extraCommands = interfaces::clientState->chokedcommands + 1 - moveMsg.m_nNewCommands;

		int backupCommands = max(2, extraCommands);
		moveMsg.m_nBackupCommands = std::clamp(backupCommands, 0, MAX_BACKUP_COMMANDS);

		int numCommands = moveMsg.m_nNewCommands + moveMsg.m_nBackupCommands;

		int from = -1;	// first command is deltaed against zeros 

		bool bOK = true;

		for (int to = nextCommandNr - numCommands + 1; to <= nextCommandNr; to++) {
			bool isnewcmd = to >= (nextCommandNr - moveMsg.m_nNewCommands + 1);

			// first valid command number is 1
			bOK = bOK && interfaces::client->WriteUsercmdDeltaToBuffer(&moveMsg.m_DataOut, from, to, isnewcmd);
			from = to;
		}

		// only write message if all usercmds were written correctly, otherwise parsing would fail
		if (bOK) {
			INetChannel* chan = reinterpret_cast<INetChannel*>(interfaces::engineClient->GetNetChannelInfo());

			chan->m_nChokedPackets -= extraCommands;

			SendNetMsgOriginal(chan, reinterpret_cast<INetMessage&>(moveMsg), false, false);
		}
	}

	bool __fastcall SendNetMsgHookFunc(INetChannel* self, INetMessage& msg, bool bForceReliable, bool bVoice) {
		if (msg.GetGroup() == static_cast<int>(NetMessage::clc_VoiceData))
			bVoice = true;

		auto msgName = msg.GetName();

		if (luaInit) {
			auto* lua = interfaces::clientLua;

			if (LuaHelpers::PushHookRun(lua, "SendNetMsg" ) != 0) {
				lua->PushString(msgName);

				bool dontsend = false;
				if (LuaHelpers::CallHookRun(lua, 1, 1))
				{
					if (lua->IsType(-1, Type::Bool))
						dontsend = lua->GetBool(-1);

					lua->Pop(1);
				}
				if (dontsend)
					return true;
			}
		}

		if (strcmp(msgName, "clc_Move") == 0) {
			SendMove();

			return true;
		}

		return SendNetMsgOriginal(self, msg, bForceReliable, bVoice);
	}

	// CheckForSequenceChange
	using SeqChangeFn = bool(__fastcall*)(void* self, CStudioHdr* hdr, int nCurSequence, bool bForceNewSequence, bool bInterpolate);
	SeqChangeFn SeqChangeOriginal = nullptr;

	bool __fastcall CheckForSequenceChangeHookFunc(void* self, CStudioHdr* hdr, int nCurSequence, bool bForceNewSequence, bool bInterpolate) {
		if (!globals::shouldInterpolateSequences)
			bInterpolate = false;

		return SeqChangeOriginal(self, hdr, nCurSequence, bForceNewSequence, bInterpolate);
	}

	// CL_Move
	using CLMoveFn = void(__fastcall*)(float accumulated_extra_samples, bool bFinalTick);
	CLMoveFn CLMoveOriginal = nullptr;

	void __fastcall CLMoveHookFunc(float accumulated_extra_samples, bool bFinalTick) {
		if (luaInit) {
			auto* lua = interfaces::clientLua;

			if (LuaHelpers::PushHookRun(lua, "OnMove" ) != 0) {
				bool dontcall = false;
				if (LuaHelpers::CallHookRun(lua, 0, 1))
				{
					if (lua->IsType(-1, Type::Bool))
						dontcall = lua->GetBool(-1);

					lua->Pop(1);
				}
				if (dontcall)
					return;
			}
		}

		do {
			globals::bLoopMove = false;

			CLMoveOriginal(accumulated_extra_samples, bFinalTick);
		} while (globals::bLoopMove);
	}

	// Interpolate
	using InterpolateFn = bool(__fastcall*)(CBasePlayer* self, float currentTime);
	InterpolateFn InterpolateOriginal = nullptr;

	bool __fastcall InterpolateHookFunc(CBasePlayer* self, float currentTime) {
		if (globals::shouldInterpolate)
			return InterpolateOriginal(self, currentTime);

		return (self == localPlayer) ? InterpolateOriginal(self, currentTime) : true;
	}

	// UpdateClientsideAnimation
	using UpdateClientsideAnimationFn = void(__fastcall*)(CBasePlayer* self);
	UpdateClientsideAnimationFn UpdateClientsideAnimationOriginal = nullptr;

	void __fastcall UpdateClientsideAnimationHookFunc(CBasePlayer* self) {
		if (!globals::shouldFixAnimations)
			return UpdateClientsideAnimationOriginal(self);

		bool updateAllowed = false;
		int updateTicks = 1;

		if (luaInit) {
			auto* lua = interfaces::clientLua;

			if (LuaHelpers::PushHookRun(lua, "ShouldUpdateAnimation" ) != 0) {
				self->PushEntity();

				if (LuaHelpers::CallHookRun(lua, 1, 2))
				{
					if (lua->IsType(-1, Type::Number))
						updateTicks = lua->GetNumber(-1);

					if (lua->IsType(-2, Type::Bool))
						updateAllowed = lua->GetBool(-2);

					lua->Pop(2);
				}
			}
		}

		if (!updateAllowed)
			return;

		float OldCurtime = interfaces::globalVars->curtime;
		float OldFrameTime = interfaces::globalVars->frametime;

		interfaces::globalVars->curtime = self->m_flSimulationTime();
		interfaces::globalVars->frametime = interfaces::globalVars->interval_per_tick * updateTicks;

		UpdateClientsideAnimationOriginal(self);

		interfaces::globalVars->curtime = OldCurtime;
		interfaces::globalVars->frametime = OldFrameTime;
	}

	// Dispatch effect 
	void hookEffect(ClientEffectCallback hookFunc, const char* effectName) {
		static CClientEffectRegistration* s_pHead = *reinterpret_cast<CClientEffectRegistration**>(getAbsAddr(findPattern("client.dll", "48 8B 1D ?? ?? ?? ?? 48 85 DB 74 ?? 0F 1F 40 ?? 48 8B 0B")));
		for (CClientEffectRegistration* pReg = s_pHead; pReg; pReg = pReg->m_pNext) {
			if (pReg->m_pEffectName && strcmp(pReg->m_pEffectName, effectName) == 0) {
				impactFunctionOriginal = pReg->m_pFunction;
				pReg->m_pFunction = hookFunc;
			}
		}
	}
	
	void __fastcall ImpactFunctionHookFunc(const CEffectData& data) {
		if (luaInit) {
			auto* lua = interfaces::clientLua;

			if (LuaHelpers::PushHookRun(lua, "OnImpact" ) != 0) {
				lua->CreateTable();
				{
					lua->PushVector(data.m_vOrigin);
					lua->SetField(-2, "m_vOrigin");

					lua->PushVector(data.m_vStart);
					lua->SetField(-2, "m_vStart");

					lua->PushVector(data.m_vNormal);
					lua->SetField(-2, "m_vNormal");

					lua->PushAngle(data.m_vAngles);
					lua->SetField(-2, "m_vAngles");

					lua->PushNumber(data.m_nEntIndex);
					lua->SetField(-2, "m_nEntIndex");

					lua->PushNumber(data.m_nDamageType);
					lua->SetField(-2, "m_nDamageType");

					lua->PushNumber(data.m_nHitBox);
					lua->SetField(-2, "m_nHitBox");
				}

				LuaHelpers::CallHookRun(lua, 1, 0);
			}
		}

		impactFunctionOriginal(data);
	}

	void SimTimeProxyFunc(const CRecvProxyData* pData, void* pStruct, void* pOut)
	{
		if (!pData->m_Value.m_Int)
			return;

		simTimeProxyFuncOriginal(pData, pStruct, pOut);
	}

	void hookSimTime() {
		ClientClass* clientClass = interfaces::client->GetAllClasses();
		while (clientClass) {
			RecvTable* tbl = clientClass->m_pRecvTable;
			if (strcmp(tbl->m_pNetTableName, "DT_BaseEntity") == 0) {
				for (int i = 0; i < tbl->m_nProps; i++) {
					RecvProp* prop = &tbl->m_pProps[i];
					if (strcmp(prop->m_pVarName, "m_flSimulationTime") == 0) {
						simTimeProxyFuncOriginal = prop->m_ProxyFn;
						prop->m_ProxyFn = SimTimeProxyFunc;
						simTimeRecvProp = prop;
						return;
					}
				}
			}
			clientClass = clientClass->m_pNext;
		}
	}

	void hook() {
		void* SendNetMsgT = vmt::get<void*>(interfaces::engineClient->GetNetChannel(), 40);

		vmt::hook(interfaces::clientMode, &preCreateMoveOriginal, (const void*)ClientModeCreateMoveHookFunc, 21);
		vmt::hook(interfaces::client, &postCreateMoveOriginal, (const void*)CreateMoveHookFuncNaked, 21);
		vmt::hook(interfaces::client, &FrameStageOriginal, (const void*)FrameStageNotifyHookFunc, 35);
		vmt::hook(interfaces::input, &GetUserCmdOriginal, (const void*)GetUserCmdHookFunc, 8);
		vmt::hook(interfaces::prediction, &RunCommandOriginal, (const void*)RunCommandHookFunc, 17);
		vmt::hook(interfaces::modelRender, &DrawModelExecuteOriginal, (const void*)DrawModelExecuteHookFunc, 20);

		hookEffect(ImpactFunctionHookFunc, "Impact");
		hookSimTime();

		if (MH_Initialize() == MH_OK)
		{
			MH_CreateHook(SendNetMsgT, (LPVOID)&SendNetMsgHookFunc, (LPVOID*)&SendNetMsgOriginal);

			MH_CreateHook((LPVOID)SeqChangePattern, (LPVOID)&CheckForSequenceChangeHookFunc, (LPVOID*)&SeqChangeOriginal);
			MH_CreateHook((LPVOID)CL_MovePattern, (LPVOID)&CLMoveHookFunc, (LPVOID*)&CLMoveOriginal);

			MH_EnableHook(MH_ALL_HOOKS);
		}
	}

	void postInit() {
		localPlayer = reinterpret_cast<CBasePlayer*>(interfaces::entityList->GetClientEntity(interfaces::engineClient->GetLocalPlayer()));

		void* InterpolateT = vmt::get<void*>(localPlayer, 100);
		void* UpdateClientAnimsT = vmt::get<void*>(localPlayer, 237);

		MH_CreateHook(InterpolateT, (LPVOID)&InterpolateHookFunc, (LPVOID*)&InterpolateOriginal);
		MH_CreateHook(UpdateClientAnimsT, (LPVOID)&UpdateClientsideAnimationHookFunc, (LPVOID*)&UpdateClientsideAnimationOriginal);

		MH_EnableHook(InterpolateT);
		MH_EnableHook(UpdateClientAnimsT);
	}

	void unHook() {
		PreCreateMoveFn PreCreateMoveDummy = nullptr; 
		PostCreateMoveFn PostCreateMoveDummy = nullptr;
		FrameStageFn FrameStageDummy = nullptr;
		GetUserCmdFn GetUserCmdDummy = nullptr;
		RunCommandFn RunCommandDummy = nullptr;
		DrawModelExecuteFn DrawModelDummy = nullptr;

		vmt::hook(interfaces::clientMode, &PreCreateMoveDummy, preCreateMoveOriginal, 21);
		vmt::hook(interfaces::client, &PostCreateMoveDummy, postCreateMoveOriginal, 21);
		vmt::hook(interfaces::client, &FrameStageDummy, FrameStageOriginal, 35);
		vmt::hook(interfaces::input, &GetUserCmdDummy, GetUserCmdOriginal, 8);
		vmt::hook(interfaces::prediction, &RunCommandDummy, RunCommandOriginal, 17);
		vmt::hook(interfaces::modelRender, &DrawModelDummy, DrawModelExecuteOriginal, 20);

		hookEffect(impactFunctionOriginal, "Impact");
		simTimeRecvProp->m_ProxyFn = simTimeProxyFuncOriginal;

		MH_DisableHook(MH_ALL_HOOKS);
		MH_Uninitialize();
	}
}