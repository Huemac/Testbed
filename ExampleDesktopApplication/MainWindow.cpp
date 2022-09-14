#include "PCH.hpp"
#include "Resource.h"
#include "MainWindow.hpp"

namespace Example
{
	inline BYTE FunkyColor(int offset, UINT elapsedTime)
	{
		return static_cast<BYTE>(std::sin(0.01 * elapsedTime + offset) * 127 + 128);
	}

	inline COLORREF FunkyColor(UINT elapsedTime)
	{
		return RGB(FunkyColor(0, elapsedTime), FunkyColor(2, elapsedTime), FunkyColor(4, elapsedTime));
	}

	MainWindow::MainWindow(HINSTANCE instance) :
		Window<MainWindow>(instance)
	{
	}

	MainWindow::~MainWindow()
	{
	}

	WNDCLASSEXW MainWindow::RegisterInfo() const
	{
		WNDCLASSEXW w;
		Clear(&w);

		w.cbSize = sizeof(WNDCLASSEXW);
		w.style = CS_HREDRAW | CS_VREDRAW;
		w.hIcon = LoadIcon(Instance(), MAKEINTRESOURCE(IDI_EXAMPLEAPP));
		w.hCursor = LoadCursor(Instance(), IDC_CROSS);
		w.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
		w.lpszMenuName = MAKEINTRESOURCEW(IDC_EXAMPLEAPP);
		w.lpszClassName = ClassName(MainWindow);
		w.hIconSm = LoadIcon(Instance(), MAKEINTRESOURCE(IDI_EXAMPLEAPP));

		return w;
	}

	CREATESTRUCTW MainWindow::CreateInfo() const
	{
		CREATESTRUCTW c;
		Clear(&c);

		c.x = CW_USEDEFAULT;
		c.y = CW_USEDEFAULT;
		c.cx = 800;
		c.cy = 600;

		c.style = WS_OVERLAPPEDWINDOW;
		c.lpszClass = ClassName(MainWindow);
		c.lpszName = L"Main Window";

		return c;
	}

	bool MainWindow::HandleMessage(UINT message, WPARAM wParam, LPARAM)
	{
		switch (message)
		{
			case WM_CREATE:
			{
				constexpr static UINT framesPerSecond = 60;
				_timer = SetTimer(Frame(), 1, 1000 / framesPerSecond, nullptr);

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

				return true;
			}
			case WM_PAINT:
			{
				PAINTSTRUCT paintStruct = { 0 };
				HDC context = BeginPaint(Frame(), &paintStruct);
				HBRUSH brush = CreateSolidBrush(_backgroundColor);
				FillRect(context, &paintStruct.rcPaint, brush);
				DeleteObject(brush);
				EndPaint(Frame(), &paintStruct);
				return true;
			}
			case WM_TIMER:
			{
				_backgroundColor = FunkyColor(++_elapsedTime);
				InvalidateRect(Frame(), nullptr, false);

				_progressBar.Send(PBM_STEPIT, 0, 0);

				return true;
			}
			case WM_COMMAND:
			{
				int menuSelection = LOWORD(wParam);

				switch (menuSelection)
				{
					case IDM_ABOUT:
						DialogBox(Instance(), MAKEINTRESOURCE(IDD_ABOUTBOX), Frame(), About);
						break;
					case IDM_EXIT:
						DestroyWindow(Frame());
						break;
					case IDC_HELLOBUTTON:

						std::wstring text = _textBox.Text();

						if (text.empty())
						{
							text = L"You typed nothing!";
						}

						MessageBox(Frame(), text.c_str(), L"HELLO", MB_OK);
						break;
				}

				break;
			}
			case WM_DESTROY:
			{
				if (_timer)
				{
					KillTimer(Frame(), _timer);
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