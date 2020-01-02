#pragma once
#include <Windows.h>
#include "paint.h"
namespace zzj {
	class Window : public Device{
	public:
		Window(HWND window):Device(window){
			Bind(window);
		}
		Window() {};
		void Bind(HWND _window);
		BOOL MoveWindowPT(int x, int y, int cx, int cy,bool isPt=false);
		BOOL CenterWindow();
		static BOOL MoveWindowPT(HWND window, int x, int y, int cx, int cy,bool isPt=false);
		static BOOL MoveWindowPT(HWND father,int controlId, int x, int y, int cx, int cy,bool isPt=false);
	private:
		bool IsWindowValid();
		HWND window=NULL;
	};
};