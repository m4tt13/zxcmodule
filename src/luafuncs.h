#pragma once

#include "cutlvector.h"

class lua_library_func_t {
public:
	const char* m_func_name;
	char m_pad[4];
	int(*m_handler)(void*);
};

class lua_library_t {
public:
	virtual ~lua_library_t() = default;
	virtual bool ___unknown_get_bool() = 0;
	virtual void __unknown_void() = 0;

	bool m_some_bool;
	const char* m_table_name;
	CUtlVector<lua_library_func_t*> m_lib_funcs; 
};

struct lua_library_holder_t {
	lua_library_holder_t* m_next;
	size_t m_size; // only correct on the first
	lua_library_t* m_library;
};

class lua_class_t {
public:
	virtual void set_metamethods() = 0;
	const char* m_class_name;
	const char* m_base_class; // check null!
	char m_pad[4];
	CUtlVector<lua_library_func_t*>* m_lib_funcs; // why in the flying fuck is this a pointer?
	char m_pad2[28];
};

struct lua_class_holder_t {
	CUtlVector<lua_class_t*> m_classes;
};
