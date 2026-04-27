#include "entity.h"

void CBaseEntity::AllocateIntermediateData( void )
{
	using AllocateIntermediateDataFn = void(__fastcall*)(void*);
	static AllocateIntermediateDataFn _AllocateIntermediateData = (AllocateIntermediateDataFn)findPattern("client.dll", "40 57 48 83 EC ? 48 83 B9 ? ? ? ? ? 48 8B F9 0F 85 ? ? ? ? 48 8B 01");

	_AllocateIntermediateData( this );
}

void CBaseEntity::SetPredictionRandomSeed( int seed )
{
	static int* _predictionRandomSeed = reinterpret_cast<int*>(getAbsAddr(findPattern("client.dll", "0F B6 1D ?? ?? ?? ?? 0F 29 74 24")));

	*_predictionRandomSeed = seed;
}

void CBaseEntity::SetPredictionPlayer( const CBaseHandle& hndl )
{
	CBaseHandle* _predictionPlayer = reinterpret_cast<CBaseHandle*>(getAbsAddr(findPattern("client.dll", "44 89 15 ? ? ? ? 66 0F 6E 87")));

	*_predictionPlayer = hndl;
}