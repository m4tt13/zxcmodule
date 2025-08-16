
#pragma once

class INetChannelInfo;
class INetChannel;

enum ClientFrameStage_t
{
	FRAME_UNDEFINED = -1,						// (haven't run any frames yet)
	FRAME_START = 0,
	FRAME_NET_UPDATE_START = 1,					// A network packet is being recieved
	FRAME_NET_UPDATE_POSTDATAUPDATE_START = 2,	// Data has been received and we're going to start calling PostDataUpdate 
	FRAME_NET_UPDATE_POSTDATAUPDATE_END = 3,	// Data has been received and we've called PostDataUpdate on all data recipients
	FRAME_NET_UPDATE_END = 4,					// We've received all packets, we can now do interpolation, prediction, etc..
	FRAME_RENDER_START = 5,						// We're about to start rendering the scene
	FRAME_RENDER_END = 6 						// We've finished rendering the scene.
};

#include "vmt.h"
#include "util.h"

#include "angle.h"

class IEngineClient {
public:
	VPROXY(ServerCmd, 6, void, (char const*, bool));
	VPROXY(ClientCmd, 7, void, (char const*));
	VPROXY(GetLocalPlayer, 12, int, (void));
	VPROXY(Time, 14, double, (void));
	VPROXY(GetLastTimeStamp, 15, float, (void));
	VPROXY(SetViewAngles, 20, void, (Angle&));
	VPROXY(IsBoxVisible, 31, bool, (Vector const&, Vector const&));
	VPROXY(IsBoxInViewCluster, 32, bool, (Vector const&, Vector const&));
	VPROXY(GetGameDirectory, 35, char const*, (void));
	VPROXY(IsOccluded, 69, bool, (Vector const&, Vector const&));
	VPROXY(GetNetChannelInfo, 72, INetChannelInfo*, (void));
	VPROXY(IsPlayingTimeDemo, 78, bool, (void));
	VPROXY(ExecuteClientCmd, 102, void, (char const*));
	VPROXY(ClientCmd_Unrestricted, 106, void, (char const*));
	VPROXY(SetRestrictServerCommands, 107, void, (bool));
	VPROXY(SetRestrictClientCommands, 108, void, (bool));
	VPROXY(GMOD_RawClientCmd_Unrestricted, 140, void, (char const*));
	 
	INetChannel* GetNetChannel() { return reinterpret_cast<INetChannel*>(GetNetChannelInfo()); }
}; 
 