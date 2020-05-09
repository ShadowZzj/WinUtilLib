#include "PrinterUtil.h"

namespace WinPrintWrapper {
	std::wstring PdfParam::ToSumatraCmdStr() const
	{
#define _tws(val) std::to_wstring(val)
#define _co L","
#define _ws L" "
		std::wstring res;
		using str::ConcateWstring;
		res += L"\"";
		str::Str<wchar_t> res2;
		//copy
		ConcateWstring(res, _tws(copy), L"x", _co);

		str::ConcateStr(res2, copy, L"x", _co);

		//pageRange
		ConcateWstring(res, _tws(pageFrom), L"-", _tws(pageTo), _co);
		//color
		if (color)
			ConcateWstring(res, L"color", _co);
		else
			ConcateWstring(res, L"monochrome", _co);
		//collate
		if (collate)
			ConcateWstring(res, L"collate", _co);
		else
			ConcateWstring(res, L"nocollate", _co);

		switch (paperSize)
		{
		case WinPrintWrapper::PdfParam::letter:
			ConcateWstring(res, L"paper=", L"letter", _co);
			break;
		case WinPrintWrapper::PdfParam::legal:
			ConcateWstring(res, L"paper=", L"legal", _co);
			break;
		case WinPrintWrapper::PdfParam::tabloid:
			ConcateWstring(res, L"paper=", L"tabloid", _co);
			break;
		case WinPrintWrapper::PdfParam::statement:
			ConcateWstring(res, L"paper=", L"statement", _co);
			break;
		case WinPrintWrapper::PdfParam::A2:
			ConcateWstring(res, L"paper=", L"A2", _co);
			break;
		case WinPrintWrapper::PdfParam::A3:
			ConcateWstring(res, L"paper=", L"A3", _co);
			break;
		case WinPrintWrapper::PdfParam::A4:
			ConcateWstring(res, L"paper=", L"A4", _co);
			break;
		case WinPrintWrapper::PdfParam::A5:
			ConcateWstring(res, L"paper=", L"A5", _co);
			break;
		case WinPrintWrapper::PdfParam::A6:
			ConcateWstring(res, L"paper=", L"A6", _co);
			break;
		default:
			break;
		}
		//duplex or simplex
		switch (savePaper)
		{
		case WinPrintWrapper::PdfParam::duplex:
			ConcateWstring(res, L"duplex", _co);
			break;
		case WinPrintWrapper::PdfParam::duplexshort:
			ConcateWstring(res, L"duplexshort", _co);
			break;
		case WinPrintWrapper::PdfParam::duplexlong:
			ConcateWstring(res, L"duplexlong", _co);
			break;
		case WinPrintWrapper::PdfParam::simplex:
			ConcateWstring(res, L"simplex", _co);
			break;
		default:
			break;
		}
		//even or odd
		switch (foe)
		{
		case WinPrintWrapper::PdfParam::flat:
			break;
		case WinPrintWrapper::PdfParam::odd:
			ConcateWstring(res, L"odd", _co);
			break;
		case WinPrintWrapper::PdfParam::even:
			ConcateWstring(res, L"even", _co);
			break;
		default:
			break;
		}

		if (res.back() == L',') {
			res.pop_back();
		}
		res += L"\"";
		return res;
#undef _tws
#undef _co 
#undef _ws
	}

