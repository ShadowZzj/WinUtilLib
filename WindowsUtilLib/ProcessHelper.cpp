#include "ProcessHelper.h"
#include <tlhelp32.h>
#include "BaseUtil.h"
#include <iostream>
#include <strsafe.h>
#include "ThreadHelper.h"
using namespace zzj;

DWORD Process::GetSessionId() {
	DWORD processId = GetProcessId();
	DWORD sessionId = GetSessionId(processId);
	return sessionId;
}

DWORD Process::GetProcessId() {
	return processId;
}

HANDLE Process::GetProcessHandle() {
	return process;
}
DWORD Process::GetSessionId(DWORD processId) {
	DWORD sessionId;
	if (ProcessIdToSessionId(processId, &sessionId))
		return sessionId;
	else
		return INVALID_VAL;
}

HANDLE zzj::Process::GetProcessHandle(DWORD processId,DWORD desiredAccess)
{ 
	return OpenProcess(desiredAccess, FALSE, processId);

}

DWORD Process::GetProcessId(std::wstring processName) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot) {
		return NULL;
	}
	PROCESSENTRY32 pe = { sizeof(pe) };
	for (BOOL ret = Process32First(hSnapshot, &pe); ret; ret = Process32Next(hSnapshot, &pe)) {
		if (str::Equal(pe.szExeFile, processName.c_str())) {
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

DWORD Process::GetProcessDirectory(size_t len, char* buf) {
	if (!IsValid())
		return GetCurrentDirectoryA(len, buf);
	else {
		//TODO
		return INVALID_VAL;
	}
}

DWORD Process::GetProcessDirectory(size_t len, wchar_t* buf) {
	if (!IsValid())
		return GetCurrentDirectoryW(len, buf);
	else {
		//TODO
		return INVALID_VAL;
	}
}

bool Process::SetProcessDirectory(const char* dir) {
	if (!IsValid())
		return SetCurrentDirectoryA(dir);
	else {
		//TODO
		return INVALID_VAL;
	}
}
bool Process::SetProcessDirectory(const wchar_t* dir) {
	if (!IsValid())
		return SetCurrentDirectoryW(dir);
	else {
		//TODO
		return INVALID_VAL;
	}
}

bool zzj::Process::BindProcess(HANDLE handle)
{
	return InitWithHandle(handle);
}

bool zzj::Process::BindProcess(DWORD processId, DWORD deriredAccess)
{
	return InitWithId(processId, deriredAccess);
}

void EnvHelper::RefreshEnv() {
	env.clear();
	wchar_t* envStr = GetEnvironmentStringsW();
	wchar_t name[MAX_PATH]{};
	wchar_t value[MAX_PATH]{};
	wchar_t* current = envStr;
	wchar_t* pos;
	HRESULT hr = S_OK;

	while (current != NULL) {
		if (*current != L'=') {
			//pos points to value
			pos = (wchar_t*)str::Find(current, L"=");
			pos++;

			size_t nameLen = (size_t)pos - (size_t)current - sizeof(wchar_t);
			hr = StringCbCopyNW(name, MAX_PATH, current, nameLen);
			if (FAILED(hr))
				break;
			char* pName = str::Wstr2Str(name);
			defer{ Allocator::Free(nullptr,pName); };
			std::string nameStr = pName;
			
			hr = StringCchCopyNW(value, MAX_PATH, pos, str::Len(pos) + 1);
			if (FAILED(hr))
				break;

			char* pValue= str::Wstr2Str(value);
			std::string valueStr;
			if (pValue) {
				valueStr = pValue;
				env[nameStr] = valueStr;
			}
		}
		while (*current != L'\0')
			current++;
		current++;
		if (*current == L'\0')
			break;
	}
	FreeEnvironmentStringsW(envStr);

}

DWORD EnvHelper::GetEnvVariable(const char* key, char* value, size_t len) {
	DWORD requireLen = GetEnvironmentVariableA(key, value, 0);
	if (len <= requireLen)
		return requireLen;

	GetEnvironmentVariableA(key, value, len);
	return 0;
}



DWORD EnvHelper::GetEnvVariable(const wchar_t* key, wchar_t* value, size_t len) {
	DWORD requireLen = GetEnvironmentVariableW(key, value, 0);
	if (len <= requireLen)
		return requireLen;

	GetEnvironmentVariableW(key, value, len);
	return 0;
}

DWORD EnvHelper::ExpandEnvVariable(const char* key, char* value, size_t len) {
	DWORD requireLen = ExpandEnvironmentStringsA(key, NULL, 0);
	if (len <= requireLen)
		return requireLen;

	ExpandEnvironmentStringsA(key, value, len);
	return 0;
}

DWORD EnvHelper::ExpandEnvVariable(const wchar_t* key, wchar_t* value, size_t len) {
	DWORD requireLen = ExpandEnvironmentStringsW(key, NULL, 0);
	if (len <= requireLen)
		return requireLen;

	ExpandEnvironmentStringsW(key, value, len);
	return 0;
}

BOOL EnvHelper::AddEnvVariableNoRefresh(const char* key, const char* value) {
	return SetEnvironmentVariableA(key, value);
}
BOOL EnvHelper::AddEnvVariableNoRefresh(const wchar_t* key, const wchar_t* value) {
	return SetEnvironmentVariableW(key, value);
}
BOOL EnvHelper::AddEnvVariable(const char* key, const char* value) {
	bool ret = AddEnvVariableNoRefresh(key, value);
	RefreshEnv();
	return ret;
}
BOOL EnvHelper::AddEnvVariable(const wchar_t* key, const wchar_t* value) {
	bool ret = AddEnvVariableNoRefresh(key, value);
	RefreshEnv();
	return ret;
}

