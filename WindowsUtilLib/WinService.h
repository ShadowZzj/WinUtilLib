#pragma once
#include <Windows.h>
#include <string>
#include "BaseUtil.h"
using ServiceProto = void (*)(DWORD, LPSTR*);
using HandlerProto = void(*)(DWORD);
class WinService;

class WinServiceHelper {
public:
	void AddService(std::string name, ServiceProto service, HandlerProto handler);
	void AddService(const WinService& ser);
	void StartServ();
private:
	SERVICE_TABLE_ENTRYA* serviceTable=nullptr;
	HandlerProto* handlers=nullptr;
	int serviceCount=0;

	void AddCtrlHandler(std::string name, HandlerProto handler);
};

class WinService {
public:
	friend class WinServiceHelper;
	WinService(std::string name);
	WinService(const WinService&) = delete;
	WinService& operator=(const WinService&) = delete;

	virtual BOOL OnInit()=0;
	virtual void Run() = 0;
	virtual void OnStop() = 0;
	virtual void OnShutdown() = 0;
	virtual void OnPreShutDown() = 0;
private:
	std::string name;
	DWORD serviceType;
	SERVICE_STATUS_HANDLE statusHandle;
	HANDLE eventStop;
	HANDLE registerWaitObj;

	VOID NTAPI SvcStopCallback(PVOID lpParam, BOOLEAN TimerOrWaitFired);
	BOOL ReportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwSvcExitCode, DWORD dwCheckPoint, DWORD dwWaitHint);
	void WINAPI ServiceMain(DWORD   dwNumServicesArgs, LPSTR* lpServiceArgVectors);
	void WINAPI ServiceHandler(DWORD dwControl);
	void InternalOnStop();
	void InternalOnShutdown();
	void InternalOnPreShutdown();
};