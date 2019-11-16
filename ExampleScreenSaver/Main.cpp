#include "PCH.hpp"

#pragma comment(lib, "Scrnsavw.lib")
#pragma comment(lib, "ComCtl32.lib")

namespace
{
    UINT g_elapsedTime = 0;
    UINT_PTR g_timer = 0;

    inline BYTE FunkyColor(int offset)
    {
        return static_cast<BYTE>(std::sin(0.01 * g_elapsedTime + offset) * 127 + 128);
    }

    inline HBRUSH FunkyBrush()
    {
        return CreateSolidBrush(RGB(FunkyColor(0), FunkyColor(2), FunkyColor(4)));
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

LRESULT WINAPI ScreenSaverProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CREATE:
        {
            constexpr UINT framesPerSecond = 60;
            g_timer = SetTimer(window, 1, 1000 / framesPerSecond, nullptr);
            break;
        }
        case WM_TIMER:
        {
            ++g_elapsedTime;
            HDC deviceContext = GetDC(window);
            RECT clientRectangle = { 0 };
            GetClientRect(window, &clientRectangle);
            HBRUSH brush = FunkyBrush();
            FillRect(deviceContext, &clientRectangle, brush);
            DeleteObject(brush);
            ReleaseDC(window, deviceContext);
            return 0L;
        }

        case WM_DESTROY:
        {
            if (g_timer)
            {
                KillTimer(window, g_timer);
            }

            PostQuitMessage(0);
            break;
        }
    }

    return DefScreenSaverProc(window, message, wParam, lParam);
}