	bool PrinterJobManager::SetPrinter(std::wstring printerName) {
		HANDLE printer;
		bool res = OpenPrinter((LPWSTR)printerName.c_str(), &printer, NULL);
		if (!res)
			return false;
		this->printerName = printerName;
		ClosePrinter(printer);
		return true;
	}
	bool PrinterJobManager::GetPrinterJobs(JOB_INFO_2** ppJobInfo, int* pcJobs, DWORD* printerStatus) {
		if (printerName.empty())
			return false;

		HANDLE hPrinter;
		PRINTER_DEFAULTS def{};

		def.DesiredAccess = PRINTER_ALL_ACCESS;

		wchar_t* buf = str::Dup(printerName.c_str());
		defer{ Allocator::Free(NULL,buf); };

		if (!OpenPrinter(buf, &hPrinter, &def))
			return false;
		defer{ ClosePrinter(hPrinter); };

		DWORD cByteNeeded,
			nReturned,
			cByteUsed;
		JOB_INFO_2* pJobStorage = NULL;
		PRINTER_INFO_2* pPrinterInfo = NULL;

		/* Get the buffer size needed. */
		if (!GetPrinter(hPrinter, 2, NULL, 0, &cByteNeeded))
		{
			if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
				return FALSE;
		}

		pPrinterInfo = (PRINTER_INFO_2*)Allocator::Alloc(NULL, cByteNeeded);
		if (!(pPrinterInfo))
			/* Failure to allocate memory. */
			return FALSE;

		/* Get the printer information. */
		if (!GetPrinter(hPrinter,
			2,
			(LPBYTE)pPrinterInfo,
			cByteNeeded,
			&cByteUsed))
		{
			/* Failure to access the printer. */
			free(pPrinterInfo);
			pPrinterInfo = NULL;
			return FALSE;
		}

		/* Get job storage space. */
		if (!EnumJobs(hPrinter,
			0,
			pPrinterInfo->cJobs,
			2,
			NULL,
			0,
			(LPDWORD)&cByteNeeded,
			(LPDWORD)&nReturned))
		{
			if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			{
				free(pPrinterInfo);
				pPrinterInfo = NULL;
				return FALSE;
			}
		}

		pJobStorage = (JOB_INFO_2*)Allocator::Alloc(NULL, cByteNeeded);
		if (!pJobStorage) {
			/* Failure to allocate Job storage space. */
			free(pPrinterInfo);
			pPrinterInfo = NULL;
			return FALSE;
		}

		ZeroMemory(pJobStorage, cByteNeeded);

		/* Get the list of jobs. */
		if (!EnumJobs(hPrinter,
			0,
			pPrinterInfo->cJobs,
			2,
			(LPBYTE)pJobStorage,
			cByteNeeded,
			(LPDWORD)&cByteUsed,
			(LPDWORD)&nReturned))
		{
			free(pPrinterInfo);
			free(pJobStorage);
			pJobStorage = NULL;
			pPrinterInfo = NULL;
			return FALSE;
		}

		/*
		 *  Return the information.
		 */
		*pcJobs = nReturned;
		if(printerStatus)
			*printerStatus = pPrinterInfo->Status;
		*ppJobInfo = pJobStorage;
		free(pPrinterInfo);
		return TRUE;
	}
	bool PrinterJobManager::GetPrinterJob(DWORD jobID, JOB_INFO_2** pJobInfo) {
		if (printerName.empty())
			return false;

		HANDLE hPrinter;
		PRINTER_DEFAULTS def{};

		def.DesiredAccess = PRINTER_ALL_ACCESS;

		wchar_t* buf = str::Dup(printerName.c_str());
		defer{ Allocator::Free(NULL,buf); };

		if (!OpenPrinter(buf, &hPrinter, &def))
			return false;
		defer{ ClosePrinter(hPrinter); };

		JOB_INFO_2* job;

		DWORD needed = 0;
		if (!GetJob(hPrinter, jobID, 2, nullptr, 0, &needed))
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
				job = (JOB_INFO_2*)Allocator::Alloc(nullptr, needed);
				if (GetJob(hPrinter, jobID, 2, (LPBYTE)job, needed, &needed)) {
					*pJobInfo = job;
					return true;
				}
				else {
					Allocator::Free(nullptr, job);
					return false;
				}
			}
			else
				return false;

