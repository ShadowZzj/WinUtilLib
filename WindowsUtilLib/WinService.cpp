#include "WinService.h"
#include <functional>
void WinServiceHelper::AddService(std::string name, ServiceProto service,HandlerProto handler)
{
	SERVICE_TABLE_ENTRYA* tmp;
	if (serviceTable == nullptr) {
		tmp = Allocator::Alloc<SERVICE_TABLE_ENTRYA>(nullptr, 2);
	}
	else
		tmp = (SERVICE_TABLE_ENTRYA*)Allocator::MemDup(nullptr, serviceTable, sizeof(SERVICE_TABLE_ENTRYA) *(serviceCount+1), sizeof(SERVICE_TABLE_ENTRYA));
	tmp[serviceCount].lpServiceName = (LPSTR)name.c_str();
	tmp[serviceCount].lpServiceProc = (LPSERVICE_MAIN_FUNCTIONA)service;

	tmp[serviceCount + 1].lpServiceName = NULL;
	tmp[serviceCount + 1].lpServiceProc = NULL;
	++serviceCount;

	if (serviceTable != NULL)
		Allocator::Free(nullptr, serviceTable);

	serviceTable = tmp;

	AddCtrlHandler(name, handler);
}

void WinServiceHelper::AddService(const WinService& ser)
{
	std::string name = ser.name;
	auto service = std::bind(&WinService::ServiceMain, &ser, std::placeholders::_1);
	auto handler = std::bind(&WinService::ServiceHandler, &ser, std::placeholders::_1);
	AddService(name, (ServiceProto)&service, (HandlerProto)&handler);
}

void WinServiceHelper::StartServ()
{
	StartServiceCtrlDispatcherA(serviceTable);
}

void WinServiceHelper::AddCtrlHandler(std::string name, HandlerProto handler)
{
	HandlerProto* tmp;
	tmp = (HandlerProto*)Allocator::MemDup(nullptr, handlers, sizeof(HandlerProto) * serviceCount, sizeof(HandlerProto));

	tmp[serviceCount] = handler;
	
	if (handlers != NULL)
		Allocator::Free(nullptr, handler);
	handlers = tmp;
}



void __stdcall WinService::ServiceMain(DWORD dwNumServicesArgs, LPSTR* lpServiceArgVectors){
	auto handler = std::bind(&WinService::ServiceHandler, this, std::placeholders::_1);
	SERVICE_STATUS_HANDLE _statusHandle = ::RegisterServiceCtrlHandlerA(name.c_str(), (LPHANDLER_FUNCTION)&handler);
	if (NULL == _statusHandle) {
		return;
	}

	statusHandle = _statusHandle;
	ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 0, 0, 3000);

	auto stopCallback = std::bind(&WinService::SvcStopCallback, this, std::placeholders::_1);
	eventStop = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (!::RegisterWaitForSingleObject(&registerWaitObj, eventStop, (WAITORTIMERCALLBACK)&stopCallback, NULL, INFINITE, WT_EXECUTEONLYONCE | WT_EXECUTELONGFUNCTION)) {
		return;
	}
	if (!OnInit()) {
		ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0, 0, 0);
		return;
	}

	ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0, 0, 0);
	// need to add something?

	return;
}

void __stdcall WinService::ServiceHandler(DWORD dwControl){
	switch (dwControl)
	{
	case SERVICE_CONTROL_INTERROGATE:
		return ;
	case SERVICE_CONTROL_STOP:
		InternalOnStop();
		return;
	case SERVICE_CONTROL_SHUTDOWN:
		InternalOnShutdown();
		return;
	case SERVICE_CONTROL_PRESHUTDOWN:
		InternalOnPreShutdown();
		return ;
	default:
		return;
	}

}

VOID WinService::SvcStopCallback(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	if (NULL != registerWaitObj)
		::UnregisterWait(registerWaitObj);
	if (NULL != eventStop)
		::CloseHandle(eventStop);

	ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0, 0, 0);
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

WinService::WinService(std::string name)
{
	this->name = name;
	eventStop = NULL;
	registerWaitObj = NULL;

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


