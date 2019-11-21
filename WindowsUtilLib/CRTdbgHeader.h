#pragma once
#ifdef DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // DEBUG

/*
	Visual Studio runtime error code support£º Add $err,hr in watch window.
*/