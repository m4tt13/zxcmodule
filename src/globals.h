
#pragma once

#include <string>

class CBasePlayer;

namespace globals {
	extern bool			bSendPacket;

	extern bool			shouldInterpolate;
	extern bool			shouldInterpolateSequences;
	extern bool			shouldFixAnimations;

	extern bool			bLoopMove;

	extern bool			bCustomDisconnect;	
	extern std::string	customDisconnect;

	extern CBasePlayer* localPlayer;
}
