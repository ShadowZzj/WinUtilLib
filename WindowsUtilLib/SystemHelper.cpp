#include "SystemHelper.h"
#include <wtsapi32.h>
#pragma comment(lib, "Wtsapi32.lib")

int zzj::SystemInfo::GetWindowsVersion()
{
    std::string vname;
    typedef void(__stdcall* NTPROC)(DWORD*, DWORD*, DWORD*);
    HINSTANCE hinst = LoadLibraryA("ntdll.dll");
    DWORD dwMajor, dwMinor, dwBuildNumber;
    NTPROC proc = (NTPROC)GetProcAddress(hinst, "RtlGetNtVersionNumbers");
    proc(&dwMajor, &dwMinor, &dwBuildNumber);

    if (dwMajor == 6 && dwMinor == 1)
        return 7;
    else if (dwMajor == 6 && dwMinor == 3)
        return 8;
    else if (dwMajor == 10 && dwMinor == 0)
        return 10;
    else return -1;
}

std::string zzj::SystemInfo::GetActiveConsoleUserName()
{
    std::string csUsrName    = "";
    std::string csDomainName = "";
    LPSTR szUserName        = NULL;
    LPSTR szDomainName      = NULL;
    DWORD dwLen              = 0;
    DWORD dwSessionId;

    dwSessionId = WTSGetActiveConsoleSessionId();

    if (0xffffffff == dwSessionId || 0 == dwSessionId)
        return "";

    BOOL bStatus =
        WTSQuerySessionInformationA(WTS_CURRENT_SERVER_HANDLE, dwSessionId, WTSDomainName, &szDomainName, &dwLen);

    if (bStatus)
    {
        csDomainName = szDomainName;
        bStatus = WTSQuerySessionInformationA(WTS_CURRENT_SERVER_HANDLE, dwSessionId, WTSUserName, &szUserName, &dwLen);
        if (bStatus)
        {
            csUsrName = szUserName;

            WTSFreeMemory(szUserName);
        }
        WTSFreeMemory(szDomainName);
    }
    return csUsrName;
}
