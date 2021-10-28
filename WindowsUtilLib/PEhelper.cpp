#include "PEhelper.h"
#include "HandleHelper.h"
#include "FileHelper.h"
using namespace zzj;


bool zzj::PEFile::FileProperty::_queryValue(const std::string &valueName, const std::string &moduleName, std::string &RetStr)
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

zzj::PEFile::FileProperty::FileProperty(const std::string &imageName) : m_imageName(imageName)
{
}

zzj::PEFile::FileProperty::~FileProperty()
{
}

//获取文件描述
std::string zzj::PEFile::FileProperty::GetFileDescription()
{
    std::string tempStr;
    if (_queryValue("FileDescription", m_imageName, tempStr))
        return tempStr;
    else
        return "";
}

//获取文件版本
std::string zzj::PEFile::FileProperty::GetFileVersion()
{
    std::string tempStr;
    if (_queryValue("FileVersion", m_imageName, tempStr))
        return tempStr;
    else
        return "";
}

//获取内部名称
std::string zzj::PEFile::FileProperty::GetInternalName()
{
    std::string tempStr;
    if (_queryValue("InternalName", m_imageName, tempStr))
        return tempStr;
    else
        return "";
}

//获取公司名称
std::string zzj::PEFile::FileProperty::GetCompanyName()
{
    std::string tempStr;
    if (_queryValue("CompanyName", m_imageName, tempStr))
        return tempStr;
    else
        return "";
}

//获取版权
std::string zzj::PEFile::FileProperty::GetLegalCopyright()
{
    std::string tempStr;
    if (_queryValue("LegalCopyright", m_imageName, tempStr))
        return tempStr;
    else
        return "";
}

//获取原始文件名
std::string zzj::PEFile::FileProperty::GetOriginalFilename()
{
    std::string tempStr;
    if (_queryValue("OriginalFilename", m_imageName, tempStr))
        return tempStr;
    else
        return "";
}

//获取产品名称
std::string zzj::PEFile::FileProperty::GetProductName()
{
    std::string tempStr;
    if (_queryValue("ProductName", m_imageName, tempStr))
        return tempStr;
    else
        return "";
}

//获取产品版本
std::string zzj::PEFile::FileProperty::GetProductVersion()
{
    std::string tempStr;
    if (_queryValue("ProductVersion", m_imageName, tempStr))
        return tempStr;
    else
        return "";
}

zzj::PEFile::FileProperty PEFile::GetFileProperty()
{
    return zzj::PEFile::FileProperty::FileProperty(fileName);
}
bool PEInfo::LoadModule(HMODULE Module)
{
	return false;
}

HMODULE PEInfo::GetModuleHandle(const char* name)
{
	return ::GetModuleHandleA(name);
}

HMODULE PEInfo::GetModuleHandle(const wchar_t* name)
{
	return ::GetModuleHandleW(name);
}

HMODULE PEInfo::GetEXEModuleHandle()
{
	return ::GetModuleHandle(NULL);
}

HMODULE PEInfo::GetCurrentModuleHandle()
{
	HMODULE ret;
	GetModuleHandleEx(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
		(PCTSTR)GetCurrentModuleHandle,
		&ret);

	return ret;
}

PEFile::PEFile(std::string _fileName, Allocator *allocator) : File(fileName)
{
	fileName = _fileName;
	if (!allocator)
		ThrowExceptionIfFail(ErrorCode::INVALID_PARAMETER);

	this->allocator = allocator;
	ErrorCode error;
	error = LoadPE();
	ThrowExceptionIfFail(error);
}

zzj::PEFile::~PEFile()
{
	UnloadPE();
}

