#include "entity.h"

void CBaseEntity::SetPredictionRandomSeed( int seed ) {
	static int* _predictionRandomSeed = reinterpret_cast<int*>(getAbsAddr(findPattern("client.dll", "0F B6 1D ?? ?? ?? ?? 0F 29 74 24")));

	*_predictionRandomSeed = seed;
}

void CBaseEntity::SetPredictionPlayer( const CBaseHandle& hndl ) {
	static CBaseHandle* _predictionPlayer = reinterpret_cast<CBaseHandle*>(getAbsAddr(findPattern("client.dll", "44 89 15 ? ? ? ? 66 0F 6E 87")));

	*_predictionPlayer = hndl;
}