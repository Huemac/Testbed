#pragma once

#include "Window.hpp"

namespace Example
{
	class MainWindow : public Window
	{
	public:
		MainWindow(HINSTANCE instance);
		~MainWindow();

		bool HandleMessage(UINT, WPARAM, LPARAM) override;

	private:
		static INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

		Widget _textBox;
		Widget _button;
		Widget _progressBar;

		UINT_PTR _timer = 0;
		UINT _elapsedTime = 0;
		COLORREF _backgroundColor = { 0 };
	};
}