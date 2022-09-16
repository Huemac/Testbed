#include "PCH.hpp"
#include "Widget.hpp"

namespace Example
{
	Widget::Widget(
		DWORD extraStyle,
		const wchar_t* className,
		const wchar_t* windowName,
		DWORD style,
		int x,
		int y,
		int w,
		int h,
		HWND parent,
		HMENU menu,
		HINSTANCE instance,
		void* data) :
		_window(CreateWindowExW(
			extraStyle,
			className,
			windowName,
			style,
			x,
			y,
			w,
			h,
			parent,
			menu,
			instance,
			data))
	{
	}

	std::wstring Widget::Text() const
	{
		std::wstring buffer;
		int length = GetWindowTextLength(_window);

		if (length > 0)
		{
			buffer.resize(length);
			GetWindowText(_window, &buffer.front(), length + 1);
		}

		return buffer;
	}

	LRESULT Widget::Send(UINT message, WPARAM wParam, LPARAM lParam) const
	{
		return SendMessage(_window, message, wParam, lParam);
	}
}