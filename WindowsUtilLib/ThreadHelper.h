#pragma once
#include "ProcessHelper.h"
#include <Windows.h>
namespace zzj {
	class Thread {
	public:

		//Set last error code of the current thread.
		static void SetCurrentLastError(DWORD err);
		//Used to get locale of the current thread, which is useful when using CompareString.
		static LCID GetCurrentThreadLocale();
		static HANDLE GetCurrentThreadHandle();
		static DWORD GetCurrentThreadId();
		static DWORD GetThreadID(HANDLE handle);
		static DWORD GetOwnerProcessId(HANDLE threadHandle);
        static HANDLE CreateOwnerMutex(const char *mutexName);
        static int CloseMutex(HANDLE hMutex);
        static HANDLE WaitForMutex(const char *mutexName);
        static int RealeaseMutex(HANDLE hMutex);

		static HANDLE ImpersonateCurrentUser();
        static bool RevertToCurrentUser(HANDLE token);
	private:

	};
};