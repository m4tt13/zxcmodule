
#include "globalvars.h"
#include "interfaces.h"
#include "entity.h"
#include "cprediction.h"
#include "predictioncopy.h"
#include "datamap.h"
#include "engineclient.h"
#include "globals.h"
#include "simulation.h"

static int g_FieldSizes[FIELD_TYPECOUNT] = {0,4,8,12,16,4,1,2,1,4,0,0,8,4,8,12,4,4,8,8,8,8,64,64,48,8,4,4,8,8,8,8,8};

MovementSimulation::MovementSimulation() : _player(nullptr), _moveData{0}, _oldInPrediction(false), _oldFirstTimePredicted(false), _oldFrameTime(0.0f), _backup(nullptr) {
}

void MovementSimulation::DestroyBackupData() {
	if (_backup) {
		for (int offset : _dataTableOffsets) {
			IGMODDataTable* dataTable = *reinterpret_cast<IGMODDataTable**>(reinterpret_cast<std::uintptr_t>(_backup) + offset);
			if (dataTable)
				interfaces::engineClient->GMOD_DestroyDataTable(dataTable);
		}
		_dataTableOffsets.clear();
		delete[] _backup;
		_backup = nullptr;
	}
}

int MovementSimulation::CollectDataTableOffsets_R( datamap_t *map ) {
	int current_position = 0;

	if ( map->baseMap )
		current_position += CollectDataTableOffsets_R( map->baseMap );

	int c = map->dataNumFields;
	int i;
	typedescription_t *field;

	for ( i = 0; i < c; i++ ) {
		field = &map->dataDesc[ i ];
		if ( field->fieldType != FIELD_EMBEDDED ) {
			if ( field->flags & FTYPEDESC_PRIVATE )
				continue;
		}

		switch ( field->fieldType )
		{
		case FIELD_EMBEDDED:
			{
				int embeddedsize = CollectDataTableOffsets_R( field->td );
				field->flatOffset[ TD_OFFSET_NORMAL ] = field->fieldOffset;
				field->flatOffset[ TD_OFFSET_PACKED ] = current_position;
				current_position += embeddedsize;
			}
			break;

		case FIELD_GMODTABLE: _dataTableOffsets.push_back( current_position );
		case FIELD_FLOAT:
		case FIELD_VECTOR:
		case FIELD_QUATERNION:
		case FIELD_INTEGER:
		case FIELD_EHANDLE:
		case FIELD_DOUBLE:
			{
				current_position = (current_position + 3) & ~3;
				field->flatOffset[ TD_OFFSET_NORMAL ] = field->fieldOffset;
				field->flatOffset[ TD_OFFSET_PACKED ] = current_position;
				current_position += g_FieldSizes[ field->fieldType ] * field->fieldSize;
			}
			break;

		case FIELD_SHORT:
			{
				current_position = (current_position + 1) & ~1;
				field->flatOffset[ TD_OFFSET_NORMAL ] = field->fieldOffset;
				field->flatOffset[ TD_OFFSET_PACKED ] = current_position;
				current_position += g_FieldSizes[ field->fieldType ] * field->fieldSize;
			}
			break;

		case FIELD_STRING:
		case FIELD_COLOR32:
		case FIELD_BOOLEAN:
		case FIELD_CHARACTER:
			{
				field->flatOffset[ TD_OFFSET_NORMAL ] = field->fieldOffset;
				field->flatOffset[ TD_OFFSET_PACKED ] = current_position;
				current_position += g_FieldSizes[ field->fieldType ] * field->fieldSize;
			}
			break;

		default:
			break;
		}
	}

	map->packed_size = current_position;
	map->packed_offsets_computed = true;

	return current_position;
}

