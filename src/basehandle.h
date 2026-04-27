#pragma once

#define INVALID_EHANDLE_INDEX 0xFFFFFFFF

class CBaseHandle
{
public:
	CBaseHandle() : m_Index(INVALID_EHANDLE_INDEX) {}
	CBaseHandle( unsigned long value ) : m_Index(value) {}

	unsigned long m_Index;
};