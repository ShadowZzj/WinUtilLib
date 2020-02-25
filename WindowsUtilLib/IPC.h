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
	class PipeServer {
	public:
		PipeServer(std::string name) {
			this->name = name;
			std::string pipeName = "\\\\.\\pipe\\";
			pipeName += name;
			pipe = CreateNamedPipeA(
				pipeName.c_str(),             // pipe name
				PIPE_ACCESS_DUPLEX,       // read/write access
				PIPE_TYPE_MESSAGE|      // message type pipe
				PIPE_WAIT,               // blocking mode
				PIPE_UNLIMITED_INSTANCES, // max. instances 
				512,                  // output buffer size
				512,                  // input buffer size
				0,                        // client time-out
				NULL);                    // default security attribute

			if (pipe == INVALID_HANDLE_VALUE)
				CrashMe();
		}
		bool Listen() {
			bool connected = ConnectNamedPipe(pipe, NULL) ?
				TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
			return connected;
		}
		int Write(const char* str,UINT size) {
			DWORD bytesWrite;
			bool success = WriteFile(
				pipe,       
				str,    
				size,
				&bytesWrite, 
				NULL);    
			if (success)
				return bytesWrite;
			else
				return -1;
		}
		void Close() {
			CloseHandle(pipe);
		}
	private:
		HANDLE pipe;
		std::string name;
	};
}