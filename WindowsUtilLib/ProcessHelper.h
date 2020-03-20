#pragma once
#include <Windows.h>
#include <string>
#include <map>
#include "zzjErrorEnum.h"
#include "BaseUtil.h"
#include "HandleHelper.h"
#include <TlHelp32.h>
#include <list>

#define DISALLOW_COPY_AND_ASSIGN(TypeName)	\
	TypeName(const TypeName&);	\
	TypeName& operator=(const TypeName&);


namespace zzj {
	struct ProcessEntry
	{
		DWORD ProcessId;
		DWORD ParentProcessId;
		DWORD ThreadCount;
		LONG ThreadPriorityBase;

		std::wstring ExeName;
		std::wstring ExeFullPath;
	};

	/*
	TokenElevationTypeDefault
	built-in admin:elevated
	guest or account not in admin group : limited token and no linked token

	TokenElevationTypeLimited,TokenElevationTypeFull
	usually account in admin group
	has linked token in same user

	see https://stackoverflow.com/questions/50562419/determine-if-user-can-elevate-a-process-with-c
	*/
	struct ActiveExplorerInfo
	{
		DWORD ProcessId;
		DWORD SessionId;

		bool IsElevated;
		bool IsElevated_LinkedToken;
		TOKEN_ELEVATION_TYPE ElevationType;
		TOKEN_ELEVATION_TYPE ElevationType_LinkedToken;
		std::wstring UserSid;
		std::wstring UserName;
		std::wstring DomainName;
	};
	class ProcessIterator
	{
	public:

		typedef std::list<ProcessEntry> ProcessEntries;

		ProcessIterator();
		virtual ~ProcessIterator();

		bool SnapshotAll(ProcessEntries& entries);
		bool SnapshotFilterExeName(ProcessEntries& entries, const wchar_t* exename);

		static bool FindEntryByPid(const ProcessEntries& entries, DWORD pid, ProcessEntry** entry);

	private:

		bool GetFirstEntry(ProcessEntries& entries);
		void InitEntry(PROCESSENTRY32W* pe32);
		void ConvertPE32ToPE(PROCESSENTRY32W* pe32, ProcessEntry* pe);

		HANDLE snapshot_handle;


		DISALLOW_COPY_AND_ASSIGN(ProcessIterator)
	};
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
		static bool  IsMutexExist(std::string mutex);
		static bool GetActiveExplorerInfo(ActiveExplorerInfo* pinfo);
		//System process create user or admin process.
		static DWORD SystemCreateProcess(std::wstring& commandLine, bool bElevated, bool bWait, DWORD dwWaitTime, bool show);
		//Same level as caller.
		static BOOL RegularCreateProcess(std::string path, bool show, std::string cmdLine = "",bool wait=false);
		//Require uac if user process.
		static BOOL AdminCreateProcess(const char* pszFileName, bool show, const char* param);
		static bool KillProcess(DWORD pid);
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