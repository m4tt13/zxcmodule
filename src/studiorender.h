#pragma once

#include "vector.h"
#include <cstdint>

#define	VERTEX_LAST_BIT					10

#define	VERTEX_BONE_WEIGHT_BIT			(VERTEX_LAST_BIT + 1)
#define	USER_DATA_SIZE_BIT				(VERTEX_LAST_BIT + 4)
#define	TEX_COORD_SIZE_BIT				(VERTEX_LAST_BIT + 7)

#define	VERTEX_BONE_WEIGHT_MASK			( 0x7 << VERTEX_BONE_WEIGHT_BIT )
#define	USER_DATA_SIZE_MASK				( 0x7 << USER_DATA_SIZE_BIT )

#define	VERTEX_FORMAT_FIELD_MASK		0x0FF

#define VERTEX_BONEWEIGHT( _n )				((_n) << VERTEX_BONE_WEIGHT_BIT)
#define VERTEX_USERDATA_SIZE( _n )			((_n) << USER_DATA_SIZE_BIT)
#define VERTEX_TEXCOORD_MASK( _coord )		(( 0x7ULL ) << ( TEX_COORD_SIZE_BIT + 3 * (_coord) ))
typedef uint64_t VertexFormat_t;

#define	VERTEX_POSITION					0x0001
#define	VERTEX_NORMAL					0x0002
#define	VERTEX_COLOR					0x0004
#define	VERTEX_SPECULAR					0x0008
#define	VERTEX_BONE_INDEX				0x0080

inline VertexFormat_t VERTEX_TEXCOORD_SIZE(int nIndex, int nNumCoords)
{
	uint64_t n64 = nNumCoords;
	uint64_t nShift = TEX_COORD_SIZE_BIT + (3 * nIndex);
	return n64 << nShift;
}

enum OverrideType_t
{
	OVERRIDE_NORMAL = 0,
	OVERRIDE_BUILD_SHADOWS,
	OVERRIDE_DEPTH_WRITE,
	OVERRIDE_SSAO_DEPTH_WRITE,
};

struct StudioRenderConfig_t
{
	float fEyeShiftX;	// eye X position
	float fEyeShiftY;	// eye Y position
	float fEyeShiftZ;	// eye Z position
	float fEyeSize;		// adjustment to iris textures
	float fEyeGlintPixelWidthLODThreshold;

	int maxDecalsPerModel;
	int drawEntities;
	int skin;
	int fullbright;

	bool bEyeMove : 1;		// look around
	bool bSoftwareSkin : 1;
	bool bNoHardware : 1;
	bool bNoSoftware : 1;
	bool bTeeth : 1;
	bool bEyes : 1;
	bool bFlex : 1;
	bool bWireframe : 1;
	bool bDrawNormals : 1;
	bool bDrawTangentFrame : 1;
	bool bDrawZBufferedWireframe : 1;
	bool bSoftwareLighting : 1;
	bool bShowEnvCubemapOnly : 1;
	bool bWireframeDecals : 1;

	// Reserved for future use
	int m_nReserved[4];
};

enum
{
	ADDDECAL_TO_ALL_LODS = -1
};

enum
{
	STUDIORENDER_DRAW_ENTIRE_MODEL = 0,
	STUDIORENDER_DRAW_OPAQUE_ONLY = 0x01,
	STUDIORENDER_DRAW_TRANSLUCENT_ONLY = 0x02,
	STUDIORENDER_DRAW_GROUP_MASK = 0x03,

	STUDIORENDER_DRAW_NO_FLEXES = 0x04,
	STUDIORENDER_DRAW_STATIC_LIGHTING = 0x08,

	STUDIORENDER_DRAW_ACCURATETIME = 0x10,		// Use accurate timing when drawing the model.
	STUDIORENDER_DRAW_NO_SHADOWS = 0x20,
	STUDIORENDER_DRAW_GET_PERF_STATS = 0x40,

	STUDIORENDER_DRAW_WIREFRAME = 0x80,

	STUDIORENDER_DRAW_ITEM_BLINK = 0x100,

	STUDIORENDER_SHADOWDEPTHTEXTURE = 0x200,

	STUDIORENDER_SSAODEPTHTEXTURE = 0x1000,

	STUDIORENDER_GENERATE_STATS = 0x8000,
};

#define VERTEX_TEXCOORD0_2D ( ( (uint64_t) 2 ) << ( TEX_COORD_SIZE_BIT + ( 3*0 ) ) )
enum MaterialVertexFormat_t
{
	MATERIAL_VERTEX_FORMAT_MODEL_SKINNED = (VertexFormat_t)VERTEX_POSITION | VERTEX_COLOR | VERTEX_NORMAL | VERTEX_TEXCOORD0_2D | VERTEX_BONEWEIGHT(2) | VERTEX_BONE_INDEX | VERTEX_USERDATA_SIZE(4),
	MATERIAL_VERTEX_FORMAT_MODEL_SKINNED_DX7 = (VertexFormat_t)VERTEX_POSITION | VERTEX_COLOR | VERTEX_NORMAL | VERTEX_TEXCOORD0_2D | VERTEX_BONEWEIGHT(2) | VERTEX_BONE_INDEX,
	MATERIAL_VERTEX_FORMAT_MODEL = (VertexFormat_t)VERTEX_POSITION | VERTEX_COLOR | VERTEX_NORMAL | VERTEX_TEXCOORD0_2D | VERTEX_USERDATA_SIZE(4),
	MATERIAL_VERTEX_FORMAT_MODEL_DX7 = (VertexFormat_t)VERTEX_POSITION | VERTEX_COLOR | VERTEX_NORMAL | VERTEX_TEXCOORD0_2D,
	MATERIAL_VERTEX_FORMAT_COLOR = (VertexFormat_t)VERTEX_SPECULAR
};
struct ColorMeshInfo_t
{
	void* m_pMesh;
	void* m_pPooledVBAllocator;
	int						m_nVertOffsetInBytes;
	int						m_nNumVerts;
};

struct DrawModelInfo_t
{
	void* m_pStudioHdr;
	void* m_pHardwareData;
	void* m_Decals;
	int				m_Skin;
	int				m_Body;
	int				m_HitboxSet;
	void* m_pClientEntity;
	int				m_Lod;
	ColorMeshInfo_t* m_pColorMeshes;
	bool			m_bStaticLighting;
	Vector			m_vecAmbientCube[6];		// ambient, and lights that aren't in locallight[]
	int				m_nLocalLightCount;
	int		m_LocalLightDescs[4];
};