#include "WinService.h"
#include <functional>
#include "BaseUtil.h"
WinService* winService = nullptr;

void __stdcall WinService::ServiceMain(DWORD dwNumServicesArgs, LPSTR* lpServiceArgVectors) {
	if (!winService)
		return;
	SERVICE_STATUS_HANDLE _statusHandle = ::RegisterServiceCtrlHandlerA(winService->name.c_str(), (LPHANDLER_FUNCTION)ServiceHandler);
	if (NULL == _statusHandle) {
		return;
	}

	winService->statusHandle = _statusHandle;
	winService->ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 0, 0, 3000);

	winService->eventStop = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (!::RegisterWaitForSingleObject(&winService->registerWaitObj, winService->eventStop, (WAITORTIMERCALLBACK)SvcStopCallback, NULL, INFINITE, WT_EXECUTEONLYONCE | WT_EXECUTELONGFUNCTION)) {
		return;
	}
	if (!winService->OnInit()) {
		winService->ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0, 0, 0);
		return;
	}

	winService->ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0, 0, 0);
	winService->Run();

	return;
}

void __stdcall WinService::ServiceHandler(DWORD dwControl) {
	switch (dwControl)
	{
	case SERVICE_CONTROL_INTERROGATE:
		return;
	case SERVICE_CONTROL_STOP:
		winService->InternalOnStop();
		return;
	case SERVICE_CONTROL_SHUTDOWN:
		winService->InternalOnShutdown();
		return;
	case SERVICE_CONTROL_PRESHUTDOWN:
		winService->InternalOnPreShutdown();
		return;
	default:
		return;
	}

}



VOID WinService::SvcStopCallback(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	if (NULL != winService->registerWaitObj)
		::UnregisterWait(winService->registerWaitObj);
	if (NULL != winService->eventStop)
		::CloseHandle(winService->eventStop);

	winService->ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0, 0, 0);
}

BOOL WinService::ReportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwSvcExitCode, DWORD dwCheckPoint, DWORD dwWaitHint)
{
	SERVICE_STATUS svcStatus = { 0 };
	svcStatus.dwServiceType = serviceType;
	svcStatus.dwCurrentState = dwCurrentState;
	svcStatus.dwWin32ExitCode = dwWin32ExitCode;
	svcStatus.dwServiceSpecificExitCode = dwSvcExitCode;
	svcStatus.dwCheckPoint = dwCheckPoint;
	svcStatus.dwWaitHint = dwWaitHint;
	switch (dwCurrentState)
	{
	case SERVICE_START_PENDING:
		svcStatus.dwControlsAccepted = 0;
		break;
	case SERVICE_STOP_PENDING:
		svcStatus.dwControlsAccepted = 0;
		break;
	case SERVICE_STOPPED:
		svcStatus.dwControlsAccepted = 0;
		break;
	case SERVICE_RUNNING:
		svcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PRESHUTDOWN | SERVICE_ACCEPT_POWEREVENT;
		break;
	default:
		break;
	}

	if (NULL == statusHandle)
		return FALSE;

	return ::SetServiceStatus(statusHandle, &svcStatus);
}



void WinService::Start()
{
	winService = this;

	entry[0].lpServiceName = (LPSTR)name.c_str();
	entry[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTIONA)ServiceMain;

	entry[1].lpServiceName = NULL;
	entry[1].lpServiceProc = NULL;

	StartServiceCtrlDispatcherA(entry);
}

void WinService::InternalOnStop()
{
	OnStop();
	ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0, 0, 5000);
	::SetEvent(eventStop);
}

void WinService::InternalOnShutdown()
{
	OnShutdown();
	ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0, 0, 5000);
	::SetEvent(eventStop);
}

void WinService::InternalOnPreShutdown()
{
	OnPreShutDown();
	ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0, 0, 5000);
	::SetEvent(eventStop);
}

WinService::WinService(std::string name, std::string description,std::string displayName)
{
	this->name = name;
	this->description = description;
	this->displayName = displayName;
	eventStop = NULL;
	registerWaitObj = NULL;
}

bool WinService::InstallService()
{
	char DirBuf[1024] = { 0 };
	GetCurrentDirectoryA(1024, DirBuf);
	GetModuleFileNameA(NULL, DirBuf, sizeof(DirBuf));

	SC_HANDLE sch = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!sch)
	{
		return FALSE;
	}
	const char* serviceName = name.c_str();
	const char* display = displayName.c_str();
	SC_HANDLE schNewSrv = CreateServiceA(sch, serviceName, display, SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START,
		SERVICE_ERROR_NORMAL, DirBuf, NULL, NULL, NULL, NULL, NULL);

	if (!schNewSrv)
	{
		return FALSE;
	}

	SERVICE_DESCRIPTIONA sd;
	sd.lpDescription = (LPSTR)description.c_str();

	ChangeServiceConfig2A(schNewSrv, SERVICE_CONFIG_DESCRIPTION, &sd);
	CloseServiceHandle(schNewSrv);
	CloseServiceHandle(sch);

	return TRUE;
}

bool WinService::UninstallService()
{
	SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!scm)
	{
		return FALSE;
	}
	const char* serviceName = name.c_str();
	SC_HANDLE scml = OpenServiceA(scm, serviceName, SC_MANAGER_ALL_ACCESS);
	if (!scml)
	{
		return FALSE;
	}
	SERVICE_STATUS ss;
	if (!QueryServiceStatus(scml, &ss))
	{
		return FALSE;
	}
	if (ss.dwCurrentState != SERVICE_STOPPED)
	{
		if (!ControlService(scml, SERVICE_CONTROL_STOP, &ss) && ss.dwCurrentState != SERVICE_CONTROL_STOP)
		{
			return FALSE;
		}
	}
	if (!DeleteService(scml))
	{
		return FALSE;
	}

	return TRUE;
}

bool WinService::InstallKernelService(const char* binaryPath, const char* serviceName, const char* display,const char* description)
{

	SC_HANDLE sch = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!sch)
	{

		return FALSE;
	}
	SC_HANDLE schNewSrv = CreateServiceA(sch, serviceName, display, SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_BOOT_START,
		SERVICE_ERROR_NORMAL, binaryPath, NULL, NULL, NULL, NULL, NULL);

	if (!schNewSrv)
	{

		return FALSE;
	}

	SERVICE_DESCRIPTIONA sd;
	sd.lpDescription = (LPSTR)description;

	ChangeServiceConfig2A(schNewSrv, SERVICE_CONFIG_DESCRIPTION, &sd);
	CloseServiceHandle(schNewSrv);
	CloseServiceHandle(sch);


	return TRUE;
}
