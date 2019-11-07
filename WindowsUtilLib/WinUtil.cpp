#include "WinUtil.h"
#include "BaseUtil.h"
WCHAR* GetDefaultPrinterName() {
	DWORD bufSize;
	GetDefaultPrinter(NULL, &bufSize);
	WCHAR* buf = (WCHAR*)malloc(bufSize+2);
	memset(buf, 0, bufSize+2);

	buf[0] = L'\"';
	if (GetDefaultPrinter(buf+1, &bufSize)) {
		buf[bufSize] = L'\"';
		buf[bufSize + 1] =L'\0';
		return buf;
	}
	else
		return nullptr;
	
}