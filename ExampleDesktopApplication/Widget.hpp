#pragma once

#include <Windows.h>
#include <cassert>

namespace Example
{
	class Widget
	{
	public:
		Widget() = default;

		Widget(
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
			void* data);

		virtual ~Widget() = default;

		std::wstring Text() const;
		LRESULT Send(UINT message, WPARAM wParam, LPARAM lParam) const;

	private:
		HWND _self = nullptr;
	};
};