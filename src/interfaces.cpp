#include "interfaces.h"
#include "util.h"

#include "clientstate.h"

#include <Windows.h>
#include <bit>

using CreateInterfaceFn = void* (__fastcall*)(const char*, int*);
namespace interfaces {
	enum IFaceLocation {
		CLIENT,
		ENGINE,
		LUASHARED,
		VGUI,
		VStdLib,
		Surface
	};
	 
	CreateInterfaceFn ClientCreateInterface		= nullptr;
	CreateInterfaceFn EngineCreateInterface		= nullptr;
	CreateInterfaceFn SurfaceCreateInterface	= nullptr;
	CreateInterfaceFn LuaSharedCreateInterface	= nullptr;
	CreateInterfaceFn VGUICreateInterface		= nullptr;
	CreateInterfaceFn VStdLibCreateInterface	= nullptr;

	bool hadError = false;

	template <IFaceLocation location, typename T>
	static void CreateInterface(const char* name, T*& outIFace) {
		int status;

		void* iface;
		if constexpr (location == CLIENT) {
			iface = ClientCreateInterface(name, &status);
		}
		else if constexpr (location == ENGINE) {
			iface = EngineCreateInterface(name, &status);
		}
		else if constexpr (location == LUASHARED) {
			iface = LuaSharedCreateInterface(name, &status);
		}
		else if constexpr (location == VStdLib) {
			iface = VStdLibCreateInterface(name, &status);
		}
		else if constexpr (location == Surface) {
			iface = SurfaceCreateInterface(name, &status);
		}
		else {
			iface = VGUICreateInterface(name, &status);
		}

		if (!iface) {
			hadError = true;
		}

		outIFace = reinterpret_cast<T*>(iface);
	}

	IEngineClient* engineClient = nullptr;
	CHLClient* client = nullptr;
	CInput* input = nullptr; 
	CClientState* clientState = nullptr;
	ClientModeShared* clientMode = nullptr;
	CGlobalVars* globalVars = nullptr;
	IClientEntityList* entityList = nullptr;
	IPanel* panel = nullptr;
	CPrediction* prediction = nullptr;
	IGameMovement* gameMovement = nullptr;
	IMoveHelper* moveHelper = nullptr;
	IEngineTrace* engineTrace = nullptr;
	CLuaShared* luaShared = nullptr;
	ILuaBase* clientLua = nullptr;
	CCvar* cvar = nullptr;
	ISurface* surface = nullptr;
	CModelRender* modelRender = nullptr;
	CViewRender* view = nullptr;

	void init() {
		HMODULE clientDll		= GetModuleHandleA("client.dll");
		HMODULE engineDll		= GetModuleHandleA("engine.dll");
		HMODULE luaSharedDll	= GetModuleHandleA("lua_shared.dll");
		HMODULE vguiDll			= GetModuleHandleA("vgui2.dll");
		HMODULE vstdlibDll		= GetModuleHandleA("vstdlib.dll");
		HMODULE surfaceDll		= GetModuleHandleA("vguimatsurface.dll");

		ClientCreateInterface		= reinterpret_cast<CreateInterfaceFn>(GetProcAddress(clientDll, "CreateInterface"));
		EngineCreateInterface		= reinterpret_cast<CreateInterfaceFn>(GetProcAddress(engineDll, "CreateInterface"));
		LuaSharedCreateInterface	= reinterpret_cast<CreateInterfaceFn>(GetProcAddress(luaSharedDll, "CreateInterface"));
		VGUICreateInterface			= reinterpret_cast<CreateInterfaceFn>(GetProcAddress(vguiDll, "CreateInterface"));
		VStdLibCreateInterface		= reinterpret_cast<CreateInterfaceFn>(GetProcAddress(vstdlibDll, "CreateInterface"));
		SurfaceCreateInterface		= reinterpret_cast<CreateInterfaceFn>(GetProcAddress(surfaceDll, "CreateInterface"));

		CreateInterface<ENGINE>("VEngineClient015", engineClient);
		CreateInterface<ENGINE>("VEngineModel016", modelRender);
		CreateInterface<ENGINE>("EngineTraceClient003", engineTrace);

		CreateInterface<CLIENT>("VClient017", client);
		CreateInterface<CLIENT>("VClientEntityList003",	entityList);
		CreateInterface<CLIENT>("VClientPrediction001",	prediction);
		CreateInterface<CLIENT>("GameMovement001", gameMovement);
		
		CreateInterface<VStdLib>("VEngineCvar007", cvar);
		CreateInterface<Surface>("VGUI_Surface030",	surface);
		CreateInterface<VGUI>("VGUI_Panel009", panel);
		CreateInterface<LUASHARED>("LUASHARED003", luaShared);

		input = *reinterpret_cast<CInput**>(getAbsAddr(vmt::get<std::uintptr_t>(client, 21) + 0x3F));
		globalVars = *reinterpret_cast<CGlobalVars**>(getAbsAddr(vmt::get<std::uintptr_t>(client, 0) + 0x94));
		clientMode = *reinterpret_cast<ClientModeShared**>(getAbsAddr(vmt::get<std::uintptr_t>(client, 10)));
		view = *reinterpret_cast<CViewRender**>(getAbsAddr(vmt::get<std::uintptr_t>(client, 2) + 0xC4));

		moveHelper = *reinterpret_cast<IMoveHelper**>(getAbsAddr(findPattern("client.dll", "48 8B 0D ?? ?? ?? ?? 48 8B 01 FF 50 ?? 48 8B 03 48 8B CB FF 90 ?? ?? ?? ?? 48 8B 4B")));
		clientState = reinterpret_cast<CClientState*>(getAbsAddr(findPattern("engine.dll", "F3 0F 58 05 ?? ?? ?? ?? FF C8") + 0x1));
	}
}
