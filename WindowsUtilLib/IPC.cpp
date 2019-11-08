#include "IPC.h"

IPC::SharedMemory::SharedMemory(const wchar_t* name, unsigned int bufSize){

}

unsigned int IPC::SharedMemory::Get(void*){
	return 0;
}

bool IPC::SharedMemory::Set(void*, unsigned int len){
	return false;
}
