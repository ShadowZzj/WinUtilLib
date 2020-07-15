#include "PEhelper.h"
#include "HandleHelper.h"
#include "FileHelper.h"
using namespace zzj;

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

PEFile::PEFile(std::string _fileName, Allocator* allocator)
{
	fileName = _fileName;
	if (!allocator)
		ThrowExceptionIfFail(ErrorCode::INVALID_PARAMETER);

	this->allocator = allocator;
	ErrorCode error;
	error = LoadPE();
	ThrowExceptionIfFail(error);
}

PEFile::ErrorCode PEFile::LoadPE()
{
	ErrorCode error;
	if (fileName.empty())
		return ErrorCode::INVALID_FILE_NAME;

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

	fileSize = File::GetFileSize(fileName);
	if (fileSize == -1)
		return ErrorCode::ZZJ_LIB_ERROR;

	peImage = (char*)allocator->Alloc(AlignData(ntHeader->OptionalHeader.SizeOfImage,ntHeader->OptionalHeader.SectionAlignment));
	if (!peImage)
		return ErrorCode::ZZJ_LIB_ERROR;

	bool ret = File::ReadFileAtOffset(fileName, peImage, ntHeader->OptionalHeader.SizeOfHeaders, FILE_BEGIN);
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
		ret = File::ReadFileAtOffset(fileName,&peImage[sectionHeader[index].VirtualAddress], sectionHeader[index].SizeOfRawData,sectionHeader[index].PointerToRawData);
		if (!ret)
			return ErrorCode::ZZJ_LIB_ERROR;
	}

	return ErrorCode::SUCCESS;
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

	bool ret = File::ReadFileAtOffset(fileName, dosHeader, sizeof(*dosHeader), FILE_BEGIN);
	if (!ret)
		return ZZJ_LIB_ERROR;

	return ErrorCode::SUCCESS;
}

PEFile::ErrorCode zzj::PEFile::ReadNTHeader(std::string fileName, const IMAGE_DOS_HEADER& dosHeader, IMAGE_NT_HEADERS* ntHeader)
{
	if (!ntHeader)
		return ErrorCode::INVALID_PARAMETER;
	bool ret = File::ReadFileAtOffset(fileName, ntHeader, sizeof(*ntHeader), dosHeader.e_lfanew);
	if (!ret)
		return ZZJ_LIB_ERROR;

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