		return false;
	}
	bool PrinterJobManager::ControlJob(std::wstring printerName, const wchar_t* document, DWORD commond) {

		HANDLE hPrinter;
		PRINTER_DEFAULTS def{};

		def.DesiredAccess = PRINTER_ALL_ACCESS;

		wchar_t* buf = str::Dup(printerName.c_str());
		defer{ Allocator::Free(NULL,buf); };

		if (!OpenPrinter(buf, &hPrinter, &def))
			return false;
		defer{ ClosePrinter(hPrinter); };

		JOB_INFO_2* jobs;
		int count;
		if (!GetPrinterJobs(&jobs, &count, nullptr))
			return false;

		Allocator::Free(nullptr, jobs);

		std::wstring wdocu = document;
		for (int i = 0; i < count; i++) {
			std::wstring jobDocu = jobs[i].pDocument;
			if(jobDocu.find(wdocu)!=-1)
				if (!SetJob(hPrinter, jobs[i].JobId, 0, NULL, commond)) {
					DWORD error = GetLastError();

					return false;
				}
				else
					return true;
		}

		return false;
	}

	wchar_t* PrinterBase::PrinterStatusToWstr(DWORD status) {
		enum Status :unsigned int {
			_,
			Other,
			Unknown,
			Idle,
			Printing,
			Warmup,
			StoppedPrinting,
			Offline
		};
		const wchar_t* statStr[] = { L"_",L"Other",L"Unkonwn",L"Idle",L"Printing",L"Warmup",L"StoppedPrinting",L"Offline" };

		if (dimof1(statStr) <= status)
			return nullptr;
		wchar_t* res = str::Dup(statStr[status]);
		return res;
	}
	std::vector<std::wstring> WinPrintWrapper::PrinterBase::JobStatusToWstr(DWORD status) {
#define str(a) L#a
		const wchar_t* statusStr[]{
			str(JOB_STATUS_PAUSED),
			str(JOB_STATUS_ERROR),
			str(JOB_STATUS_DELETING),
			str(JOB_STATUS_SPOOLING),
			str(JOB_STATUS_PRINTING),
			str(JOB_STATUS_OFFLINE),
			str(JOB_STATUS_PAPEROUT),
			str(JOB_STATUS_PRINTED),
			str(JOB_STATUS_DELETED),
			str(JOB_STATUS_BLOCKED_DEVQ),
			str(JOB_STATUS_USER_INTERVENTION),
			str(JOB_STATUS_RESTART)
		};

		int len = dimof1(statusStr);
		std::vector<std::wstring> ret;

		for (int i = 0, init = 1; i < len; i++) {
			if (status & init) {
				ret.push_back(statusStr[i]);
			}
			init = init << 1;
		}
		return ret;

#undef str
	}
	std::vector<std::string> WinPrintWrapper::PrinterBase::PrinterCapabilitiesToWstrs(UINT32* capabilities, UINT len) {
		using namespace std;
		vector<string> ret;
		for (UINT i = 0; i < len; i++) {
			ret.push_back(PrinterCapabilityToStr(capabilities[i]));
		}
		return ret;
	}
	std::string WinPrintWrapper::PrinterBase::PrinterCapabilityToStr(UINT32 capability)
	{
		const char* capsStr[] = { "unknown","other","color","duplex","copy","collate" };

		if (dimof1(capsStr) <= capability)
			return nullptr;

		std::string res(capsStr[capability]);
		return res;
	}
	std::string WinPrintWrapper::PrinterBase::PrinterPaperSizeToStr(UINT32 size)
	{
		std::string res;
		switch (size)
		{
		case 7:
			res = "Letter";
			break;
		case 21:
			res = "A3";
			break;
		case 22:
			res = "A4";
			break;
		case 23:
			res = "A5";
			break;
		default:
			res = "";
			break;
		}
		return res;
	}
	WCHAR* WinPrintWrapper::PrinterBase::GetDefaultPrinterName()
	{
		DWORD bufSize;
		GetDefaultPrinter(NULL, &bufSize);
		WCHAR* buf = (WCHAR*)Allocator::Alloc(NULL, bufSize + 2);
		memset(buf, 0, bufSize + 2);

		buf[0] = L'\"';
		if (GetDefaultPrinter(buf + 1, &bufSize)) {
			buf[bufSize] = L'\"';
			buf[bufSize + 1] = L'\0';
			return buf;
		}
		else
			return nullptr;


	}
	BOOL WinPrintWrapper::PrinterBase::SetPrinterDevmode(wchar_t* pPrinterName, DWORD field, DWORD value)
	{
		HANDLE hPrinter = NULL;
		DWORD dwNeeded = 0;
		PRINTER_INFO_2* pi2 = NULL;
		DEVMODE* pDevMode = NULL;
		PRINTER_DEFAULTS pd;
		BOOL bFlag;
		LONG lFlag;
		// Open printer handle (on Windows NT, you need full-access because you
		// will eventually use SetPrinter)...
		ZeroMemory(&pd, sizeof(pd));
		pd.DesiredAccess = PRINTER_ALL_ACCESS;
		bFlag = OpenPrinter(pPrinterName, &hPrinter, &pd);
		if (!bFlag || (hPrinter == NULL))
			return FALSE;

		// The first GetPrinter tells you how big the buffer should be in 
		// order to hold all of PRINTER_INFO_2. Note that this should fail with 
		// ERROR_INSUFFICIENT_BUFFER.  If GetPrinter fails for any other reason 
		// or dwNeeded isn't set for some reason, then there is a problem...
		SetLastError(0);
		bFlag = GetPrinter(hPrinter, 2, 0, 0, &dwNeeded);
		if ((!bFlag) && (GetLastError() != ERROR_INSUFFICIENT_BUFFER) ||
			(dwNeeded == 0))
		{
			ClosePrinter(hPrinter);
			return FALSE;
		}

		// Allocate enough space for PRINTER_INFO_2...
		pi2 = (PRINTER_INFO_2*)GlobalAlloc(GPTR, dwNeeded);
		if (pi2 == NULL)
		{
			ClosePrinter(hPrinter);
			return FALSE;
		}

		// The second GetPrinter fills in all the current settings, so all you
		// need to do is modify what you're interested in...
		bFlag = GetPrinter(hPrinter, 2, (LPBYTE)pi2, dwNeeded, &dwNeeded);
		if (!bFlag)
		{
			GlobalFree(pi2);
			ClosePrinter(hPrinter);
			return FALSE;
		}

		// If GetPrinter didn't fill in the DEVMODE, try to get it by calling
		// DocumentProperties...
		if (pi2->pDevMode == NULL)
		{
			dwNeeded = DocumentProperties(NULL, hPrinter,
				pPrinterName,
				NULL, NULL, 0);
			if (dwNeeded <= 0)
			{
				GlobalFree(pi2);
				ClosePrinter(hPrinter);
				return FALSE;
			}

			pDevMode = (DEVMODE*)GlobalAlloc(GPTR, dwNeeded);
			if (pDevMode == NULL)
			{
				GlobalFree(pi2);
				ClosePrinter(hPrinter);
				return FALSE;
			}

			lFlag = DocumentProperties(NULL, hPrinter,
				pPrinterName,
				pDevMode, NULL,
				DM_OUT_BUFFER);
			if (lFlag != IDOK || pDevMode == NULL)
			{
				GlobalFree(pDevMode);
				GlobalFree(pi2);
				ClosePrinter(hPrinter);
				return FALSE;
			}

			pi2->pDevMode = pDevMode;
		}

		// Driver is reporting that it doesn't support this change...
		if (!(pi2->pDevMode->dmFields & field))
		{
			GlobalFree(pi2);
			ClosePrinter(hPrinter);
			if (pDevMode)
				GlobalFree(pDevMode);
			return FALSE;
		}

		// Specify exactly what we are attempting to change...
		pi2->pDevMode->dmFields = field;
		pi2->pDevMode->dmColor = value;

		// Do not attempt to set security descriptor...
		pi2->pSecurityDescriptor = NULL;

		// Make sure the driver-dependent part of devmode is updated...
		lFlag = DocumentProperties(NULL, hPrinter,
			pPrinterName,
			pi2->pDevMode, pi2->pDevMode,
			DM_IN_BUFFER | DM_OUT_BUFFER);
		if (lFlag != IDOK)
		{
			GlobalFree(pi2);
			ClosePrinter(hPrinter);
			if (pDevMode)
				GlobalFree(pDevMode);
			return FALSE;
		}

		// Update printer information...
		bFlag = SetPrinter(hPrinter, 2, (LPBYTE)pi2, 0);
		if (!bFlag)
			// The driver doesn't support, or it is unable to make the change...
		{
			GlobalFree(pi2);
			ClosePrinter(hPrinter);
			if (pDevMode)
				GlobalFree(pDevMode);
			return FALSE;
		}

		// Tell other apps that there was a change...
		SendMessageTimeout(HWND_BROADCAST, WM_DEVMODECHANGE, 0L,
			(LPARAM)(LPCSTR)pPrinterName,
			SMTO_NORMAL, 1000, NULL);

		// Clean up...
		if (pi2)
			GlobalFree(pi2);
		if (hPrinter)
			ClosePrinter(hPrinter);
		if (pDevMode)
			GlobalFree(pDevMode);

		return TRUE;
	}
}
