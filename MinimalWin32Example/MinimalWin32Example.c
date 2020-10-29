#include <Windows.h>

int main()
{
	HANDLE stdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	WriteConsoleA(stdOut, "Hello World!", 12, NULL, NULL);
	return 0;
}