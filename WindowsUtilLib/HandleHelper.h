#pragma once
#include <Windows.h>
class ScopeKernelHandle
{
public:
	ScopeKernelHandle(HANDLE handle):handle(handle){}
	~ScopeKernelHandle() {
		if(canClose)
			CloseHandle(handle);
	}
	operator HANDLE() {
		return handle;
	}

	bool SetInherited(bool isInherited);
	bool SetCanClose(bool canClose);
private:
	bool canClose=true;
	HANDLE handle=INVALID_HANDLE_VALUE;
};
