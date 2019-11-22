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

	class AnonyPipe {
	public:
		AnonyPipe(){}
		AnonyPipe(HANDLE _readHandle, HANDLE _writeHandle) {
			SetReadHandle(_readHandle);
			SetWriteHandle(_writeHandle);
		}
		void SetReadHandle(HANDLE _readHandle) { readHandle = _readHandle; }
		void SetWriteHandle(HANDLE _writeHandle) { writeHandle = _writeHandle; }

		int Write(const void* data,size_t len);
		int Read(void* buf,size_t bufSize);
	private:
		HANDLE readHandle;
		HANDLE writeHandle;
	};

	class ReadEndPipe {
	public:
		ReadEndPipe(HANDLE read) { SetHandle(read); };
		~ReadEndPipe() {
		}
		int SyncRead(void* buf, size_t bufSize,size_t noReturnBytes=0) {
			if (readHandle == INVALID_HANDLE_VALUE)
				return -1;
			DWORD readBytes;
			bool success= ReadFile(readHandle, buf, bufSize, &readBytes, NULL);
			if (!success || readBytes == 0)
				return -1;
			return readBytes;
		}
		bool SetHandle(HANDLE read) {
			readHandle = read;
			return true;
		}
	private:
		HANDLE readHandle=INVALID_HANDLE_VALUE;

	};
}