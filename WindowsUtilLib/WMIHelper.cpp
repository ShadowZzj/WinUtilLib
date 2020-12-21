#include <iostream>
#include "WMIHelper.h"
#include "BaseUtil.h"
#include "StrUtil.h"
#pragma comment(lib,"wbemuuid.lib")
namespace WMIHelper {
	using namespace std;
	WMIWrapper* WMIWrapper::wmiWrapper = NULL;


	HRESULT WMIWrapper::WMIConnectServer(IN const wchar_t* serverName)
	{
		HRESULT hres;
		hres = pLoc->ConnectServer(
			_bstr_t(serverName), // WMI namespace
			NULL,                    // User name
			NULL,                    // User password
			0,                       // Locale
			WBEM_FLAG_CONNECT_USE_MAX_WAIT,                    // Security flags                 
			0,                       // Authority       
			0,                       // Context object
			&pSvc                    // IWbemServices proxy
		);

		if (FAILED(hres)) {
			cout << "Could not connect. Error code = 0x"
				<< hex << hres << endl;

			pLoc->Release();
			isIWbemLocatorReleaseNeeded = false;
			pLoc = nullptr;
			pSvc = nullptr;

			return hres;
		}
		wcout << "Connected to " << serverName << " WMI namespace" << endl;

		isIWbemServicesReleaseNeeded = true;

		// Set the IWbemServices proxy so that impersonation
		// of the user (client) occurs.
		hres = CoSetProxyBlanket(

			pSvc,                         // the proxy to set
			RPC_C_AUTHN_WINNT,            // authentication service
			RPC_C_AUTHZ_NONE,             // authorization service
			NULL,                         // Server principal name
			RPC_C_AUTHN_LEVEL_CALL,       // authentication level
			RPC_C_IMP_LEVEL_IMPERSONATE,  // impersonation level
			NULL,                         // client identity 
			EOAC_NONE                     // proxy capabilities     
		);

		if (FAILED(hres)) {
			cout << "Could not set proxy blanket. Error code = 0x"
				<< hex << hres << endl;
			pSvc->Release();
			isIWbemServicesReleaseNeeded = false;
			pSvc = nullptr;

			pLoc->Release();
			isIWbemLocatorReleaseNeeded = false;
			pLoc = nullptr;

			return hres;               // Program has failed.
		}

		return hres;
	}
	HRESULT WMIWrapper::WMIExecQuery(const char* sentenceType, const char* sentence)
	{
		if (!pSvc || !sentenceType || !sentence)
			return -1;

		HRESULT hres;
		hres = pSvc->ExecQuery(
			bstr_t(sentenceType),
			bstr_t(sentence),
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
			NULL,
			&pEnumerator);
		if (FAILED(hres)) {
			cout << "Query failed. "
				<< "Error code = 0x"
				<< hex << hres << endl;
			pSvc->Release();
			isIWbemServicesReleaseNeeded = false;
			pSvc = nullptr;

			pLoc->Release();
			isIWbemLocatorReleaseNeeded = false;
			pLoc = nullptr;

			return hres;               // Program has failed.
		}

		isIEnumWbemClassObjectReleaseNeeded = true;
		return hres;
	}
	HRESULT WMIWrapper::WMIGetNextObject(IWbemClassObject** pclsObj) {
		if (!pEnumerator)
			return -1;

		HRESULT hres;
		DWORD uReturn = 0;

		hres = pEnumerator->Next(WBEM_INFINITE, 1, pclsObj, &uReturn);
		if (0 == uReturn)
			return -1;

		return S_OK;
	}
	HRESULT WMIWrapper::WMIEnumUnInitialize() {
		if (isIEnumWbemClassObjectReleaseNeeded) {
			pEnumerator->Release();
			pEnumerator = nullptr;
		}
		return S_OK;
	}
	std::vector<VARIANT> WMIWrapper::GetProperty(const char* className, const wchar_t* property, const char* filterProperty, const char* filterValue, bool& success)
	{
		using namespace std;

		success = true;
		HRESULT hres;
		char buf[1024]{};

		if(!filterProperty||!filterValue)
			sprintf_s(buf, "SELECT * FROM %s", className);
		else
			sprintf_s(buf, "SELECT * FROM %s WHERE %s=\"%s\"", className, filterProperty, filterValue);
		
		char* sentence = str::BackSlashDup(buf);

		if (!sentence)
			sentence = buf;

		hres = wmiWrapper->WMIExecQuery("WQL", sentence);
		if (FAILED(hres)) {
			success = false;
			return std::vector<VARIANT>();
		}

		IWbemClassObject* clsObj=nullptr;
		VARIANT vtProp;
		std::vector<VARIANT> ret;

		while (!FAILED((hres = wmiWrapper->WMIGetNextObject(&clsObj)))) {
			hres = clsObj->Get(property, 0, &vtProp, 0, 0);
			if (FAILED(hres)) {
				clsObj->Release();
				success = false;
				wmiWrapper->WMIEnumUnInitialize();
				if (sentence != buf)
					Allocator::Free(NULL, sentence);
				return std::vector<VARIANT>();
			}
			ret.push_back(vtProp);
			clsObj->Release();
		}

		wmiWrapper->WMIEnumUnInitialize();
		if (sentence != buf)
			Allocator::Free(NULL, sentence);
		return ret;
	}
	WMIWrapper* WMIWrapper::GetInstance() {
		if (!wmiWrapper) {
			wmiWrapper = new WMIWrapper();
		}
		return wmiWrapper;
	}
	void WMIWrapper::DestroyInstance() {
		if (wmiWrapper) {
			delete wmiWrapper;
			wmiWrapper = NULL;
		}
	}
	WMIWrapper::WMIWrapper() {
		comWrapper = COMHelper::COMWrapper::GetInstance();
		Initialize();
	}
	WMIWrapper::~WMIWrapper() {
		UnInitialize();
	}
	HRESULT WMIWrapper::Initialize() {
		HRESULT hres;
		hres = comWrapper->CreateComInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID**)&pLoc);
		if (FAILED(hres)) {
			CrashMe();
		}
		isIWbemLocatorReleaseNeeded = true;
		return hres;
	}
	int WMIWrapper::UnInitialize()
	{
		WMIEnumUnInitialize();
		if (isIWbemLocatorReleaseNeeded) {
			pLoc->Release();
			pLoc = nullptr;
		}
		if (isIWbemServicesReleaseNeeded) {
			pSvc->Release();
			pSvc = nullptr;
		}
		return 0;
	}
}