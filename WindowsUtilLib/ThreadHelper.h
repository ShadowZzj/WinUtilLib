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
	private:

	};
};