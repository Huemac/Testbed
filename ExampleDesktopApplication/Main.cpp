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

	Example::MainWindow mainWindow;

	if (!mainWindow.InitInstance(instance, showCommand))
	{
		return GetLastError();
	}

	HACCEL acceleratorTable = LoadAccelerators(instance, MAKEINTRESOURCE(IDC_EXAMPLEAPP));

	MSG message = { 0 };

	while (GetMessage(&message, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(message.hwnd, acceleratorTable, &message))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}

	return static_cast<int>(message.wParam);
}