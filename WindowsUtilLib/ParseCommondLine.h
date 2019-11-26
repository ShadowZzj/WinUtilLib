#pragma once
#include "BaseUtil.h"
template<class T>
Vec<T> ParseCommondLine(int argc, T argv[]) {
	Vec<T> args;

	for (size_t i = 0; i < argc; i++) {
		args.AppendStrPtr(argv[i]);
	}
	return args;
}