#pragma once
#include <Windows.h>
#include <string>
namespace zzj {
	class File {
	public:
		static bool ReadFileAtOffset(std::string fileName, void* buffer, unsigned long numToRead, unsigned long fileOffset);
		static DWORD GetFileSize(std::string fileName);
	};
}