#include <iostream>
#include "ProcessHelper.h"
int main(int argc,char* argv[]){
	zzj::Process process;
	process.GetProcessHandle();
	process.GetProcessId();
	zzj::Process::GetProcessId(process.GetProcessHandle());
}

