#include "COMHelper.h"
#include <iostream>
#pragma comment(lib,"wbemuuid.lib")
namespace COMHelper {
	using namespace std;
	COMWrapper* COMWrapper::comWrapper = NULL;

	HRESULT COMWrapper::CreateComInstance(const IID& rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, const IID& riid, OUT LPVOID** ppv) {
		HRESULT hres = CoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, (LPVOID*)ppv);
		if (FAILED(hres)) {
			cout << "Failed to create IWbemLocator object. "
				<< "Error code = 0x"
				<< hex << hres << endl;
			CoUninitialize();
			isCoUninitializeNeeded = false;
			return hres;
		}

		return hres;
	}
	COMWrapper* COMWrapper::GetInstance() {
		if (!comWrapper) {
			comWrapper = new COMWrapper();
		}
		return comWrapper;
	}
	void COMWrapper::DestroyInstance() {
		if (comWrapper) {
			delete comWrapper;
			comWrapper = NULL;
		}
	}
	COMWrapper::COMWrapper() {
		Initialize();
	}
	COMWrapper::~COMWrapper() {
		UnInitialize();
	}
	HRESULT COMWrapper::Initialize() {
		HRESULT hres;

		// Initialize COM.
		hres = CoInitializeEx(0, COINIT_MULTITHREADED);
		if (FAILED(hres)) {
			cout << "Failed to initialize COM library. "
				<< "Error code = 0x"
				<< hex << hres << endl;
			return 1;              // Program has failed.
		}
		isCoUninitializeNeeded = true;

		// Initialize 
		hres = CoInitializeSecurity(
			NULL,
			-1,      // COM negotiates service                  
			NULL,    // Authentication services
			NULL,    // Reserved
			RPC_C_AUTHN_LEVEL_DEFAULT,    // authentication
			RPC_C_IMP_LEVEL_IMPERSONATE,  // Impersonation
			NULL,             // Authentication info 
			EOAC_NONE,        // Additional capabilities
			NULL              // Reserved
		);

		if (FAILED(hres)) {
			cout << "Failed to initialize security. "
				<< "Error code = 0x"
				<< hex << hres << endl;
			CoUninitialize();
			isCoUninitializeNeeded = false;
			return hres;          // Program has failed.
		}
		return hres;
	}
	int COMWrapper::UnInitialize()
	{
		if (isCoUninitializeNeeded)
			CoUninitialize();
		return 0;
	}
}