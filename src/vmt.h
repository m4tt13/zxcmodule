#pragma once

#include <Windows.h>
#include <cstdint>

namespace vmt {
	static inline void** get(void* obj) {
		return *reinterpret_cast<void***>(obj);
	}

	template <typename T>
	static inline T get(void* obj, std::uintptr_t index) {
		return reinterpret_cast<T>(get(obj)[index]);
	}

	template <typename Ret, typename ...Args>
	static inline Ret call(void* obj, std::uintptr_t index, Args ...args) {
		return reinterpret_cast<Ret(__thiscall*)(const void*, decltype(args)...)>(get(obj)[index])(obj, args...);
	}

	template<typename T>
	T* getReal(uintptr_t address, int index, uintptr_t offset) {
		uintptr_t step = 3;
		uintptr_t instructionSize = 7;
		uintptr_t instruction = ((*(uintptr_t**)(address))[index] + offset);
		 
		uintptr_t relativeAddress = *(DWORD*)(instruction + step);
		uintptr_t realAddress = instruction + instructionSize + relativeAddress;
		return *(T**)(realAddress);
	}

	template <typename Fn>
	static bool hookVmt(void** vmt, Fn* oldFunc, const void* newFunc, std::uintptr_t index) {
		Fn* funcAddr = reinterpret_cast<Fn*>(vmt + index);
		if (oldFunc)
			*oldFunc = *funcAddr;

		DWORD oldProt;
		if (!VirtualProtect(funcAddr, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProt))
			return false;
	 
		memcpy(funcAddr, &newFunc, sizeof(void*));

		DWORD temp;
		return VirtualProtect(funcAddr, sizeof(void*), oldProt, &temp);
	}

	template <typename Fn>
	static bool hook(void* obj, Fn* oldFunc, const void* newFunc, std::uintptr_t index) {
		return hookVmt(get(obj), oldFunc, newFunc, index);
	}
}
