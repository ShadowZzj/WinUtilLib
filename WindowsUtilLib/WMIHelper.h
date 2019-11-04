#pragma once
#pragma once
#pragma once
#include "COMHelper.h"
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

		static WMIWrapper* GetInstance();
		static void DestroyInstance();

	protected:
		HRESULT Initialize();
		int UnInitialize();
	private:
		bool isIWbemLocatorReleaseNeeded = false;
		bool isIWbemServicesReleaseNeeded = false;
		bool isIEnumWbemClassObjectReleaseNeeded = false;
		IWbemLocator* pLoc = nullptr;
		IWbemServices* pSvc = nullptr;
		IEnumWbemClassObject* pEnumerator = nullptr;

		WMIWrapper();
		~WMIWrapper();
		WMIWrapper(const WMIWrapper&) = delete;
		WMIWrapper operator=(const WMIWrapper&) = delete;

		static WMIWrapper* wmiWrapper;
		COMHelper::COMWrapper* comWrapper;
	};

}
