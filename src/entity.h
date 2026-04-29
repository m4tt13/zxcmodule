
#pragma once

#include <bit>

#include "animstate.h"
#include "util.h"
#include "defines.h"
#include "varmap.h"
#include "cliententitylist.h"
#include "basehandle.h"

class IClientNetworkable;
class IClientRenderable;
class CBaseAnimating;
class CBaseEntity;
class CBasePlayerAnimState;
class CUserCmd;
class matrix3x4_t;
class Vector;
class Angle;
struct datamap_t;

class IHandleEntity { 
public:
	virtual ~IHandleEntity() {}
	virtual void SetRefEHandle(const CBaseHandle& handle) = 0;
	virtual const CBaseHandle& GetRefEHandle() const = 0;
};

class IClientUnknown : public IHandleEntity {
public:
	virtual void*				GetCollideable() = 0;
	virtual IClientNetworkable*	GetClientNetworkable() = 0;
	virtual IClientRenderable*	GetClientRenderable() = 0;
	virtual void*				GetIClientEntity() = 0;
	virtual CBaseEntity*		GetBaseEntity() = 0;
	virtual void*				GetClientThinkable() = 0;
};

struct ShouldTransmitState_t;  struct DataUpdateType_t; class bf_read;

class IClientNetworkable
{
public:
	virtual IClientUnknown*		GetIClientUnknown() = 0;
	virtual void				Release() = 0;
	virtual ClientClass*		GetClientClass() = 0;
	virtual void				NotifyShouldTransmit(ShouldTransmitState_t state) = 0;
	virtual void				OnPreDataChanged(DataUpdateType_t updateType) = 0;
	virtual void				OnDataChanged(DataUpdateType_t updateType) = 0;
	virtual void				PreDataUpdate(DataUpdateType_t updateType) = 0;
	virtual void				PostDataUpdate(DataUpdateType_t updateType) = 0;
	virtual bool				IsDormant() = 0;
	virtual int					entIndex() = 0;
	virtual void				ReceiveMessage(int classID, bf_read& msg) = 0;
	virtual void*				GetDataTableBasePtr() = 0;
	virtual void				SetDestroyedOnRecreateEntities(void) = 0;
};

class IClientRenderable
{
public:
	virtual IClientUnknown*		GetIClientUnknown() = 0;
};

class CBaseEntity {
public:
	IClientUnknown* GetClientUnknown() { return reinterpret_cast<IClientUnknown*>(this); }
	IClientNetworkable* GetClientNetworkable() { return vmt::call<IClientNetworkable*>(this, 4); }

	VPROXY(GetPredDescMap, 15, datamap_t*, (void));
	VPROXY(GetBaseAnimating, 39, CBaseAnimating*, (void));
	VPROXY(IsPlayer, 131, bool, (void));
	VPROXY(PushEntity, 173, void, (void));

	NETVAR(double, DT_BaseEntity, m_flSimulationTime);

	//NETVAR_(Vector, DT_BaseEntity, m_vecAbsVelocity[0], GetAbsVelocity);
	NETVAR_(Vector, DT_BaseEntity, m_vecVelocity[0], GetVelocity);
	NETVAR_(Vector, DT_BaseEntity, m_vecOrigin, GetAbsOrigin);
	OFFSETVAR(char, m_MoveType, 0x1FC);

	OFFSETVAR(VarMapping_t, GetVarMapping, 40);

	OFFSETVAR(int, m_iEFlags, 0x1F8); 
	NETVAR(int, DT_BaseEntity, m_fEffects);

	OFFSETVAR(unsigned char*, m_pOriginalData, 0x728);

	void AllocateIntermediateData( void );

	static void	SetPredictionRandomSeed( int seed );
	static void	SetPredictionPlayer( const CBaseHandle& hndl );
};

class CBaseWeapon : public CBaseEntity {
public:

};

class CBaseAnimating : public CBaseEntity {
public:
	VPROXY(SetupBones, 16, bool, (matrix3x4_t* pBoneToWorldOut, int nMaxBones, int boneMask), pBoneToWorldOut, nMaxBones, boneMask);
	VPROXY(UpdateClientsideAnimation, 235, void, (void)); // wrong index ( 297 )

	OFFSETVAR(void*, m_hitboxBoneCacheHandle, 0x1A90);
	NETVAR(bool, DT_BaseAnimating, m_bClientSideAnimation);
	NETVAR(float, DT_BaseAnimating, m_flModelScale);
};

class CBasePlayer : public CBaseAnimating {
public:
	MoveType GetMoveType() { return static_cast<MoveType>(m_MoveType()); }
	WaterLevel GetWaterLevel() { return static_cast<WaterLevel>(m_nWaterLevel()); }

	bool HasFlag(EntityFlags flag) { return m_fFlags() & static_cast<int>(flag); }
	bool IsOnGround() { return HasFlag(EntityFlags::ONGROUND); }
	bool IsDucking() { return HasFlag(EntityFlags::DUCKING); }
	bool IsInWater() { return GetWaterLevel() != WaterLevel::NotInWater; }
	bool IsInNoclip() { return GetMoveType() == MoveType::NOCLIP; }

	// CPrediction__RunCommand 2 line
	OFFSETVAR(CUserCmd*, GetCurrentCommand, 0x2C50); 
	OFFSETVAR(CBasePlayerAnimState*, GetAnimState, 0x35D8);

	NETVAR(char, DT_GMOD_Player, m_nWaterLevel);

	NETVAR_(float, DT_GMOD_Player, m_angEyeAngles[0], EyePitch);
	NETVAR_(float, DT_GMOD_Player, m_angEyeAngles[1], EyeYaw);

	NETVAR_(Vector, DT_BasePlayer, m_vecBaseVelocity, GetBaseVelocity);
	NETVAR_(Vector, DT_BasePlayer, m_vecViewOffset[0], GetViewOffset);

	NETVAR(int, DT_BasePlayer, m_nTickBase);
	NETVAR(int, DT_BasePlayer, m_fFlags);
	NETVAR(CBaseHandle, DT_BasePlayer, m_hActiveWeapon);

	NETVAR(bool, DT_BasePlayer, m_bDucked);
	NETVAR(float, DT_BasePlayer, m_flDucktime);
	NETVAR(float, DT_BasePlayer, m_flDuckJumpTime);
	NETVAR(float, DT_BasePlayer, m_flJumpTime);
	NETVAR(bool, DT_BasePlayer, m_bDucking);
	NETVAR(bool, DT_BasePlayer, m_bInDuckJump);

	NETVAR(float, DT_BasePlayer, m_flMaxspeed);

	NETVAR(CBaseHandle, DT_BasePlayer, m_hGroundEntity);
};

inline CBasePlayer *ToBasePlayer( CBaseEntity *pEntity )
{
	if ( !pEntity || !pEntity->IsPlayer() )
		return nullptr;

	return static_cast<CBasePlayer *>( pEntity );
}