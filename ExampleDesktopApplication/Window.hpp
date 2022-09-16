#pragma once

#include "Widget.hpp"

namespace Example
{
#define ClassName(x) L#x

	template <typename Derived>
	class Window
	{
	public:
		static LRESULT CALLBACK WindowProcedure(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
		{
			Derived* derived = nullptr;

			if (message == WM_CREATE)
			{
				auto created = reinterpret_cast<CREATESTRUCT*>(lParam);
				derived = static_cast<Derived*>(created->lpCreateParams);
				derived->_window = window;
				SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(derived));
			}
			else
			{
				LONG_PTR userData = GetWindowLongPtr(window, GWLP_USERDATA);
				derived = reinterpret_cast<Derived*>(userData);
			}

			if (derived && derived->HandleMessage(message, wParam, lParam))
			{
				return S_OK;
			}

			return DefWindowProc(window, message, wParam, lParam);
		}

		Window(
			HINSTANCE instance,
			const wchar_t* className,
			const wchar_t* title,
			int width,
			int height,
			HICON icon = nullptr,
			HCURSOR cursor = nullptr,
			HBRUSH brush = nullptr,
			const wchar_t* menuName = nullptr)
		{
			WNDCLASSEXW windowClass;
			Clear(&windowClass);
			windowClass.cbSize = sizeof(WNDCLASSEXW);
			windowClass.style = CS_HREDRAW | CS_VREDRAW;
			windowClass.lpfnWndProc = &Derived::WindowProcedure;
			windowClass.cbClsExtra = 0;
			windowClass.cbWndExtra = 0;
			windowClass.hInstance = instance;
			windowClass.hIcon = icon;
			windowClass.hCursor = cursor;
			windowClass.hbrBackground = brush;
			windowClass.lpszMenuName = menuName;
			windowClass.lpszClassName = className;

			ATOM atom = RegisterClassExW(&windowClass);

			assert(atom != 0);

			_windowStyle.lpCreateParams = this;
			_windowStyle.hInstance = instance;
			_windowStyle.hMenu = nullptr;
			_windowStyle.hwndParent = nullptr;
			_windowStyle.cy = height;
			_windowStyle.cx = width;
			_windowStyle.y = CW_USEDEFAULT;
			_windowStyle.x = CW_USEDEFAULT;
			_windowStyle.style = WS_OVERLAPPEDWINDOW;
			_windowStyle.lpszName = title;
			_windowStyle.lpszClass = MAKEINTATOM(atom);
			_windowStyle.dwExStyle = 0;
		}

		virtual ~Window()
		{
		}

		bool Show(int showCommand)
		{
			if (!_window)
			{
				HWND window = CreateWindowEx(
					_windowStyle.dwExStyle,
					_windowStyle.lpszClass,
					_windowStyle.lpszName,
					_windowStyle.style,
					_windowStyle.x,
					_windowStyle.y,
					_windowStyle.cx,
					_windowStyle.cy,
					_windowStyle.hwndParent,
					_windowStyle.hMenu,
					_windowStyle.hInstance,
					_windowStyle.lpCreateParams);

				// WindowProcedure triggers immediately after CreateWindowEx
				assert(window != nullptr);
				assert(window == _window);
			}

			ShowWindow(_window, showCommand);
			return true;
		}

	protected:
		virtual bool HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

		HINSTANCE Instance() const
		{
			return _windowStyle.hInstance;
		}

		Widget AddWidget(
			const wchar_t* className,
			const wchar_t* windowName,
			DWORD style,
			int x,
			int y,
			int w,
			int h,
			HMENU menu = nullptr,
			DWORD extraStyle = 0)
		{
			return Widget(extraStyle, className, windowName, style, x, y, w, h, _window, menu, Instance(), nullptr);
		}

		HWND _window = nullptr;

	private:
		CREATESTRUCT _windowStyle;
	};
}