#include "ProcessHelper.h"
#include <tlhelp32.h>
#include "BaseUtil.h"

DWORD Process::GetCurrentSessionId(){
	DWORD processId=GetCurrentProcessId();
	DWORD sessionId = GetSessionId(processId);
	return sessionId;
}

DWORD Process::GetCurrentProcessId(){
	return ::GetCurrentProcessId();
}

DWORD Process::GetSessionId(DWORD processId){
	DWORD sessionId;
	if (ProcessIdToSessionId(processId, &sessionId))
		return sessionId;
	else
		return INVALID_VAL;
}

DWORD Process::GetProcessId(std::wstring processName){
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot) {
		return NULL;
	}
	PROCESSENTRY32 pe = { sizeof(pe) };
	for (BOOL ret = Process32First(hSnapshot, &pe); ret; ret = Process32Next(hSnapshot, &pe)) {
		if (str::Equal(pe.szExeFile,processName.c_str())) {
			CloseHandle(hSnapshot);
			return pe.th32ProcessID;
		}
	}
	CloseHandle(hSnapshot);
	return 0;
}

DWORD Process::GetProcessId(HANDLE processHandle)
{
	DWORD ret = ::GetProcessId(processHandle);
	if (ret)
		return ret;
	else
		return INVALID_VAL;
}
