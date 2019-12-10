#pragma once
#include <Windows.h>
//If you see a argument requires ScopedHDC, you can pass HDC and don't need to ReleaseDC in your client code, this class will handle release internally.
class ScopedHDC {
public:
	ScopedHDC(HDC hdc) :hdc(hdc) {}
	ScopedHDC(ScopedHDC& other) {
		other.StealDataTo(*this);
	}
	~ScopedHDC() { Release(); }

	ScopedHDC& operator=(ScopedHDC& other) {
		other.StealDataTo(*this);
	}
	operator HDC() {
		return hdc;
	}
private:
	HDC hdc;
	HWND hwnd;

	void Release() {
		ReleaseDC(WindowFromDC(hdc), hdc);
		hdc = NULL;
	}

	void StealDataTo(ScopedHDC& other) {
		other.Release();
		other.hdc = hdc;
		hdc = NULL;
	}
};

class Device {
public:
	struct ComposedXY
	{
		int x;
		int y;
	};
	Device(ScopedHDC hdc) :hdc(hdc) {
	}
	ComposedXY GetDpi();
	ComposedXY DLUToPixel(ComposedXY dlu);
	ComposedXY PTToPixel(ComposedXY pt);

	static HDC GetScreenHDC();
	static HDC GetHDC(HWND wndHandle);
private:
	ComposedXY GetBase();
	ScopedHDC hdc;
};
