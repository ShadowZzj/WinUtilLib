#pragma once
#include <string>
#include <locale.h>
#include <iostream>
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
		if (!s || IsEmpty(toReplace) || !replaceWith)
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
	template<class T>
	bool Equal(const T* src, const T* dst) {
		if (typeid(T) != typeid(char) && typeid(T) != typeid(wchar_t))
			return false;

		size_t srcCount = Len(src);
		size_t dstCount = Len(dst);
		if (srcCount != dstCount)
			return false;

		for (size_t i = 0; i < dstCount; i++) {
			if (src[i] != dst[i])
				return false;
		}
		return true;
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
	char* WideCharToUTF8(const wchar_t* wStr);
	std::string StringToUTF8(const std::string& str);
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
	//Statistical method to judge whether it is unicode text.
	bool IsTextUnicode(const void* buf, size_t cb, int* res);
	std::string UTF8ToString(const std::string& str);
	template<class T>
	void PrintBufferHex(T* buf, size_t len,T splitChar,std::ostream &out) {
		for (int i = 0; i < len; i++)
			out << std::hex << buf[i] << splitChar;

	}
	int GB2312ToUnicode(const char* gb2312, char* unicode);
	bool GUID2String_util(const GUID* pGUID, std::wstring& strGUID);
	bool String2GUID_util(GUID* pGUID, const std::wstring& strGUID);
	bool String2GUID_util(GUID* pGUID, const std::string& strGUID);
	bool GUID2String_util(const GUID* pGUID, std::string& strGUID);

	bool W2A_util(std::wstring& wStr, std::string& aStr);
	bool A2W_util(std::string& aStr, std::wstring& wStr);
	bool Unknown2W_util(const char* ustr, size_t ustrsize, std::wstring& wStr);
}
