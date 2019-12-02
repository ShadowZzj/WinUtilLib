#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include "BaseUtil.h"
#include "WinService.h"
#include "log.h"
class MyService :public WinService {
public:
	using WinService::WinService;

	virtual BOOL OnInit() {
		file_logger->info("init");
		return true;
	}
	virtual void Run() {
		while (1) {
			file_logger->info("Run");
			Sleep(5000);
		}
	}
	virtual void OnStop() {
		file_logger->info("stop");
	}
	virtual void OnShutdown() {
		file_logger->info("shutdown");
	}
	virtual void OnPreShutDown() {
		file_logger->info("preshutdown");
	}
};
int main(int argc, char* argv[])
{
	FileLoggerInit("test.log");
	Vec<char*> cmds = ParseCommondLine(argc, argv);
	defer{ DeleteVecMembers(cmds); };

	MyService myservice("myservice", "a service test for zzj", "TestService");
	if (argc == 1)
		myservice.Start();
	for (auto str : cmds) {
		if (str::Equal(str, "-install")) {
			myservice.InstallService();
		}
		else if (str::Equal(str, "-uninstall")) {
			myservice.UninstallService();
		}
	}
}