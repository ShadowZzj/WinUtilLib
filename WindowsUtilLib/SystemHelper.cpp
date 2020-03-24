#include "SystemHelper.h"

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