PEFile::ErrorCode PEFile::LoadPE()
{
	ErrorCode error;
	if (fileName.empty())
		return ErrorCode::INVALID_FILE_NAME;

	UnloadPE();
	//dosHeader points to temp buffer.
	dosHeader = Allocator::Alloc<IMAGE_DOS_HEADER>(allocator,1);
	if (!dosHeader)
		return ErrorCode::ZZJ_LIB_ERROR;

	error = ReadDosHeader(fileName, dosHeader);
	ReturnIfFail(error);

	ntHeader = Allocator::Alloc<IMAGE_NT_HEADERS>(allocator, 1);
	if (!ntHeader)
		return ErrorCode::ZZJ_LIB_ERROR;

	error = ReadNTHeader(fileName, *dosHeader, ntHeader);
	ReturnIfFail(error);

	if (peImage) {
		allocator->Free(peImage);
		peImage = nullptr;
	}

	peImage = (char*)allocator->Alloc(AlignData(ntHeader->OptionalHeader.SizeOfImage,ntHeader->OptionalHeader.SectionAlignment));
	if (!peImage)
		return ErrorCode::ZZJ_LIB_ERROR;

	bool ret = FileHelper::ReadFileAtOffset(fileName, peImage, ntHeader->OptionalHeader.SizeOfHeaders, FILE_BEGIN);
	if (!ret)
		return ErrorCode::ZZJ_LIB_ERROR;

	allocator->Free(dosHeader);
	allocator->Free(ntHeader);

	//dosHeader points in pe image.

	dosHeader = (IMAGE_DOS_HEADER*)peImage;
	ntHeader = (IMAGE_NT_HEADERS*)(peImage + dosHeader->e_lfanew);
	sectionHeader = IMAGE_FIRST_SECTION(ntHeader);

	//loop read section.
	for (int index = 0;
		index< ntHeader->FileHeader.NumberOfSections;
		index++
		)
	{
        ret = FileHelper::ReadFileAtOffset(fileName, &peImage[sectionHeader[index].VirtualAddress],
                                           sectionHeader[index].SizeOfRawData, sectionHeader[index].PointerToRawData);
		if (!ret)
			return ErrorCode::ZZJ_LIB_ERROR;
	}

	error = ReadDataDirectoryTable(peImage, ntHeader, dataDirectoryTable);
	ReturnIfFail(error);

	error = ReadImportLibrary(peImage, dataDirectoryTable, importLibrary, numOfImportLibrary);
	ReturnIfFail(error);


	fileSize = FileHelper::GetFileInstance(fileName).GetFileInfo().GetFileSize();
	if (fileSize == -1)
		return ErrorCode::ZZJ_LIB_ERROR;

	IMAGE_SECTION_HEADER* lastSection = &sectionHeader[ntHeader->FileHeader.NumberOfSections-1];
	extraDataSize = fileSize - (lastSection->PointerToRawData + lastSection->SizeOfRawData);

	if (extraDataSize == 0)
		return ErrorCode::SUCCESS;

	if (extraData) {
		allocator->Free(extraData);
		extraData = nullptr;
	}

	extraData = (char*)allocator->Alloc(extraDataSize);
    ret       = FileHelper::ReadFileAtOffset(fileName, extraData, extraDataSize,
                                       lastSection->PointerToRawData + lastSection->SizeOfRawData);
	if (!ret) {
		allocator->Free(extraData);
		extraData = nullptr;
		return ErrorCode::ZZJ_LIB_ERROR;
	}

	return ErrorCode::SUCCESS;
}

PEFile::ErrorCode PEFile::UnloadPE()
{
	if (extraData) {
		allocator->Free(extraData);
		extraData == nullptr;
	}
	if (peImage) {
		allocator->Free(peImage);
		peImage = nullptr;
	}

	return ErrorCode::SUCCESS;
}

DWORD zzj::PEFile::RVAToVA(char* imageBase, DWORD rva)
{
	if (imageBase)
		return (DWORD)imageBase + rva;
	return -1;
}

PEFile::ErrorCode PEFile::IsValidPE(std::string fileName)
{
	return SUCCESS;
}

bool PEFile::IsOperationSucceed(ErrorCode error)
{
	if (error == SUCCESS)
		return true;
	return false;

}

PEFile::ErrorCode PEFile::ReadDosHeader(std::string fileName, IMAGE_DOS_HEADER* dosHeader)
{
	
	if (!dosHeader)
		return ErrorCode::INVALID_PARAMETER;

	bool ret = FileHelper::ReadFileAtOffset(fileName, dosHeader, sizeof(*dosHeader), FILE_BEGIN);
	if (!ret)
		return ZZJ_LIB_ERROR;

	return ErrorCode::SUCCESS;
}

PEFile::ErrorCode zzj::PEFile::ReadNTHeader(std::string fileName, const IMAGE_DOS_HEADER& dosHeader, IMAGE_NT_HEADERS* ntHeader)
{
	if (!ntHeader)
		return ErrorCode::INVALID_PARAMETER;
    bool ret = FileHelper::ReadFileAtOffset(fileName, ntHeader, sizeof(*ntHeader), dosHeader.e_lfanew);
	if (!ret)
		return ZZJ_LIB_ERROR;

	return ErrorCode::SUCCESS;
}

PEFile::ErrorCode zzj::PEFile::ReadDataDirectoryTable(char* imageBase,  IMAGE_NT_HEADERS* ntheader, IMAGE_DATA_DIRECTORY*& dataDirectoryTable)
{
	dataDirectoryTable = &ntheader->OptionalHeader.DataDirectory[0];
	return ErrorCode::SUCCESS;
}

PEFile::ErrorCode zzj::PEFile::ReadImportLibrary(char* imageBase, IMAGE_DATA_DIRECTORY* dataDirectoryTable, IMAGE_IMPORT_DESCRIPTOR*& importLibrary, DWORD& numOfImportLibrary)
{
	
	importLibrary = (IMAGE_IMPORT_DESCRIPTOR*)RVAToVA(imageBase,dataDirectoryTable[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	
	numOfImportLibrary = 0;
	while ((&importLibrary[numOfImportLibrary])->Name != 0)
		numOfImportLibrary++;

	return ErrorCode::SUCCESS;
}

DWORD zzj::PEFile::AlignData(DWORD data, DWORD alignment)
{
	DWORD remainder = data % alignment;
	DWORD quotient = data / alignment;
	if (remainder > 0)
		return alignment * (quotient + 1);
	return data;
}

