#include "PCH.hpp"
#include "Resource.h"
#include "MainWindow.hpp"

namespace Example
{
	BYTE FunkyByte(UINT elapsed, BYTE offset)
	{
		float rads = (0.01f * elapsed) + (offset << 1);
		float factor = std::abs(std::sinf(rads));
		return static_cast<uint8_t>(factor * 0xFF);
	}

	DWORD FunkyColor(UINT elapsed)
	{
		struct ColorBGRA
		{
			uint8_t Blue;
			uint8_t Green;
			uint8_t Red;
			uint8_t Alpha;
		};

		union Color
		{
			uint32_t Value;
			ColorBGRA Components;
		} color;

		color.Components.Alpha = 0x00;
		color.Components.Red = FunkyByte(elapsed, 0);
		color.Components.Green = FunkyByte(elapsed, 1);
		color.Components.Blue = FunkyByte(elapsed, 2);

		return color.Value;
	}

	MainWindow::MainWindow(HINSTANCE instance) :
		Window(
			instance,
			ClassName(MainWindow),
			L"Main Window",
			800,
			600,
			LoadIcon(instance, MAKEINTRESOURCE(IDI_EXAMPLEAPP)),
			LoadCursor(instance, IDC_CROSS),
			reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1),
			MAKEINTRESOURCEW(IDC_EXAMPLEAPP))
	{
	}

	MainWindow::~MainWindow()
	{
	}

	bool MainWindow::HandleMessage(UINT message, WPARAM wParam, LPARAM)
	{
		switch (message)
		{
			case WM_CREATE:
			{
				constexpr static UINT framesPerSecond = 60;
				_timer = SetTimer(_window, 1, 1000 / framesPerSecond, nullptr);

				_textBox = AddWidget(
					WC_EDIT,
					L"",
					WS_CHILD | WS_VISIBLE | WS_BORDER,
					10,
					10,
					400,
					20);

				_button = AddWidget(
					WC_BUTTON,
					L"?",
					WS_CHILD | WS_VISIBLE | WS_BORDER,
					420,
					10,
					70,
					20,
					reinterpret_cast<HMENU>(IDC_HELLOBUTTON));

				_progressBar = AddWidget(
					PROGRESS_CLASS,
					L"",
					WS_CHILD | WS_VISIBLE | WS_BORDER,
					10,
					40,
					400,
					20);

				_progressBar.Send(PBM_SETRANGE, 0, MAKELPARAM(0, 100));
				_progressBar.Send(PBM_SETSTEP, (WPARAM)1, 0);

				// Just as an example.
				// Widget class can be subclassed and listen messages of a window or another widget.
				_button.Intercept(_window);

				return true;
			}
			case WM_PAINT:
			{
				PAINTSTRUCT paintStruct = { 0 };
				HDC context = BeginPaint(_window, &paintStruct);
				HBRUSH brush = CreateSolidBrush(_backgroundColor);
				FillRect(context, &paintStruct.rcPaint, brush);
				DeleteObject(brush);
				EndPaint(_window, &paintStruct);

				return true;
			}
			case WM_TIMER:
			{
				_backgroundColor = FunkyColor(++_elapsedTime);
				InvalidateRect(_window, nullptr, false);

				_progressBar.Send(PBM_STEPIT, 0, 0);

				return true;
			}
			case WM_COMMAND:
			{
				int menuSelection = LOWORD(wParam);

				switch (menuSelection)
				{
					case IDM_ABOUT:
						DialogBox(Instance(), MAKEINTRESOURCE(IDD_ABOUTBOX), _window, About);
						break;
					case IDM_EXIT:
						DestroyWindow(_window);
						break;
					case IDC_HELLOBUTTON:

						std::wstring text = _textBox.Text();

						if (text.empty())
						{
							text = L"You typed nothing!";
						}

						MessageBox(_window, text.c_str(), L"HELLO", MB_OK);
						break;
				}

				break;
			}
			case WM_DESTROY:
			{
				if (_timer)
				{
					KillTimer(_window, _timer);
				}

				PostQuitMessage(0);
				return true;
			}
		}

		return false;
	}

	INT_PTR CALLBACK MainWindow::About(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
	{
		UNREFERENCED_PARAMETER(lParam);

		switch (message)
		{
			case WM_INITDIALOG:
			{
				return static_cast<INT_PTR>(TRUE);
			}
			case WM_COMMAND:
			{
				if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
				{
					EndDialog(dialog, LOWORD(wParam));
					return static_cast<INT_PTR>(TRUE);
				}
				break;
			}
		}

		return static_cast<INT_PTR>(FALSE);
	}
}