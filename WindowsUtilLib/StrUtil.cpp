#include "BaseUtil.h"
#include <comdef.h>

#include <sstream>

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
		if (IsEmpty(wstr))
			return nullptr;
		const WCHAR* wc = wstr;
		_bstr_t b(wc);
		const char* c = b;
		char* ret = str::Dup(c);
		return ret;
	}
	std::string Wstr2Str(std::wstring wstr)
	{

		const wchar_t* tmp = wstr.c_str();
		char* str = Wstr2Str(tmp);
		defer{ Allocator::Free(nullptr,str); };
		std::string ret(str);
		return ret;
	}
	std::wstring Str2Wstr(std::string str) {
		const char* tmp = str.c_str();
		wchar_t* wstr = Str2Wstr(tmp);
		defer{ Allocator::Free(nullptr,wstr); };
		std::wstring ret(wstr);
		return ret;
	}
	wchar_t* Str2Wstr(const char* str){
		if (IsEmpty(str))
			return nullptr;
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
	bool IsTextUnicode(const void* buf, size_t cb, int* res) {
		return ::IsTextUnicode(buf, cb, res);
	}
	char* WideCharToUTF8(const wchar_t* wStr) {
		char* buf = (char*)Allocator::Alloc(nullptr, 1000);
		int ret=WideCharToMultiByte(CP_UTF8, NULL, wStr, str::Len(wStr), buf, 1000, NULL, NULL);
		if (ret == 0) {
			int error = GetLastError();
			std::cout << "ret==0 error=" << error;
		}
		return buf;
	}
	std::string StringToUTF8(const std::string& str)
	{
		int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

		wchar_t* pwBuf = new wchar_t[nwLen + 1];
		ZeroMemory(pwBuf, nwLen * 2 + 2);

		::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

		int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

		char* pBuf = new char[nLen + 1];
		ZeroMemory(pBuf, nLen + 1);

		::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

		std::string retStr(pBuf);

		delete[]pwBuf;
		delete[]pBuf;

		pwBuf = NULL;
		pBuf = NULL;

		return retStr;
	}
	std::string UTF8ToString(const std::string& str) {
		int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
		wchar_t* pwBuf = new wchar_t[nwLen + 1];
		memset(pwBuf, 0, nwLen * 2 + 2);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);
		int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
		char* pBuf = new char[nLen + 1];
		memset(pBuf, 0, nLen + 1);
		WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
		std::string retStr = pBuf;
		delete[]pBuf;
		delete[]pwBuf;
		pBuf = NULL;
		pwBuf = NULL;
		return retStr;
	}


	int GB2312ToUnicode(const char* gb2312, char* unicode)
	{
		UINT nCodePage = 936; //GB2312
		int len = MultiByteToWideChar(nCodePage, 0, gb2312, -1, NULL, 0);
		wchar_t* wstr = new wchar_t[len + 1];
		memset(wstr, 0, len + 1);
		MultiByteToWideChar(nCodePage, 0, gb2312, -1, wstr, len);
		len = len * sizeof(wchar_t);
		memcpy(unicode, wstr, len);
		if (wstr) delete[] wstr;
		return len;
	}
	bool GUID2String_util(const GUID* pGUID, std::wstring& strGUID)
	{
		if (NULL == pGUID)
			return false;

		strGUID.clear();
		GUID IfGuid = *pGUID;

		WCHAR cGUID[64] = { 0 };
		if (0 != StringFromGUID2(IfGuid, cGUID, 63))
		{
			strGUID = std::wstring(cGUID);
			return true;
		}

		return false;
	}
	bool String2GUID_util(GUID* pGUID, const std::wstring& strGUID)
	{
		CLSID clsidRet = { 0 };
		if (NOERROR == CLSIDFromString(strGUID.c_str(), &clsidRet)) {
			*pGUID = clsidRet;
			return true;
		}
		return false;
	}
	bool String2GUID_util(GUID* pGUID, const std::string& strGUID)
	{
		std::wstring wstrGUID;
		std::string strGUID_temp = strGUID;
		if (!A2W_util(strGUID_temp, wstrGUID))
			return false;

		return String2GUID_util(pGUID, wstrGUID);
	}
	bool GUID2String_util(const GUID* pGUID, std::string& strGUID)
	{
		std::wstring wstrGUID;
		if (GUID2String_util(pGUID, wstrGUID))
		{
			std::string strGUID_temp;
			if (W2A_util(wstrGUID, strGUID_temp))
			{
				strGUID = strGUID_temp;
				return true;
			}
		}
		return false;
	}

	bool W2A_util(std::wstring& wStr, std::string& aStr)
	{
		bool bRet = false;
		size_t strCount = wStr.size();
		if (0 == strCount)
			return false;

		char szLocal[2] = { 0 };
		int nCharRet = ::WideCharToMultiByte(CP_UTF8, 0, wStr.c_str(), strCount, szLocal, 0, NULL, NULL);
		if (0 == nCharRet) {
			return false;
		}

		char* aTemp = new char[nCharRet];
		if (nullptr == aTemp) {
			return false;
		}
		memset(aTemp, 0, nCharRet * sizeof(char));

		int nCharRet1 = ::WideCharToMultiByte(CP_UTF8, 0, wStr.c_str(), strCount, aTemp, nCharRet, NULL, NULL);
		if (0 != nCharRet1) {

			aStr.assign(aTemp, nCharRet);
			bRet = true;
		}

		delete[]aTemp;

		return bRet;
	}

	bool A2W_util(std::string& aStr, std::wstring& wStr)
	{
		bool bRet = false;
		size_t strCount = aStr.size();
		if (0 == strCount)
			return false;

		WCHAR szLocal[2] = { 0 };
		int nCharRet = ::MultiByteToWideChar(CP_UTF8, 0, aStr.data(), strCount, szLocal, 0);
		if (0 == nCharRet) {
			return false;
		}

		nCharRet = nCharRet + 1;
		WCHAR* wTemp = new WCHAR[nCharRet];
		if (nullptr == wTemp) {
			return false;
		}
		memset(wTemp, 0, nCharRet * sizeof(WCHAR));

		int nCharRet1 = ::MultiByteToWideChar(CP_UTF8, 0, aStr.data(), strCount, wTemp, nCharRet - 1);
		if (0 != nCharRet1) {
			wStr = wTemp;
			bRet = true;
		}

		delete[]wTemp;

		return bRet;
	}

	bool Unknown2W_util(const char* ustr, size_t ustrsize, std::wstring& wStr)
	{
		bool bRet = false;
		WCHAR szTemp[2] = { 0 };

		//utf8 detect
		int nCharRet = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, ustr, ustrsize, szTemp, 0);
		if (0 != nCharRet) {
			nCharRet = nCharRet + 1;
			WCHAR* wTemp = new WCHAR[nCharRet];
			if (nullptr == wTemp) {
				return false;
			}
			memset(wTemp, 0, nCharRet * sizeof(WCHAR));

			nCharRet = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, ustr, ustrsize, wTemp, nCharRet - 1);
			if (0 != nCharRet) {
				wStr = wTemp;
				bRet = true;
			}

			delete[]wTemp;
			return bRet;
		}

		//gb2312 detect
		nCharRet = ::MultiByteToWideChar(936, MB_ERR_INVALID_CHARS, ustr, ustrsize, szTemp, 0);
		if (0 != nCharRet) {
			nCharRet = nCharRet + 1;
			WCHAR* wTemp = new WCHAR[nCharRet];
			if (nullptr == wTemp) {
				return false;
			}
			memset(wTemp, 0, nCharRet * sizeof(WCHAR));

			nCharRet = ::MultiByteToWideChar(936, MB_ERR_INVALID_CHARS, ustr, ustrsize, wTemp, nCharRet - 1);
			if (0 != nCharRet) {
				wStr = wTemp;
				bRet = true;
			}

			delete[]wTemp;
			return bRet;
		}

		return false;
	}
}