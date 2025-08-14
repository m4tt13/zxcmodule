#pragma once

#define GMOD_USE_SOURCESDK
#include "GarrysMod/Lua/LuaBase.h"
using namespace GarrysMod::Lua;

class IEngineClient;
class IGameMovement;
class IMoveHelper;
class IEngineTrace;
class IClientEntityList;
class IPanel;
class ISurface;
class CVRenderView;
class CHLClient;
class CInput;
class CClientState;
class ClientModeShared;
class CGlobalVars;
class CPrediction;
class CLuaShared;
class CCvar;
class CModelRender;
class CViewRender;

namespace interfaces {
	extern IEngineClient*		engineClient;
	extern CModelRender*		modelRender;
	extern IClientEntityList*	entityList;
	extern CHLClient*			client;
	extern CInput*				input;
	extern CClientState*		clientState;
	extern ClientModeShared*	clientMode;
	extern CGlobalVars*			globalVars;
	extern CLuaShared*			luaShared;
	extern ILuaBase*			clientLua;
	extern IPanel*				panel;
	extern CPrediction*			prediction;
	extern IGameMovement*		gameMovement;
	extern IMoveHelper*			moveHelper;
	extern IEngineTrace*		engineTrace;
	extern CCvar*				cvar;
	extern ISurface*			surface;
	extern CViewRender*         view;

	void init();
}