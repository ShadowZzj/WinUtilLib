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
	template<typename T1, typename ...T2>
	std::wstring& ConcateWstring(std::wstring& dstStr, T1 arg, T2 ...args) {
		dstStr += arg;
		if constexpr (sizeof...(args) > 0)
			return ConcateWstring(dstStr, args...);
		else
			return dstStr;
	}
	
	template<typename T,typename T1,typename ...T2>
	Str<T>& ConcateStr(Str<T>& src, T1 arg, T2 ...args) {
		src += arg;
		if constexpr (sizeof...(args) > 0)
			return ConcateStr(src, args...);
		else
			return src;
	}
	char* BackSlashDup(const char* str);
	char* Wstr2Str(const wchar_t* wstr);
	std::string Wstr2Str(std::wstring wstr);
	wchar_t* Str2Wstr(const char* str);
	std::wstring Str2Wstr(std::string str);
	template<class T1>
	auto ReverseStrWstr(const T1* str) {
		if (str == nullptr)
			return (void*)nullptr;

		if (typeid(T1) == typeid(char)) {
			return (void*)Str2Wstr((const char*)str);
		}
		else {
			return (void*)Wstr2Str((const wchar_t*)str);
		}
	}
	
}