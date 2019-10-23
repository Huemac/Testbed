#include "PCH.hpp"

#pragma comment(lib, "Scrnsavw.lib")
#pragma comment(lib, "ComCtl32.lib")

namespace
{
    constexpr UINT FPS = 60;
    UINT ElapsedTime = 0;
    UINT_PTR Timer = 0;

    inline BYTE FunkyColor(int offset)
    {
        return static_cast<BYTE>(std::sin(0.01 * ElapsedTime + offset) * 127 + 128);
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
            Timer = SetTimer(window, 1, 1000 / FPS, nullptr);
            break;

        case WM_TIMER:
        {
            ++ElapsedTime;
            HDC deviceContext = GetDC(window);
            RECT clientRectangle = { 0 };
            GetClientRect(window, &clientRectangle);
            FillRect(deviceContext, &clientRectangle, FunkyBrush());
            ReleaseDC(window, deviceContext);
            break;
        }

        case WM_DESTROY:
            if (Timer)
            {
                KillTimer(window, Timer);
            }

            PostQuitMessage(0);
            break;
    }

    return DefScreenSaverProc(window, message, wParam, lParam);
}