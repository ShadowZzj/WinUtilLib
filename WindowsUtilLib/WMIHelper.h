#pragma once
#pragma once
#pragma once
#include "COMHelper.h"
#include <vector>
class Test;
namespace WMIHelper {
	//Not support multi-thread, caller must synchronize.
	class WMIWrapper {
	public:
		friend class Test;

		//WMI
		HRESULT WMIConnectServer(const wchar_t* serverName);
		HRESULT WMIExecQuery(const char* sentenceType, const char* sentence);
		HRESULT WMIGetNextObject(IWbemClassObject** pclsObj);
		HRESULT WMIEnumUnInitialize();
	    std::vector<VARIANT> GetProperty(const char* className, const wchar_t* property, const char* filterProperty, const char* filterValue, bool& success);
		static WMIWrapper* GetInstance();
		static void DestroyInstance();

	private:
		WMIWrapper();
		~WMIWrapper();
		WMIWrapper(const WMIWrapper&) = delete;
		WMIWrapper operator=(const WMIWrapper&) = delete;
		HRESULT Initialize();
		int UnInitialize();

		bool isIWbemLocatorReleaseNeeded = false;
		bool isIWbemServicesReleaseNeeded = false;
		bool isIEnumWbemClassObjectReleaseNeeded = false;
		IWbemLocator* pLoc = nullptr;
		IWbemServices* pSvc = nullptr;
		IEnumWbemClassObject* pEnumerator = nullptr;
		COMHelper::COMWrapper* comWrapper;

		static WMIWrapper* wmiWrapper;
	};

}
