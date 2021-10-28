#include "FileHelper.h"
#include "HandleHelper.h"
#include <tchar.h>
#include <windows.h>
#include <sys/stat.h>
#include <string>
#include <algorithm>
using namespace zzj;

bool FileHelper::ReadFileAtOffset(std::string fileName, void *buffer, unsigned long numToRead, unsigned long fileOffset)
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

#pragma warning(disable : 4996)
bool zzj::FileHelper::IsFileExist(std::string fileName)
{
    if (FILE *file = fopen(fileName.c_str(), "r"))
    {
        fclose(file);
        return true;
    }
    else
        return false;
}

int zzj::FileHelper::RemoveDirectoryRecursive(std::string path)
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

std::string zzj::FileHelper::GetExecutablePath()
{
    char current_proc_path[MAX_PATH] = {0};
    ::GetModuleFileNameA(NULL, current_proc_path, MAX_PATH);

    std::string exePath;
    exePath.append(current_proc_path);
    int separator_pos = exePath.rfind('\\');

    if (std::string::npos == separator_pos)
    {
        exePath = "";
    }
    else
    {
        exePath = exePath.substr(0, separator_pos);
    }
    return exePath;
}

std::string zzj::FileHelper::GetDllPath(void *dllAnyFunctionAddress)
{
    char path[MAX_PATH];
    HMODULE hm      = NULL;
    std::string ret = "";
    int separator_pos;

    if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (LPCSTR)dllAnyFunctionAddress, &hm) == 0)
    {
        ret = "";
        goto exit;
    }
    if (GetModuleFileNameA(hm, path, sizeof(path)) == 0)
    {
        ret = "";
        goto exit;
    }

    ret.append(path);
    separator_pos = ret.rfind('\\');
    if (std::string::npos == separator_pos)
    {
        ret = "";
    }
    else
    {
        ret = ret.substr(0, separator_pos);
    }
exit:
    return ret;
}

std::unique_ptr<zzj::File> zzj::FileHelper::GetFileInstance(const std::string &imagePath)
{
    return std::make_unique<zzj::File>(imagePath);
}
