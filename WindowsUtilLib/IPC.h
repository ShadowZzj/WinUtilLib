#pragma once
#include <Windows.h>
namespace IPC {
	class SharedMemory {
	public:
		SharedMemory(const wchar_t* name, unsigned int bufSize = 0);
		unsigned int Get(void*);
		bool Set(void*, unsigned int len);
	};
}