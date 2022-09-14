#pragma once

#include "Widget.hpp"

namespace Example
{
#define ClassName(x) L#x

	template <typename Derived>
	class Window
	{
	public:
		static LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
		{
			Derived* derived = nullptr;

			if (message == WM_NCCREATE)
			{
				auto created = reinterpret_cast<CREATESTRUCT*>(lParam);
				derived = reinterpret_cast<Derived*>(created->lpCreateParams);
				SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(derived));
				derived->_frame = window;
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

		Window(HINSTANCE instance = GetModuleHandle(nullptr)) :
			_instance(instance)
		{
		}

		virtual ~Window()
		{
		}

		Widget AddWidget(
			const wchar_t* className,
			const wchar_t* windowName,
			DWORD style,
			int x,
			int y,
			int w,
			int h,
			HMENU menu = nullptr)
		{
			return Widget(className, windowName, style, x, y, w, h, _frame, menu, _instance, nullptr);
		}

		bool Show(int showCommand)
		{
			// HERE BE DRAGONS

			if (!_class)
			{
				_class = Register();
			}

			if (_class && !_frame)
			{
				HWND frame = Create();
				assert(frame == _frame);
			}

			if (!_frame)
			{
				return false;
			}

			ShowWindow(_frame, showCommand);
			return true;
		}

	protected:
		virtual WNDCLASSEXW RegisterInfo() const = 0;
		virtual CREATESTRUCTW CreateInfo() const = 0;
		virtual bool HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

		HINSTANCE Instance() const
		{
			return _instance;
		}

		HWND Frame() const
		{
			return _frame;
		}

	private:
		ATOM Register()
		{
			WNDCLASSEXW wc = RegisterInfo();
			
			if (!wc.lpfnWndProc)
			{
				wc.lpfnWndProc = Derived::WindowProc;
			}

			if (!wc.hInstance)
			{
				wc.hInstance = _instance;
			}

			return RegisterClassExW(&wc);
		}

		HWND Create()
		{
			CREATESTRUCTW c = CreateInfo();

			return CreateWindowEx(
				c.dwExStyle,
				c.lpszClass ? c.lpszClass : MAKEINTATOM(_class),
				c.lpszName,
				c.style,
				c.x,
				c.y,
				c.cx,
				c.cy,
				c.hwndParent,
				c.hMenu,
				c.hInstance ? c.hInstance : _instance,
				c.lpCreateParams ? c.lpCreateParams : this);
		}

		HINSTANCE _instance = nullptr;
		ATOM _class = 0;
		HWND _frame = nullptr;
	};
}