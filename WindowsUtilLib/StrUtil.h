#pragma once
#include <string>
namespace str {
	template<class T>
	class Str;
	size_t Len(const char* str);
	size_t Len(const wchar_t* str);
	char* FmtV(const char* fmt, va_list args);
	inline const char* Find(const char* str, const char* find) {
		return strstr(str, find);
	}
	inline const wchar_t* Find(const wchar_t* str, const wchar_t* find) {
		return wcsstr(str, find);
	}
	template<class T>
	T* Replace(const T* s, const T* toReplace, const T* replaceWith) {
		if (!s || str::IsEmpty(toReplace) || !replaceWith)
			return nullptr;

		Str<T> res;
		size_t replaceLen = str::Len(replaceWith);
		size_t toReplaceLen = str::Len(toReplace);
		const T* start, * end;
		start = s;
		while ((end = str::Find(s, toReplace)) != nullptr) {
			res.Append(start, end - start);
			res.Append(replaceWith, replaceLen);
			start = end + toReplaceLen;
		}
		res.Append(start);
		return res.StealData();
	}
	bool IsEmpty(const char* s);
	bool IsEmpty(const wchar_t* s);
	char* Dup(const char* s);
	wchar_t* Dup(const wchar_t* s);
	template<class T>
	T* DupN(const T* s, size_t lenCch) {
		if (!s)
			return nullptr;
		T* res = (T*)memdup((void*)s, (lenCch + 1) * sizeof(T));
		if (res)
			res[lenCch] = 0;
		return res;
	}
	std::wstring& ConcateWstring(std::wstring& dstStr, std::initializer_list<std::wstring> list);
	char* BackSlashDup(const char* str);
}