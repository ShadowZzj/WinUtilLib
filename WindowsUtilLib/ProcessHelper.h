#pragma once
#include <Windows.h>
#include <string>
#include <map>
#include "zzjErrorEnum.h"
#include "BaseUtil.h"
#include "HandleHelper.h"
namespace zzj {
	class EnvHelper {
	public:
		EnvHelper() {
			RefreshEnv();
		}
		void RefreshEnv();
		DWORD GetEnvVariable(const char* key, char* value, size_t len);
		DWORD GetEnvVariable(const wchar_t* key, wchar_t* value, size_t len);
		std::map<std::string, std::string> GetEnvVariables() {
			return env;
		}
		DWORD ExpandEnvVariable(const char* key, char* value, size_t len);
		DWORD ExpandEnvVariable(const wchar_t* key, wchar_t* value, size_t len);
		//if exist, modify. If nonexist, add. If value==NULL, delete.
		BOOL AddEnvVariableNoRefresh(const char* key, const char* value);
		BOOL AddEnvVariableNoRefresh(const wchar_t* key, const wchar_t* value);
		BOOL AddEnvVariable(const char* key, const char* value);
		BOOL AddEnvVariable(const wchar_t* key, const wchar_t* value);


	private:
		std::map<std::string, std::string> env;
	};

	class Process {
	public:
		static const int INVALID_VAL = -1;
		DWORD GetSessionId();
		DWORD GetProcessId();
		HANDLE GetProcessHandle();
		DWORD GetProcessDirectory(size_t len, char* buf);
		DWORD GetProcessDirectory(size_t len, wchar_t* buf);

		bool SetProcessDirectory(const char* dir);
		bool SetProcessDirectory(const wchar_t* dir);
		bool BindProcess(HANDLE handle);
		bool BindProcess(DWORD processId, DWORD deriredAccess);
		static DWORD GetSessionId(DWORD processId);
		static HANDLE GetProcessHandle(DWORD processId, DWORD desiredAccess);
		static DWORD GetProcessId(std::wstring processName);
		static DWORD GetProcessId(HANDLE processHandle);
		Process(){
			process=::GetCurrentProcess();
			processId = ::GetCurrentProcessId();
		}
		Process(HANDLE processHandle) {
			InitWithHandle(processHandle);
		}
		Process(DWORD _processId,DWORD deriredAccess) {
			if (!InitWithId(_processId, deriredAccess))
				CrashMe();
		}
		operator HANDLE() {
			return process;
		}
		BAN_COPY(Process)

		EnvHelper envHelper;
	private:
		bool InitWithHandle(HANDLE processHandle) {
			process = processHandle;
			processId = GetProcessId(processHandle);
			return true;
		}
		bool InitWithId(DWORD _processId,DWORD desiredAccess) {
			HANDLE processHandle = OpenProcess(desiredAccess, FALSE, _processId);
			if (processHandle == INVALID_HANDLE_VALUE)
				return false;
			process = processHandle;
			processId = _processId;
			return true;
		}
		bool IsValid() {
			return process != INVALID_HANDLE_VALUE&&processId!=INVALID_VAL;
		}
		ScopeKernelHandle process=INVALID_HANDLE_VALUE;
		DWORD processId = INVALID_VAL;
	};

}