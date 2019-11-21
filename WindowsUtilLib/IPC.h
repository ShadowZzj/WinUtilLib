#pragma once
#include <Windows.h>
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
		ReadEndPipe() {};
		ReadEndPipe(HANDLE _readHandle) { SetHandle(_readHandle); }

		void SetHandle(HANDLE _readHandle) { readHandle = _readHandle; }
		int Read(void* buf, size_t bufSize);
	private:
		HANDLE readHandle;
	};
}