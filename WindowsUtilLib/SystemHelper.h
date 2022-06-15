#pragma once
#include <Windows.h>
#include <string>
#include <optional>
namespace zzj
{
class SystemInfo
{
  public:
    class VersionInfo
    {
      public:
        int major;
        int minor;
        int buildnumber;
    };
    static std::optional<VersionInfo> GetWindowsVersion();
    static std::string GetActiveConsoleUserName();
    static std::string GetActiveConsoleSessionId();

};

} // namespace zzj