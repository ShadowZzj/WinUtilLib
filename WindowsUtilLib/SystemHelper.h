#pragma once
#include <Windows.h>
#include <string>
namespace zzj
{
	class SystemInfo
	{
	public:
		static int GetWindowsVersion();
		static std::string GetActiveConsoleUserName();
	private:

	};


}