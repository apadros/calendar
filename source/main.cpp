#include "apad_base_types.h"
#include "apad_error.h"
#include "apad_file.h"
#include "apad_win32.h"

#include <stdio.h>
ConsoleAppEntryPoint(args, argsCount) {
	const char* path = "data/calendar.txt";
	bool b = FileExists(path);
	bool error = ErrorIsSet();
	
	auto file = LoadFile(path);
	error = ErrorIsSet();
	char* test = (char*)file.memory;
	
	printf("\n%s\n", test);
	
	return 0;
}