#pragma once

struct datamap_t;
struct typedescription_t;

enum
{
	TD_OFFSET_NORMAL = 0,
	TD_OFFSET_PACKED = 1,
	TD_OFFSET_COUNT,
};

enum
{
	PC_EVERYTHING = 0,
	PC_NON_NETWORKED_ONLY,
	PC_NETWORKED_ONLY,
};

#define PC_DATA_PACKED			true
#define PC_DATA_NORMAL			false

typedef void ( *FN_FIELD_COMPARE )( const char *classname, const char *fieldname, const char *fieldtype,
	bool networked, bool noterrorchecked, bool differs, bool withintolerance, const char *value );

class CPredictionCopy
{
public:
	CPredictionCopy( int type, void *dest, bool dest_packed, void const *src, bool src_packed,
		bool counterrors = false, bool reporterrors = false, bool performcopy = true, 
		bool describefields = false, FN_FIELD_COMPARE func = nullptr );

	int	TransferData( const char *operation, int entindex, datamap_t *dmap );

private:

	int				m_nType;
	void			*m_pDest;
	void const		*m_pSrc;
	int				m_nDestOffsetIndex;
	int				m_nSrcOffsetIndex;


	bool			m_bErrorCheck;
	bool			m_bReportErrors;
	bool			m_bDescribeFields;
	typedescription_t *m_pCurrentField;
	char const		*m_pCurrentClassName;
	datamap_t		*m_pCurrentMap;
	bool			m_bShouldReport;
	bool			m_bShouldDescribe;
	int				m_nErrorCount;
	bool			m_bPerformCopy;

	FN_FIELD_COMPARE	m_FieldCompareFunc;

	typedescription_t	 *m_pWatchField;
	char const			*m_pOperation;
};