#pragma once

#include <Windows.h>

namespace Example
{
#define ClassName(x) L#x

	class MainWindow
	{
	public:
		MainWindow();
		~MainWindow();

		bool InitInstance(HINSTANCE, int);

	private:
		ATOM Register(HINSTANCE);

		static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
		static INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

		std::wstring_view _title = L"Main Window";
		std::wstring_view _windowClassName = ClassName(MainWindow);
		HWND _textBox = nullptr;
		HWND _button = nullptr;
		HWND _progressBar = nullptr;
		UINT_PTR _timer = 0;
		UINT _elapsedTime = 0;
		COLORREF _backgroundColor = { 0 };
		HINSTANCE _instance = nullptr;
	};
}