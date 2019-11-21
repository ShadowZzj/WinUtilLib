#include <Windows.h>
#include "ThreadHelper.h"
void ThreadHelper::SetLastError(DWORD err)
{
	return ::SetLastError(err);
}

LCID ThreadHelper::GetThreadLocale()
{
	return ::GetThreadLocale();
}