void MovementSimulation::Start(CBasePlayer* player) {
	_player = player;

	player->GetCurrentCommand() = &_dummyCmd;
	player->SetPredictionPlayer(player->GetClientUnknown()->GetRefEHandle());

	Store(player);

	_oldInPrediction = interfaces::prediction->InPrediction();
	_oldFirstTimePredicted = interfaces::prediction->IsFirstTimePredicted();
	_oldFrameTime = interfaces::globalVars->frametime;

	if (player != globals::localPlayer) {
		// the hacks that make it work
		player->m_bDucked() = player->IsDucking();
		player->m_bDucking() = false;
		player->m_bInDuckJump() = false;
		player->m_flDucktime() = 0.0f;
		player->m_flDuckJumpTime() = 0.0f;
		player->m_flJumpTime() = 0.0f;

		if (player->IsOnGround()) {
			player->GetAbsOrigin().z += 0.03125f; // to prevent getting stuck in the ground

			CBaseEntity* worldEnt = interfaces::entityList->GetClientEntity(0);
			if (worldEnt)
				player->m_hGroundEntity() = worldEnt->GetClientUnknown()->GetRefEHandle();
		}
		else {
			player->m_hGroundEntity() = INVALID_EHANDLE_INDEX;
		}
	}

	// Setup move data
	SetupMoveData(player);
}

void MovementSimulation::SimulateTick() {
	if (!_player)
		return;

	interfaces::prediction->GetInPrediction() = true;
	interfaces::prediction->GetIsFirstTimePredicted() = false;
	interfaces::globalVars->frametime = interfaces::prediction->GetEnginePaused() ? 0.0f : interfaces::globalVars->interval_per_tick;

	interfaces::gameMovement->ProcessMovement(_player, &_moveData);
}

void MovementSimulation::Finish() {
	if (!_player)
		return;

	_player->GetCurrentCommand() = nullptr;
	_player->SetPredictionPlayer(INVALID_EHANDLE_INDEX);

	Restore(_player);

	interfaces::prediction->GetInPrediction() = _oldInPrediction;
	interfaces::prediction->GetIsFirstTimePredicted() = _oldFirstTimePredicted;
	interfaces::globalVars->frametime = _oldFrameTime;

	_player = nullptr;

	memset(&_moveData, 0, sizeof(_moveData));
} 

void MovementSimulation::SetupMoveData(CBasePlayer* player) {
	_moveData.m_bFirstRunOfFunctions = false;
	_moveData.m_bGameCodeMovedPlayer = false;
	_moveData.m_nPlayerHandle = player->GetClientUnknown()->GetRefEHandle();
	_moveData.m_vecVelocity = player->GetVelocity();
	_moveData.m_vecAbsOrigin = player->GetAbsOrigin();
	_moveData.m_vecViewAngles = { player->EyePitch(), player->EyeYaw(), 0.0f };
	_moveData.m_vecAngles = _moveData.m_vecViewAngles;
	_moveData.m_vecOldAngles = _moveData.m_vecViewAngles;
	_moveData.m_nButtons = player->IsDucking() ? CUserCmd::IN_DUCK : 0;
	_moveData.m_nOldButtons = _moveData.m_nButtons;
	_moveData.m_flClientMaxSpeed = player->m_flMaxspeed();

	Vector forward = _moveData.m_vecViewAngles.Forward();
	forward.z = 0.0f;
	forward.Normalize();

	Vector right = _moveData.m_vecViewAngles.Right();
	right.z = 0.0f;
	right.Normalize();

	float divisor = forward.x * right.y - right.x * forward.y;
	_moveData.m_flForwardMove = (_moveData.m_vecVelocity.x * right.y - right.x * _moveData.m_vecVelocity.y) / divisor;
	_moveData.m_flSideMove = (forward.x * _moveData.m_vecVelocity.y - _moveData.m_vecVelocity.x * forward.y) / divisor;
}

void MovementSimulation::Store(CBasePlayer* player) {
	datamap_t* map = player->GetPredDescMap();
	if (map) {
		if (!_backup) {
			CollectDataTableOffsets_R(map);
			size_t allocsize = max(map->packed_size, 4);
			_backup = new unsigned char[allocsize];
			memset(_backup, 0, allocsize);
		}

		CPredictionCopy copyHelper(PC_EVERYTHING, _backup, PC_DATA_PACKED, player, PC_DATA_NORMAL);
		copyHelper.TransferData("MovementSimulationStore", player->GetClientNetworkable()->entIndex(), map);
	}
}

void MovementSimulation::Restore(CBasePlayer* player) {
	datamap_t* map = player->GetPredDescMap();
	if (map && _backup) {
		CPredictionCopy copyHelper(PC_EVERYTHING, player, PC_DATA_NORMAL, _backup, PC_DATA_PACKED);
		copyHelper.TransferData("MovementSimulationRestore", player->GetClientNetworkable()->entIndex(), map);
	}
}

MovementSimulation g_simulation;