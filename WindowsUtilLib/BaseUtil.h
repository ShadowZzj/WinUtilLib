#pragma once
#include "CRTdbgHeader.h"
#include <malloc.h>
#include <Windows.h>
#include <iostream>
#include <string>
#include <functional>
#include <vector>


#pragma warning(push)
#pragma warning(disable:6011)
inline void CrashMe() {
	char* p = nullptr;
	*p = 0;
}
#pragma warning(pop)


#define CrashIf(cond) \
if(cond)\
  CrashMe();

#define CONCAT_INTERNAL(x, y) x##y

#define CONCAT(x, y) CONCAT_INTERNAL(x, y)

#define dimof1(array) (sizeof(DimofSizeHelper(array)))

#define dimof2(array)  GetDim(array)

//simulate Golang defer keyword£¬wrap ExitScope with lambda function£¬when exits its scope, the deconstruct function is called.
#define defer const auto& CONCAT(defer__, __LINE__) = ExitScopeHelp() + [&]()


template <typename T>
struct ExitScope {
	T lambda;
	ExitScope(T lambda) : lambda(lambda) {}
	~ExitScope() { lambda(); }
	ExitScope(const ExitScope&);

private:
	ExitScope& operator=(const ExitScope&);
};

class Allocator
{
public:
	Allocator() {}
	~Allocator() {}
	//public interface for redesign malloc\realloc\free.
	virtual void* Alloc(size_t size) = 0;
	virtual void* ReAlloc(void* mem, size_t size) = 0;
	virtual void Free(void* mem) = 0;

	//fallback to malloc\realloc\free
	static void* Alloc(Allocator* allocator, size_t size);
	static void* ReAlloc(Allocator* allocator, void* mem, size_t size);
	static void Free(Allocator* allocator, void* mem);

	//helper template to specify special type.
	template<typename T>
	static T* Alloc(Allocator* allocator, int num) {
		if (!allocator) {
			return (T*)Alloc(NULL, sizeof(T) * num);
		}

		return (T*)allocator->Alloc(num * sizeof(T));
	}

	static void* MemDup(Allocator* a, const void* mem, size_t size, size_t padding = 0);
	static char* StrDup(Allocator* a, const char* str);
#ifdef OS_WIN
	static wchar* StrDupW(Allocator* a, const wchar* str);
#endif // OS_WIN

private:

};

class ExitScopeHelp {
public:
	template <typename T>
	ExitScope<T> operator+(T t) {
		return t;
	}
	~ExitScopeHelp(){}
};

class OwnedData {
public:
	char* data = nullptr;
	size_t size = 0;

	OwnedData() {}
	// takes ownership of data
	OwnedData(const char* data, size_t size = 0);
	~OwnedData();

	OwnedData(const OwnedData& other) = delete;
	OwnedData& operator=(const OwnedData& other) = delete;

	OwnedData& operator=(OwnedData&& other);
	OwnedData(OwnedData&& other);

	bool IsEmpty();
	void Clear();
	void TakeOwnership(const char* s, size_t size = 0);
	char* StealData();
	char* Get() const;

	// creates a copy of s
	static OwnedData MakeFromStr(const char* s, size_t size = 0);
};


template <typename T, size_t N>
char(&DimofSizeHelper(T(&array)[N]))[N];

template <typename T, size_t N>
size_t GetDim(T(&array)[N]) {
	return N;
}

void* memdup(const void* data, size_t len);

#include "StrUtil.h"
#include "Vec.h"
#include "ParseCommondLine.h"
#include "WinUtil.h"