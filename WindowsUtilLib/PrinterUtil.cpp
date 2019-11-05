#include "PrinterUtil.h"

namespace WinPrintWrapper {
	std::wstring PdfParam::ToSumatraCmdStr()
	{
#define _tws(val) std::to_wstring(val)
#define _co L","
#define _ws L" "
		std::wstring res;
		using str::ConcateWstring;
		res += L"\"";

		//copy
		ConcateWstring(res, { _tws(copy),L"x" ,_co });
		//pageRange
		ConcateWstring(res, { _tws(pageFrom),L"-",_tws(pageTo),_co });
		//color
		if (color)
			ConcateWstring(res, { L"color",_co });
		else
			ConcateWstring(res, { L"monochrome",_co });
		//collate
		if (collate)
			ConcateWstring(res, { L"collate",_co });
		else
			ConcateWstring(res, { L"nocollate",_co });

		switch (paperSize)
		{
		case WinPrintWrapper::PdfParam::letter:
			ConcateWstring(res, { L"paper=",L"letter",_co });
			break;
		case WinPrintWrapper::PdfParam::legal:
			ConcateWstring(res, { L"paper=",L"legal",_co });
			break;
		case WinPrintWrapper::PdfParam::tabloid:
			ConcateWstring(res, { L"paper=",L"tabloid",_co });
			break;
		case WinPrintWrapper::PdfParam::statement:
			ConcateWstring(res, { L"paper=",L"statement",_co });
			break;
		case WinPrintWrapper::PdfParam::A2:
			ConcateWstring(res, { L"paper=",L"A2",_co });
			break;
		case WinPrintWrapper::PdfParam::A3:
			ConcateWstring(res, { L"paper=",L"A3",_co });
			break;
		case WinPrintWrapper::PdfParam::A4:
			ConcateWstring(res, { L"paper=",L"A4",_co });
			break;
		case WinPrintWrapper::PdfParam::A5:
			ConcateWstring(res, { L"paper=",L"A5",_co });
			break;
		case WinPrintWrapper::PdfParam::A6:
			ConcateWstring(res, { L"paper=",L"A6",_co });
			break;
		default:
			break;
		}
		//duplex or simplex
		switch (savePaper)
		{
		case WinPrintWrapper::PdfParam::duplex:
			ConcateWstring(res, { L"duplex",_co });
			break;
		case WinPrintWrapper::PdfParam::duplexshort:
			ConcateWstring(res, { L"duplexshort",_co });
			break;
		case WinPrintWrapper::PdfParam::duplexlong:
			ConcateWstring(res, { L"duplexlong",_co });
			break;
		case WinPrintWrapper::PdfParam::simplex:
			ConcateWstring(res, { L"simplex",_co });
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
			ConcateWstring(res, { L"odd",_co });
			break;
		case WinPrintWrapper::PdfParam::even:
			ConcateWstring(res, { L"even",_co });
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
	bool PrinterJobManager::GetPrinterJobs() {
		return false;
	}
	bool PrinterJobManager::GetPrinterJob(DWORD jobID) {
		return false;
	}
	bool PrinterJobManager::ControlJob(DWORD jobID, DWORD commond)
	{
		return false;
	}
}