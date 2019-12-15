#pragma once
#include <windows.h>
#include <stdlib.h>
class PEInfo {
public:
	explicit PEInfo(HMODULE _module) {
		module = _module;
		LoadModule(module);
	}
	bool LoadModule(HMODULE Module);
	static HMODULE GetModuleHandle(const char* name);
	static HMODULE GetModuleHandle(const wchar_t* name);
	//if the code is in dll, you can use this function to get exe module
	static HMODULE GetEXEModuleHandle();
	//if the code is in dll, you can use this function to get dll module.
	static HMODULE GetCurrentModuleHandle();
private:
	HMODULE module;

};