#include "BaseUtil.h"
#include <comdef.h>
namespace str {
	size_t Len(const wchar_t* str) {
		if (str == nullptr)
			return -1;
		return wcslen(str);
	}
	size_t Len(const char* str) {
		if (str == nullptr)
			return -1;
		return strlen(str);
	}
	char* Dup(const char* s) {
		return s ? _strdup(s) : nullptr;
	}
	wchar_t* Dup(const wchar_t* s) {
		if (!s)
			return nullptr;
		wchar_t* dst;
		size_t size=wcslen(s)*sizeof(wchar_t);
		dst = (wchar_t*)Allocator::Alloc(nullptr, size+2);
		memcpy(dst, s, size);
		return dst;
	}
	std::wstring& ConcateWstring(std::wstring& dstStr, std::initializer_list<std::wstring> list) {
		for (auto& str : list) {
			dstStr += str;
		}
		return dstStr;
	}
	char* BackSlashDup(const char* str){
		if (!str)
			return nullptr;

		const char* src = str;
		size_t len = Len(str);
		size_t backSlashNum = 0;
		while (*src) {
			if (*src == '\\')
				backSlashNum++;
			src++;
		}
		if (backSlashNum == 0)
			return nullptr;
		char* ret = (char*)Allocator::Alloc(NULL, len + backSlashNum + 1);
		char* dst = ret;
		src = str;
		while (*src) {
			if (*src == '\\') {
				*dst = '\\';
				*(dst + 1) = '\\';
				dst += 2;
				src++;
			}
			else {
				*dst = *src;
				dst++;
				src++;
			}
		}
		return ret;
	}
	char* Wstr2Str(const wchar_t* wstr){
		const WCHAR* wc = wstr;
		_bstr_t b(wc);
		const char* c = b;
		char* ret = str::Dup(c);
		return ret;
	}
	wchar_t* Str2Wstr(const char* str){
		const CHAR* wc = str;
		_bstr_t b(wc);
		const WCHAR* c = b;
		WCHAR* ret = str::Dup(c);
		return ret;
	}
	char* FmtV(const char* fmt, va_list args) {
		char message[256];
		size_t bufCchSize = dimof1(message);
		char* buf = message;
		for (;;) {
			int count = vsnprintf(buf, bufCchSize, fmt, args);
			if ((count >= 0) && ((size_t)count < bufCchSize))
				break;
			/* we have to make the buffer bigger. The algorithm used to calculate
			   the new size is arbitrary*/
			if (buf != message)
				free(buf);
			if (bufCchSize < 4 * 1024)
				bufCchSize += bufCchSize;
			else
				bufCchSize += 1024;
			buf = (char*)Allocator::Alloc(nullptr, bufCchSize);
			if (!buf)
				break;
		}

		if (buf == message)
			buf = str::Dup(message);

		return buf;
	}
	bool IsEmpty(const char* s){
		return !s || *s == 0;
	}
	bool IsEmpty(const wchar_t* s){
		return !s || *s == 0;
	}
}