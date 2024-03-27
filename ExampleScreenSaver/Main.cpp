#include "PCH.hpp"

#pragma comment(lib, "Scrnsavw.lib")
#pragma comment(lib, "ComCtl32.lib")
#pragma comment(lib, "Gdiplus.lib")

#ifndef _WIN64
using GdiPlusToken = ULONG_PTR;
#else	
using GdiPlusToken = UINT_PTR;
#endif

class Logger : public std::stringstream
{
public:
	Logger() :
		std::stringstream()
	{
		*this << std::chrono::system_clock::now() << ' ';
	}
	
	~Logger()
	{
		const std::string& message = str();
		OutputDebugStringA(message.c_str());
	}
};

std::ostream& operator << (std::ostream& os, const Gdiplus::RectF& r)
{
	return os << r.X << ", " << r.Y << ", " << r.Width << ", " << r.Height;
}

class ScreenSaver
{
public:
	ScreenSaver(HWND window) :
		_window(window)
	{
		Gdiplus::GdiplusStartupInput gdiPlusStartupInput;
		Gdiplus::GdiplusStartup(&_token, &gdiPlusStartupInput, nullptr);

		constexpr UINT framesPerSecond = 60;
		constexpr UINT interval = 1000 / framesPerSecond;
		_timer = SetTimer(window, 1, interval, nullptr);

		Logger() << "Created\n";
	}

	~ScreenSaver()
	{
		_bitmap.reset();

		if (_timer)
		{
			KillTimer(_window, _timer);
			_timer = 0;
		}

		if (_token)
		{
			Gdiplus::GdiplusShutdown(_token);
			_token = 0;
		}

		Logger() << "Destroyed\n";
	}

	uint8_t FunkyByte(uint8_t offset) const
	{
		float derp = std::sinf(0.01f * _elapsedTime + offset);
		return static_cast<uint8_t>(derp * 0x80 + 0x7F);
	}

	Gdiplus::ARGB FunkyColor() const
	{
		return Gdiplus::Color::MakeARGB(0xFF, FunkyByte(0), FunkyByte(2), FunkyByte(4));
	}

	Gdiplus::PointF TextPosition(const RECT& paintArea) const
	{
		float canvasWidth = float(paintArea.right - paintArea.left);
		float canvasHeight = float(paintArea.bottom - paintArea.top);

		Gdiplus::PointF position(
			(canvasWidth - _textRect.Width) / 2,
			(canvasHeight - _textRect.Height) / 2);

		float radius = min(position.X, position.Y);

		float delay = 50.0f;

		position.X += radius * std::sinf(_elapsedTime / delay);
		position.Y -= radius * std::cosf(_elapsedTime / delay);

		return position;
	}

	void MeasureText(std::wstring_view text, const Gdiplus::Font& font)
	{
		Gdiplus::Graphics graphics(_window);

		graphics.MeasureString(
			text.data(),
			static_cast<INT>(text.size()),
			&font,
			Gdiplus::PointF(),
			&_textRect);
	}

	void Resize(WORD screenWidth, WORD screenHeight)
	{
		const float fontSize = std::abs(screenWidth - screenHeight) / 2.0f;
		const Gdiplus::Font font(L"Segoe", fontSize, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
		const std::wstring message = L"Example!";

		MeasureText(message, font);

		Logger() << "Resized\n"
			<< "\tScreen width: " << screenWidth << "px\n"
			<< "\tScreen height: " << screenHeight << "px\n"
			<< "\tFont size: " << fontSize << "px\n"
			<< "\tText rectangle: " << _textRect << '\n';

		_bitmap = std::make_unique<Gdiplus::Bitmap>(
			static_cast<INT>(_textRect.Width),
			static_cast<INT>(_textRect.Height));

		Gdiplus::Graphics graphics(_bitmap.get());

		graphics.SetTextRenderingHint(Gdiplus::TextRenderingHint::TextRenderingHintAntiAlias);

		const Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 255, 255));

		graphics.DrawString(
			message.c_str(),
			static_cast<int>(message.size()),
			&font,
			_textRect,
			nullptr,
			&textBrush);
	}

	void Update()
	{
		++_elapsedTime;

		_backgroundColor.SetValue(FunkyColor());

		InvalidateRect(_window, nullptr, false);
	}

	void Paint()
	{
		HDC hdc = BeginPaint(_window, &ps);

		Gdiplus::Graphics graphics(hdc);
		graphics.Clear(_backgroundColor);

		if (_bitmap)
		{
			Gdiplus::PointF position = TextPosition(ps.rcPaint);

			graphics.DrawImage(_bitmap.get(), position);
		}
	
		EndPaint(_window, &ps);
	}

private:
	const HWND _window;
	UINT_PTR _timer = 0;
	WORD _elapsedTime = 0;

	PAINTSTRUCT ps;
	GdiPlusToken _token = 0;
	Gdiplus::Color _backgroundColor;
	Gdiplus::RectF _textRect;

	std::unique_ptr<Gdiplus::Bitmap> _bitmap;
};

std::unique_ptr<ScreenSaver> screenSaver;

BOOL WINAPI ScreenSaverConfigureDialog(HWND, UINT, WPARAM, LPARAM)
{
	return FALSE;
}

BOOL WINAPI RegisterDialogClasses(HANDLE)
{
	return TRUE;
}

LRESULT WINAPI ScreenSaverProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_CREATE:
		{
			_ASSERTE(!screenSaver);
			screenSaver = std::make_unique<ScreenSaver>(window);
			break;
		}
		case WM_DESTROY:
		{
			_ASSERTE(screenSaver);
			screenSaver.reset();
			break;
		}
		case WM_SIZE:
		{
			_ASSERTE(screenSaver);
			screenSaver->Resize(LOWORD(lParam), HIWORD(lParam));
			break;
		}
		case WM_ERASEBKGND:
		{
			return TRUE;
		}
		case WM_PAINT:
		{
			_ASSERTE(screenSaver);
			screenSaver->Paint();
			break;
		}
		case WM_TIMER:
		{
			_ASSERTE(screenSaver);
			screenSaver->Update();
			break;
		}
	}

	return DefScreenSaverProc(window, message, wParam, lParam);
}