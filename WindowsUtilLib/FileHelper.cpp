#include "FileHelper.h"
#include "HandleHelper.h"
#include <tchar.h>

using namespace zzj;

bool File::ReadFileAtOffset(std::string fileName, void* buffer, unsigned long numToRead, unsigned long fileOffset)
{

	if (fileName.empty()||!buffer||fileOffset<0)
		return false;

	ScopeKernelHandle fileHandle = CreateFileA(fileName.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if (fileHandle == INVALID_HANDLE_VALUE)
		return false;

	DWORD bytesRead;
	bool bRet;

	DWORD dwRet = SetFilePointer(fileHandle, fileOffset, NULL, FILE_BEGIN);
	if (dwRet == INVALID_SET_FILE_POINTER)
		return false;

	bRet = ReadFile(fileHandle,
		buffer,
		numToRead,
		&bytesRead,
		NULL);
	if (!bRet)
		return false;
	if (numToRead != bytesRead)
		return false;

	return true;
}

DWORD zzj::File::GetFileSize(std::string fileName)
{
	if (fileName.empty())
		return -1;

	ScopeKernelHandle fileHandle = CreateFileA(fileName.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if (fileHandle == INVALID_HANDLE_VALUE)
		return -1;

	return ::GetFileSize(fileHandle, NULL);
}

bool zzj::File::IsFileExist(std::string fileName)
{
    if (FILE *file = fopen(fileName.c_str(), "r"))
    {
        fclose(file);
        return true;
    }
    else
        return false;
}
