#include "SystemHelper.h"
#include <wtsapi32.h>
#include <sddl.h>
#pragma comment(lib, "Wtsapi32.lib")

std::optional<zzj::SystemInfo::VersionInfo> zzj::SystemInfo::GetWindowsVersion()
{
    std::string vname;
    typedef void(__stdcall * NTPROC)(DWORD *, DWORD *, DWORD *);
    HINSTANCE hinst = LoadLibraryA("ntdll.dll");
    DWORD dwMajor, dwMinor, dwBuildNumber;
    NTPROC proc = (NTPROC)GetProcAddress(hinst, "RtlGetNtVersionNumbers");
    if (!proc)
        return {};
    proc(&dwMajor, &dwMinor, &dwBuildNumber);
    return VersionInfo{(int)dwMajor, (int)dwMinor, (int)dwBuildNumber};
    
}

std::string zzj::SystemInfo::GetActiveConsoleUserName()
{
    std::string csUsrName    = "";
    std::string csDomainName = "";
    LPSTR szUserName         = NULL;
    LPSTR szDomainName       = NULL;
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

std::string zzj::SystemInfo::GetActiveConsoleSessionId()
{
    DWORD sessionId = WTSGetActiveConsoleSessionId();
    if (-1 == sessionId)
        return "";
    return std::to_string(sessionId);
}
