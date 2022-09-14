#include "PCH.hpp"
#include "MainWindow.hpp"
#include "Resource.h"

int APIENTRY wWinMain(
	_In_ HINSTANCE instance,
	_In_opt_ HINSTANCE previousInstance,
	_In_ LPWSTR commandLine,
	_In_ int showCommand)
{
	UNREFERENCED_PARAMETER(previousInstance);
	UNREFERENCED_PARAMETER(commandLine);

	Example::MainWindow mainWindow(instance);

	if (!mainWindow.Show(showCommand))
	{
		return GetLastError();
	}

	MSG message = {};
	Clear(&message);

	while (GetMessage(&message, nullptr, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	return static_cast<int>(message.wParam);
}