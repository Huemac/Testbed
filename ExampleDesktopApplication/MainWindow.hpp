#pragma once

#include <Windows.h>

namespace Example
{
	class MainWindow
	{
	public:
		MainWindow();
		~MainWindow();

		bool InitInstance(HINSTANCE, int);

	private:
		bool LoadStrings(HINSTANCE);
		ATOM Register(HINSTANCE);

		static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
		static INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

		std::wstring m_title;
		std::wstring m_windowClassName;
		HWND m_textBox = nullptr;
		HWND m_button = nullptr;
		HWND m_progressBar = nullptr;
		UINT_PTR m_timer = 0;
		UINT m_elapsedTime = 0;
		COLORREF m_backgroundColor = { 0 };
		HINSTANCE m_instance = nullptr;
	};
}