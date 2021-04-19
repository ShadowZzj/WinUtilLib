#pragma once
#include <string>
#include <locale.h>
#include <iostream>

#ifdef _WIN32
#include <guiddef.h>
#endif
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
    bool IsStringUtf8(const char *str);
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
#ifdef _WIN32
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
#endif

	class Base64
    {
      public:
        enum ErrorCode
        {
			SUCCESS,
			BUFFER_TOO_SMALL,
			INVALID_BASE64
        };
        static std::string Encode(const char* data,size_t size)
        {
            static constexpr char sEncodingTable[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
                                                      'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                                                      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
                                                      'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                                                      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

            size_t in_len  = size;
            size_t out_len = 4 * ((in_len + 2) / 3);
            std::string ret(out_len, '\0');
            size_t i;
            char *p = const_cast<char *>(ret.c_str());

            for (i = 0; i < in_len - 2; i += 3)
            {
                *p++ = sEncodingTable[(data[i] >> 2) & 0x3F];
                *p++ = sEncodingTable[((data[i] & 0x3) << 4) | ((int)(data[i + 1] & 0xF0) >> 4)];
                *p++ = sEncodingTable[((data[i + 1] & 0xF) << 2) | ((int)(data[i + 2] & 0xC0) >> 6)];
                *p++ = sEncodingTable[data[i + 2] & 0x3F];
            }
            if (i < in_len)
            {
                *p++ = sEncodingTable[(data[i] >> 2) & 0x3F];
                if (i == (in_len - 1))
                {
                    *p++ = sEncodingTable[((data[i] & 0x3) << 4)];
                    *p++ = '=';
                }
                else
                {
                    *p++ = sEncodingTable[((data[i] & 0x3) << 4) | ((int)(data[i + 1] & 0xF0) >> 4)];
                    *p++ = sEncodingTable[((data[i + 1] & 0xF) << 2)];
                }
                *p++ = '=';
            }

            return ret;
        }

        static int Decode(const std::string &input, char* buf,size_t& bufSize)
        {
            static constexpr unsigned char kDecodingTable[] = {
                64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
                64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63, 52, 53, 54, 55,
                56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64, 64, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12,
                13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64, 64, 26, 27, 28, 29, 30, 31, 32,
                33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64, 64, 64,
                64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
                64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
                64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
                64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
                64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64};

            size_t in_len = input.size();
            if (in_len % 4 != 0)
                return INVALID_BASE64;

            size_t out_len = in_len / 4 * 3;
            if (input[in_len - 1] == '=')
                out_len--;
            if (input[in_len - 2] == '=')
                out_len--;

           if (bufSize < out_len)
            {
                bufSize = out_len;
                return BUFFER_TOO_SMALL;
            }

            for (size_t i = 0, j = 0; i < in_len;)
            {
                uint32_t a = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];
                uint32_t b = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];
                uint32_t c = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];
                uint32_t d = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];

                uint32_t triple = (a << 3 * 6) + (b << 2 * 6) + (c << 1 * 6) + (d << 0 * 6);

                if (j < out_len)
                    buf[j++] = (triple >> 2 * 8) & 0xFF;
                if (j < out_len)
                    buf[j++] = (triple >> 1 * 8) & 0xFF;
                if (j < out_len)
                    buf[j++] = (triple >> 0 * 8) & 0xFF;
            }

            return SUCCESS;
        }
    };
    }
