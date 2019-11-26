#pragma once
#include <Windows.h>
#include "BaseUtil.h"
#include "StrUtil.h"
namespace IPC {
	class SharedMemory {
	public:
		SharedMemory(const wchar_t* name, unsigned int bufSize = 0);
		unsigned int Get(void*);
		bool Set(void*, unsigned int len);
	};

	class ReadEndPipe {
	public:
		ReadEndPipe(HANDLE read) { SetHandle(read); };
		~ReadEndPipe() {}
		int SyncRead(void* buf, size_t bufSize, size_t noReturnBytes = 1);
		bool SetHandle(HANDLE);
	private:
		HANDLE readHandle=INVALID_HANDLE_VALUE;

	};

	class WriteEndPipe {
	public:
		WriteEndPipe(HANDLE write) { SetHandle(write); };
		~WriteEndPipe() {
		}
		int SyncWrite(void* buf, size_t bufSize, size_t noReturnBytes = 0);
		bool SetHandle(HANDLE write);
	private:
		HANDLE writeHandle = INVALID_HANDLE_VALUE;
	};
}