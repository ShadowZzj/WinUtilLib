// ConsoleTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
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
int main(int argc,char* argv[])
{
	FileLoggerInit("test.log");
	Vec<char*> cmds = ParseCommondLine(argc, argv);
	defer{ DeleteVecMembers(cmds); };

	MyService myservice("myservice", "a service test for zzj","TestService");
	if(argc==1)
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

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
