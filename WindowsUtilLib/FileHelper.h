#pragma once
#include <Windows.h>
#include <functional>
#include <map>
#include <string>
#include "File.h"

namespace zzj {
	class FileHelper {
      public:
		static bool ReadFileAtOffset(std::string fileName, void* buffer, unsigned long numToRead, unsigned long fileOffset);
        static bool IsFileExist(std::string fileName);
        static int RemoveDirectoryRecursive(std::string path);
        static std::string GetExecutablePath();
        static std::string GetDllPath(void *dllAnyFunctionAddress);
        static zzj::File GetFileInstance(const std::string &imagePath);
	};
}