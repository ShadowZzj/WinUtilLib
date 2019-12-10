#pragma once
#include <Windows.h>
#include <string>
class Process {
public:
	static const int INVALID_VAL = -1;
	DWORD GetCurrentSessionId();
	DWORD GetCurrentProcessId();
	static DWORD GetSessionId(DWORD processId);
	static DWORD GetProcessId(std::wstring processName);
	static DWORD GetProcessId(HANDLE processHandle);
private:
	bool IsValid(DWORD val) {
		return val != INVALID_VAL;
	}
};