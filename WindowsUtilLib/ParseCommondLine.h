#pragma once
#include "BaseUtil.h"
#include "Vec.h"
template<class T>
Vec<T> ParseCommondLine(int argc, T argv[]) {
	Vec<T> args;

	for (int i = 0; i < argc; i++) {
		args.AppendStrPtr(argv[i]);
	}
	return args;
}