#pragma once
#include <Windows.h>
#include <string>
#include "BaseUtil.h"
using ServiceProto = void (*)(DWORD, LPSTR*);
using HandlerProto = void(*)(DWORD);
class WinService;
extern WinService* winService;

//client need to override five virtual function, also it needs to provide servicename,displayname and description.
class WinService {
public:
	WinService(const WinService&) = delete;
	WinService(std::string name, std::string description, std::string displayName);
	WinService& operator=(const WinService&) = delete;

	void Start();
	virtual bool InstallService();
	virtual bool UninstallService();
	virtual BOOL OnInit()=0;
	virtual void Run() = 0;
	virtual void OnStop() = 0;
	virtual void OnShutdown() = 0;
	virtual void OnPreShutDown() = 0;
    int CheckSafeStop(int sleepSeconds);
	static int InstallService(const char* serviceName,const char* displayName,const char* description, const char* binPath);
	static bool InstallKernelService(const char* binaryPath, const char* serviceName, const char* displayName,const char* description);
	static bool MyStartService(const char* serviceName);
	static bool StopService(const char* serviceName,uint16_t waitSecond = 5);
	static bool UninstallService(const char* serviceName);
	
	static int IsServiceInstalled(const char *serviceName, bool &installed);
    static int IsServiceRunning(const char *serviceName, bool &running);

  protected:
	std::string name;
	std::string description;
	std::string displayName;
private:
	DWORD serviceType= SERVICE_WIN32_OWN_PROCESS;
	SERVICE_STATUS_HANDLE statusHandle;
	HANDLE eventStop = NULL;
	SERVICE_TABLE_ENTRYA entry[2];
	void InternalOnStop();
	void InternalOnShutdown();
	void InternalOnPreShutdown();
	BOOL ReportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwSvcExitCode, DWORD dwCheckPoint, DWORD dwWaitHint);

	static void WINAPI ServiceMain(DWORD   dwNumServicesArgs, LPSTR* lpServiceArgVectors);
	static void WINAPI ServiceHandler(DWORD dwControl);
};