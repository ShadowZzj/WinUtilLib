#include "File.h"
#include "HandleHelper.h"

namespace zzj
{
bool zzj::FileProperty::_queryValue(const std::string &valueName, const std::string &moduleName, std::string &RetStr)
{
    bool bSuccess         = false;
    BYTE *m_lpVersionData = nullptr;
    DWORD m_dwLangCharset = 0;
    CHAR *tmpstr          = nullptr;
    do
    {
        if (valueName.empty() || moduleName.empty())
        {
            break;
        }

        DWORD dwHandle;
        DWORD dwDataSize = ::GetFileVersionInfoSizeA((LPCSTR)moduleName.c_str(), &dwHandle);
        if (dwDataSize == 0)
        {
            break;
        }

        m_lpVersionData = new (std::nothrow) BYTE[dwDataSize];
        if (nullptr == m_lpVersionData)
        {
            break;
        }

        if (!::GetFileVersionInfoA((LPCSTR)moduleName.c_str(), dwHandle, dwDataSize, (void *)m_lpVersionData))
        {
            break;
        }

        UINT nQuerySize;
        DWORD *pTransTable;
        if (!::VerQueryValueA(m_lpVersionData, "\\VarFileInfo\\Translation", (void **)&pTransTable, &nQuerySize))
        {
            break;
        }

        m_dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));
        if (nullptr == m_lpVersionData)
        {
            break;
        }

        tmpstr = new (std::nothrow) CHAR[128];
        if (nullptr == tmpstr)
        {
            break;
        }

        sprintf_s(tmpstr, 128, "\\StringFileInfo\\%08lx\\%s", m_dwLangCharset, valueName.c_str());
        LPVOID lpData;
        if (!::VerQueryValueA((void *)m_lpVersionData, tmpstr, &lpData, &nQuerySize))
        {
            break;
        }
        RetStr   = (char *)lpData;
        bSuccess = true;
    } while (false);

    if (m_lpVersionData)
    {
        delete[] m_lpVersionData;
        m_lpVersionData = nullptr;
    }
    if (tmpstr)
    {
        delete[] tmpstr;
        tmpstr = nullptr;
    }
    return bSuccess;
}

 FileProperty::FileProperty(const std::string &imageName): m_imageName(imageName)
{

}

 FileProperty::~FileProperty()
{
}

//获取文件描述
std::string FileProperty::GetFileDescription()
{
    std::string tempStr;
    if (_queryValue("FileDescription", m_imageName, tempStr))
        return tempStr;
    else
        return "";
}

//获取文件版本
std::string FileProperty::GetFileVersion()
{
    std::string tempStr;
    if (_queryValue("FileVersion", m_imageName, tempStr))
        return tempStr;
    else
        return "";
}

//获取内部名称
std::string FileProperty::GetInternalName()
{
    std::string tempStr;
    if (_queryValue("InternalName", m_imageName, tempStr))
        return tempStr;
    else
        return "";
}

//获取公司名称
std::string FileProperty::GetCompanyName()
{
    std::string tempStr;
    if (_queryValue("CompanyName", m_imageName, tempStr))
        return tempStr;
    else
        return "";
}

//获取版权
std::string FileProperty::GetLegalCopyright()
{
    std::string tempStr;
    if (_queryValue("LegalCopyright", m_imageName, tempStr))
        return tempStr;
    else
        return "";
}

//获取原始文件名
std::string FileProperty::GetOriginalFilename()
{
    std::string tempStr;
    if (_queryValue("OriginalFilename", m_imageName, tempStr))
        return tempStr;
    else
        return "";
}

//获取产品名称
std::string FileProperty::GetProductName()
{
    std::string tempStr;
    if (_queryValue("ProductName", m_imageName, tempStr))
        return tempStr;
    else
        return "";
}

//获取产品版本
std::string FileProperty::GetProductVersion()
{
    std::string tempStr;
    if (_queryValue("ProductVersion", m_imageName, tempStr))
        return tempStr;
    else
        return "";
}

FileInfo::FileInfo(const std::string &imageName) : m_imageName(imageName)
{
}

FileInfo::~FileInfo()
{
}

 std::int64_t FileInfo::GetFileSize()
{
    if (m_imageName.empty())
        return -1;

    ScopeKernelHandle fileHandle = CreateFileA(m_imageName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                                               FILE_ATTRIBUTE_NORMAL, NULL);
    if (fileHandle == INVALID_HANDLE_VALUE)
        return -2;

    return ::GetFileSize(fileHandle, NULL);
}

File::File(const std::string &imageName) : m_imageName(imageName)
{
}

File::~File()
{
}

std::unique_ptr<zzj::FileProperty> File::GetFileProperty()
{
    return std::make_unique<zzj::FileProperty>(m_imageName);
}

std::unique_ptr<zzj::FileInfo> File::GetFileInfo()
{
    return std::make_unique<zzj::FileInfo>(m_imageName);
}


} // namespace zzj