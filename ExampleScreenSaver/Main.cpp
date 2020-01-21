#include "PCH.hpp"

#pragma comment(lib, "Scrnsavw.lib")
#pragma comment(lib, "ComCtl32.lib")
#pragma comment(lib, "Gdiplus.lib")

namespace
{
    UINT_PTR g_gdiPlusToken = 0;
    UINT g_elapsedTime = 0;
    UINT_PTR g_timer = 0;

    constexpr float pi = 3.14159265358979323846f;

    inline BYTE FunkyColor(int offset)
    {
        return static_cast<BYTE>(std::sin(0.01f * g_elapsedTime + offset) * 127 + 128);
    }

    inline Gdiplus::SolidBrush BackgroundBrush()
    {
        return Gdiplus::SolidBrush({ FunkyColor(0), FunkyColor(2), FunkyColor(4) });
    }

    inline Gdiplus::PointF Position(const RECT& paintArea)
    {
        float canvasWidth = float(paintArea.right - paintArea.left);
        float canvasHeight = float(paintArea.bottom - paintArea.top);

        float width = canvasWidth / 2;
        float height = canvasHeight / 2;
        float radius = min(canvasWidth, canvasHeight) / 3;

        float degrees = float(g_elapsedTime % 360); 
        float radians = degrees * (pi / 180.0f);
        
        float x = width + radius * std::sin(radians);
        float y = height - radius * std::cos(radians);
        
        return { x, y };
    }
}

BOOL WINAPI ScreenSaverConfigureDialog(HWND, UINT, WPARAM, LPARAM)
{
    return FALSE;
}

BOOL WINAPI RegisterDialogClasses(HANDLE)
{
    return TRUE;
}

void Paint(HWND window)
{
    PAINTSTRUCT ps = { 0 };
    const HDC hdc = BeginPaint(window, &ps);

    Gdiplus::Bitmap bitmap(ps.rcPaint.right, ps.rcPaint.bottom);
    Gdiplus::Graphics buffer(&bitmap);

    const Gdiplus::SolidBrush backgroundBrush = BackgroundBrush();
    buffer.FillRectangle(&backgroundBrush, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom);

    const float fontSize = (ps.rcPaint.bottom - ps.rcPaint.top) / 12.0f;
    const Gdiplus::Font font(L"Segoe", fontSize, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::PointF position = Position(ps.rcPaint);
    const static Gdiplus::SolidBrush textBrush({ 255, 255, 255 });

    // buffer.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias); Does not work with text
    buffer.SetTextRenderingHint(Gdiplus::TextRenderingHint::TextRenderingHintAntiAlias);

    const std::wstring message = L"Example!";
    buffer.DrawString(message.c_str(), static_cast<int>(message.size()), &font, position, &textBrush);

    Gdiplus::Graphics graphics(hdc);
    graphics.DrawImage(&bitmap, ps.rcPaint.left, ps.rcPaint.top);

    EndPaint(window, &ps);
}

LRESULT WINAPI ScreenSaverProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CREATE:
        {
            Gdiplus::GdiplusStartupInput gdiPlusStartupInput;
            Gdiplus::GdiplusStartup(&g_gdiPlusToken, &gdiPlusStartupInput, nullptr);

            constexpr UINT framesPerSecond = 60;
            g_timer = SetTimer(window, 1, 1000 / framesPerSecond, nullptr);
            break;
        }
        case WM_TIMER:
        {
            ++g_elapsedTime;
            InvalidateRect(window, nullptr, false);
            break;
        }
        case WM_PAINT:
        {
            Paint(window);
            break;
        }
        case WM_DESTROY:
        {
            if (g_timer)
            {
                KillTimer(window, g_timer);
            }

            if (g_gdiPlusToken)
            {
                Gdiplus::GdiplusShutdown(g_gdiPlusToken);
            }

            PostQuitMessage(0);
            break;
        }
        default:
        {
            return DefScreenSaverProc(window, message, wParam, lParam);
        }
    }

    return EXIT_SUCCESS;
}