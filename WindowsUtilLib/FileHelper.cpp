#include "FileHelper.h"
#include "HandleHelper.h"
#include <tchar.h>
#include <windows.h>
#include <sys/stat.h>
#include <string>
#include <algorithm>
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
#pragma warning(disable : 4996)
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

int zzj::File::RemoveDirectoryRecursive(std::string path)
{
    std::string str(path);
    if (!str.empty())
    {
        while (*str.rbegin() == '\\' || *str.rbegin() == '/')
        {
            str.erase(str.size() - 1);
        }
    }
    replace(str.begin(), str.end(), '/', '\\');

    struct stat sb;
    if (stat((char *)str.c_str(), &sb) != 0)
    {
        return 0;
    }

    if(S_IFDIR &&sb.st_mode)
    {
        HANDLE hFind;
        WIN32_FIND_DATAA FindFileData;

        char DirPath[MAX_PATH];
        char FileName[MAX_PATH];

        strcpy(DirPath, (char *)str.c_str());
        strcat(DirPath, "\\*");
        strcpy(FileName, (char *)str.c_str());
        strcat(FileName, "\\");

        hFind = FindFirstFileA(DirPath, &FindFileData);
        if (hFind == INVALID_HANDLE_VALUE)
            return -1;
        strcpy(DirPath, FileName);

        bool bSearch = true;
        while (bSearch)
        {
            if (FindNextFileA(hFind, &FindFileData))
            {
                if (!(strcmp(FindFileData.cFileName, ".") && strcmp(FindFileData.cFileName, "..")))
                    continue;
                strcat(FileName, FindFileData.cFileName);
                if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                {
                    if (0!= RemoveDirectoryRecursive(FileName))
                    {
                        FindClose(hFind);
                        return -2;
                    }
                    RemoveDirectoryA(FileName);
                    strcpy(FileName, DirPath);
                }
                else
                {
                    //if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
                    //    _chmod(FileName, _S_IWRITE);

                    if (!DeleteFileA(FileName))
                    {
                        FindClose(hFind);
                        return -3;
                    }
                    strcpy(FileName, DirPath);
                }
            }
            else
            {
                if (GetLastError() == ERROR_NO_MORE_FILES)
                    bSearch = false;
                else
                {
                    FindClose(hFind);
                    return -4;
                }
            }
        }
        FindClose(hFind);
        
        return (int)(RemoveDirectoryA((char *)str.c_str()) == FALSE);
    }
    else
    {
        return -5;
    }
}
