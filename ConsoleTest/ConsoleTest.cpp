#include <iostream>
#include "ProcessHelper.h"
int main(int argc,char* argv[]){

	Process process;
	DWORD proId= process.GetCurrentProcessId();
	DWORD session=process.GetCurrentSessionId();
	DWORD prosId = Process::GetProcessId(L"chrome.exe");
}

