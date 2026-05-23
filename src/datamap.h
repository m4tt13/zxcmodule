#pragma once

typedef enum _fieldtypes {
	FIELD_VOID = 0,		
	FIELD_FLOAT,		
	FIELD_STRING,		
	FIELD_VECTOR,		
	FIELD_QUATERNION,	
	FIELD_INTEGER,		
	FIELD_BOOLEAN,		
	FIELD_SHORT,		
	FIELD_CHARACTER,	
	FIELD_COLOR32,		
	FIELD_EMBEDDED,		
	FIELD_CUSTOM,		
	FIELD_CLASSPTR,		
	FIELD_EHANDLE,		
	FIELD_EDICT,		
	FIELD_POSITION_VECTOR,
	FIELD_TIME,			
	FIELD_TICK,			
	FIELD_MODELNAME,	
	FIELD_SOUNDNAME,	
	FIELD_INPUT,		
	FIELD_FUNCTION,		
	FIELD_VMATRIX,		
	FIELD_VMATRIX_WORLDSPACE,
	FIELD_MATRIX3X4_WORLDSPACE,
	FIELD_INTERVAL,			
	FIELD_MODELINDEX,		
	FIELD_MATERIALINDEX,	
	FIELD_VECTOR2D,
	FIELD_INTEGER64,
	FIELD_GMODTABLE,
	FIELD_DOUBLE,
	FIELD_TIME64,
	FIELD_TYPECOUNT,
} fieldtype_t;

#define FTYPEDESC_GLOBAL			0x0001
#define FTYPEDESC_SAVE				0x0002
#define FTYPEDESC_KEY				0x0004
#define FTYPEDESC_INPUT				0x0008
#define FTYPEDESC_OUTPUT			0x0010
#define FTYPEDESC_FUNCTIONTABLE		0x0020
#define FTYPEDESC_PTR				0x0040
#define FTYPEDESC_OVERRIDE			0x0080
#define FTYPEDESC_INSENDTABLE		0x0100
#define FTYPEDESC_PRIVATE			0x0200
#define FTYPEDESC_NOERRORCHECK		0x0400
#define FTYPEDESC_MODELINDEX		0x0800
#define FTYPEDESC_INDEX				0x1000
#define FTYPEDESC_VIEW_OTHER_PLAYER	0x2000
#define FTYPEDESC_VIEW_OWN_TEAM		0x4000
#define FTYPEDESC_VIEW_NEVER		0x8000

class ISaveRestoreOps;
class __single_inheritance CBaseEntity;
struct inputdata_t;
typedef void (CBaseEntity::*inputfunc_t)(inputdata_t &data);
struct datamap_t;

enum {
	TD_OFFSET_NORMAL = 0,
	TD_OFFSET_PACKED = 1,
	TD_OFFSET_COUNT,
};

struct typedescription_t {
	fieldtype_t			fieldType;
	const char			*fieldName;
	int					fieldOffset;
	unsigned short		fieldSize;
	short				flags;
	const char			*externalName;	
	ISaveRestoreOps		*pSaveRestoreOps; 
	inputfunc_t			inputFunc; 
	datamap_t			*td;
	int					fieldSizeInBytes;
	struct typedescription_t *override_field;
	int					override_count;
	float				fieldTolerance;
	int					flatOffset[ TD_OFFSET_COUNT ];
	unsigned short		flatGroup;
};

struct datamap_t {
	typedescription_t	*dataDesc;
	int					dataNumFields;
	char const			*dataClassName;
	datamap_t			*baseMap;
	bool				chains_validated;
	bool				packed_offsets_computed;
	int					packed_size;
};