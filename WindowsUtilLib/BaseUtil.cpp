#include "CRTdbgHeader.h"
#include "BaseUtil.h"
void* Allocator::Alloc(Allocator* allocator, size_t size){
	if (!allocator) {
		void* res = malloc(size);
		if (!res)
			return nullptr;   
		memset(res, 0, size);
		return res;
	}
	else {
		return allocator->Alloc(size);
	}
}

void* Allocator::ReAlloc(Allocator* allocator, void* mem, size_t size){
	if (!allocator) {
		return realloc(mem,size);
	}
	else {
		return allocator->ReAlloc(mem, size);
	}

}

void Allocator::Free(Allocator* allocator,void* mem){
	if (!allocator) {
		free(mem);
	}
	else {
		allocator->Free(mem);
	}
}

void* Allocator::MemDup(Allocator* a, const void* mem, size_t size, size_t padding){
	void* dstMem = Alloc(a, size + padding);
	if (!dstMem)
		return nullptr;
	memcpy(dstMem, mem, size);
	memset((LPBYTE)dstMem + size, 0, 2);
	return dstMem;
}
char* Allocator::StrDup(Allocator* a, const char* str){
	if (str == nullptr)
		return nullptr;
	size_t size=strlen(str);
	//attention +1
	return (char*)MemDup(a, str, size+1);
}
#ifdef OS_WIN
wchar* Allocator::StrDupW(Allocator* a, const wchar* str){
	if (str == nullptr)
		return nullptr;
	size_t size = wcslen(str)+1;
	//attention +1
	return (char*)MemDup(a, str, size*2);
}
#endif // OS_WIN

OwnedData::OwnedData(const char* data, size_t size) {
	if (size == 0) {
		size = str::Len(data);
	}
	this->data = (char*)data;
	this->size = size;
}

OwnedData::~OwnedData() {
	free(data);
}

OwnedData::OwnedData(OwnedData&& other) {
	CrashIf(this == &other);
	this->data = other.data;
	this->size = other.size;
	other.data = nullptr;
	other.size = 0;
}

OwnedData& OwnedData::operator=(OwnedData&& other) {
	CrashIf(this == &other);
	this->data = other.data;
	this->size = other.size;
	other.data = nullptr;
	other.size = 0;
	return *this;
}

bool OwnedData::IsEmpty() {
	return (data == nullptr) || (size == 0);
}

void OwnedData::Clear() {
	free(data);
	data = nullptr;
	size = 0;
}

char* OwnedData::Get() const {
	return data;
}


void OwnedData::TakeOwnership(const char* s, size_t size) {
	if (size == 0) {
		size = str::Len(s);
	}
	free(data);
	data = (char*)s;
	this->size = size;
}

OwnedData OwnedData::MakeFromStr(const char* s, size_t size) {
	if (size == 0) {
		return OwnedData(str::Dup(s), str::Len(s));
	}

	char* tmp = str::DupN(s, size);
	return OwnedData(tmp, size);
}

char* OwnedData::StealData() {
	auto* res = data;
	data = nullptr;
	size = 0;
	return res;
}


void* memdup(const void* data, size_t len) {
	void* dup = malloc(len);
	if (dup) {
		memcpy(dup, data, len);
	}
	return dup;
}