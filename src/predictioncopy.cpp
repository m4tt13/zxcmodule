#include "predictioncopy.h"
#include "patternscan.h"

CPredictionCopy::CPredictionCopy( int type, void *dest, bool dest_packed, void const *src, bool src_packed, 
	bool counterrors /*= false*/, bool reporterrors /*= false*/, bool performcopy /*= true*/,
	bool describefields /*= false*/, FN_FIELD_COMPARE func /*= nullptr*/ )
{
	m_nType				= type;
	m_pDest				= dest;
	m_pSrc				= src;
	m_nDestOffsetIndex	= dest_packed ? TD_OFFSET_PACKED : TD_OFFSET_NORMAL;
	m_nSrcOffsetIndex	= src_packed ? TD_OFFSET_PACKED : TD_OFFSET_NORMAL;
	m_bErrorCheck		= counterrors;
	m_bReportErrors		= reporterrors;
	m_bPerformCopy		= performcopy;
	m_bDescribeFields	= describefields;

	m_pCurrentField		= nullptr;
	m_pCurrentMap		= nullptr;
	m_pCurrentClassName = nullptr;
	m_bShouldReport		= false;
	m_bShouldDescribe	= false;
	m_nErrorCount		= 0;

	m_FieldCompareFunc	= func;
}

int CPredictionCopy::TransferData( const char *operation, int entindex, datamap_t *dmap )
{
	using TransferDataFn = int(__fastcall*)(void*, const char *, int, datamap_t *);
	static TransferDataFn _TransferData = (TransferDataFn)findPattern("client.dll", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC ? FF 05");

	return _TransferData( this, operation, entindex, dmap );
}