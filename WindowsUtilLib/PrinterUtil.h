#pragma once
#include "BaseUtil.h"
#include <initializer_list>

namespace WinPrintWrapper {
	class WinSpoolerWrapper;
	//single
	struct PrinterInfo {
		PRINTER_INFO_2* printerInfo;
		DWORD printerStatus;
	};
	//enum
	struct PrinterInfos {
		unsigned int printerInfoNum;
		PRINTER_INFO_2* printerInfos;
		DWORD* printerStatus;
	};
	//enum
	struct PrinterDriverInfos {
		int driverInfoNum;
		DRIVER_INFO_2* driverInfos;
	};
	class PdfParam {
	public:
		unsigned int copy;
		unsigned int pageFrom;
		unsigned int pageTo;
		bool collate;
		bool color;
		enum {
			flat,
			odd,
			even
		} foe;
		enum {
			duplex,
			duplexshort,
			duplexlong,
			simplex
		} savePaper;
		enum {
			letter,
			legal,
			tabloid,
			statement,
			A2,
			A3,
			A4,
			A5,
			A6
		} paperSize;

		//eg. "1-3,2x,collate,paper=A4,duplex,even"
		std::wstring ToSumatraCmdStr();

	};

	class PrinterJobManager {
	public:
		PrinterJobManager(std::wstring printerName = L"") :printerName(printerName) {}
		bool GetPrinterJobs();
		bool GetPrinterJob(DWORD jobID);
		bool ControlJob(DWORD jobID, DWORD commond);

	private:
		std::wstring printerName;
	};
}
