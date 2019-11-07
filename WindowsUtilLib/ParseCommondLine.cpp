#include "BaseUtil.h"
Vec<wchar_t*> ParseCommondLine(int argc, wchar_t* argv[]) {
	Vec<wchar_t*> args;

	for (size_t i = 0; i < argc; i++) {
		args.AppendStrPtr(argv[i]);
	}
	return args;
}