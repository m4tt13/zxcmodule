#pragma once

struct typedescription_t;

struct datamap_t
{
	typedescription_t	*dataDesc;
	int					dataNumFields;
	char const			*dataClassName;
	datamap_t			*baseMap;
	bool				chains_validated;
	bool				packed_offsets_computed;
	int					packed_size;
};