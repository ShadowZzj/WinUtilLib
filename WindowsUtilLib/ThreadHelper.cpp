#include <Windows.h>
#include "ThreadHelper.h"
#include "ProcessHelper.h"
using namespace zzj;

void Thread::SetCurrentLastError(DWORD err)
{
	return ::SetLastError(err);
}

LCID Thread::GetCurrentThreadLocale()
{
	return ::GetThreadLocale();
}

HANDLE zzj::Thread::GetCurrentThreadHandle()
{
	return GetCurrentThread();
}

DWORD zzj::Thread::GetCurrentThreadId()
{
	return ::GetCurrentThreadId();
}


DWORD zzj::Thread::GetThreadID(HANDLE handle)
{
	return GetThreadId(handle);
}

DWORD zzj::Thread::GetOwnerProcessId(HANDLE threadHandle)
{
	DWORD processID=GetProcessIdOfThread(threadHandle);
	return processID;
}


