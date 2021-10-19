#include <Windows.h>
#include <wtsapi32.h>
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

HANDLE zzj::Thread::CreateOwnerMutex(const char *mutexName)
{
    HANDLE hMutex = CreateMutexA(NULL, true, mutexName);
    return hMutex;
}

int zzj::Thread::CloseMutex(HANDLE hMutex)
{
    ::CloseHandle(hMutex);
    return 0;
}

HANDLE zzj::Thread::WaitForMutex(const char *mutexName)
{
    std::string globalMutexName = "Global\\";
    globalMutexName += mutexName;
    HANDLE hMutex = CreateMutexA(NULL, false, globalMutexName.c_str());
    DWORD reason = WaitForSingleObject(hMutex,INFINITE);
    if (WAIT_OBJECT_0 == reason)
    {
    } 
    if (WAIT_TIMEOUT == reason)
    {
    }

    return hMutex;
}

int zzj::Thread::RealeaseMutex(HANDLE hMutex)
{
    ReleaseMutex(hMutex);
    return 0;
}

HANDLE zzj::Thread::ImpersonateCurrentUser()
{
    DWORD dwSessionID   = WTSGetActiveConsoleSessionId();
    HANDLE hToken;

    if (WTSQueryUserToken(dwSessionID, &hToken) == FALSE)
    {
        return NULL;
    }
    if (ImpersonateLoggedOnUser(hToken) == FALSE)
    {
        return NULL;
    }

    return hToken;
}

bool zzj::Thread::RevertToCurrentUser(HANDLE token)
{
    if (CloseHandle(token) == FALSE)
    {
        return false;
    }
    if (RevertToSelf() == FALSE)
    {
        return false;
    }
    return true;
}


