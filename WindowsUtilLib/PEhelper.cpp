#include "PEhelper.h"

bool PEInfo::LoadModule(HMODULE Module)
{
	return false;
}

HMODULE PEInfo::GetModuleHandle(const char* name)
{
	return ::GetModuleHandleA(name);
}

HMODULE PEInfo::GetModuleHandle(const wchar_t* name)
{
	return ::GetModuleHandleW(name);
}

HMODULE PEInfo::GetEXEModuleHandle()
{
	return ::GetModuleHandle(NULL);
}

HMODULE PEInfo::GetCurrentModuleHandle()
{
	HMODULE ret;
	GetModuleHandleEx(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
		(PCTSTR)GetCurrentModuleHandle,
		&ret);

	return ret;
}
