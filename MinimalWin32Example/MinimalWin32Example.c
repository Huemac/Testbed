#include <Windows.h>

int main(int argc, char** argv)
{
	for (int i = 0; i < 0xFFFF; ++i)
	{
		Beep(100, 1000);
		Sleep(10000);
	}

	return 0;
}