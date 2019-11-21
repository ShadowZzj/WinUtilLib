#pragma once
class ThreadHelper {
public:
	//Set last error code of the current thread.
	static void SetLastError(DWORD err);
	//Used to get locale of the current thread, which is useful when using CompareString.
	static LCID GetThreadLocale();

private:

};