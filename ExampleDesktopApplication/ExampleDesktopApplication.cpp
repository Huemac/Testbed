#include "PCH.hpp"
#include "Resource.h"
#include "ExampleDesktopApplication.hpp"

namespace Example
{
    HINSTANCE g_instance = nullptr;
    std::wstring g_title;
    std::wstring g_windowClassName;
    HWND g_textBox = nullptr;
    UINT g_elapsedTime = 0;

    COLORREF g_backgroundColor = { 0 };

    inline BYTE FunkyColor(int offset)
    {
        return static_cast<BYTE>(std::sin(0.01 * g_elapsedTime + offset) * 127 + 128);
    }

    inline COLORREF FunkyColor()
    {
        return RGB(FunkyColor(0), FunkyColor(2), FunkyColor(4));
    }

    std::wstring LoadStdString(HINSTANCE instance, UINT id)
    {
        std::wstring buffer(1, L'\0');
        int length = LoadString(instance, id, &buffer.front(), 0);

        if (length)
        {
            buffer.resize(length);
            LoadString(instance, id, &buffer.front(), length + 1);
        }

        return buffer;
    }

    void LoadStrings(HINSTANCE instance)
    {
        g_title = LoadStdString(instance, IDS_APP_TITLE);
        g_windowClassName = LoadStdString(instance, IDC_EXAMPLEAPP);
    }

    std::wstring GetWindowString(HWND window)
    {
        std::wstring buffer;
        int length = GetWindowTextLength(window);

        if (length)
        {
            buffer.resize(length);
            GetWindowText(window, &buffer.front(), length + 1);
        }

        return buffer;
    }

    ATOM MyRegisterClass(HINSTANCE instance)
    {
        WNDCLASSEXW windowClass = { 0 };

        windowClass.cbSize = sizeof(WNDCLASSEX);
        windowClass.style = CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = WindowProcedure;
        windowClass.cbClsExtra = 0;
        windowClass.cbWndExtra = 0;
        windowClass.hInstance = instance;
        windowClass.hIcon = LoadIcon(instance, MAKEINTRESOURCE(IDI_EXAMPLEAPP));
        windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
        windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        windowClass.lpszMenuName = MAKEINTRESOURCEW(IDC_EXAMPLEAPP);
        windowClass.lpszClassName = g_windowClassName.c_str();
        windowClass.hIconSm = LoadIcon(windowClass.hInstance, MAKEINTRESOURCE(IDI_EXAMPLEAPP));

        return RegisterClassEx(&windowClass);
    }

    BOOL InitInstance(HINSTANCE instance, int showCommand)
    {
        g_instance = instance;

        HWND window = CreateWindow(
            g_windowClassName.c_str(),
            g_title.c_str(),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            0,
            CW_USEDEFAULT,
            0,
            nullptr,
            nullptr,
            instance,
            nullptr);

        if (!window)
        {
            return FALSE;
        }

        ShowWindow(window, showCommand);
        UpdateWindow(window);

        return TRUE;
    }

    LRESULT CALLBACK WindowProcedure(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
    {
        static UINT_PTR s_timer = 0;

        switch (message)
        {
            case WM_CREATE:
            {
                constexpr UINT framesPerSecond = 60;
                s_timer = SetTimer(window, 1, 1000 / framesPerSecond, nullptr);

                g_textBox = CreateWindow(
                    L"EDIT",
                    L"",
                    WS_CHILD | WS_VISIBLE | WS_BORDER,
                    10,
                    10,
                    400,
                    20,
                    window,
                    nullptr,
                    nullptr,
                    nullptr);

                CreateWindow(
                    L"BUTTON",
                    L"?",
                    WS_CHILD | WS_VISIBLE | WS_BORDER,
                    420,
                    10,
                    70,
                    20,
                    window,
                    reinterpret_cast<HMENU>(IDC_HELLOBUTTON),
                    nullptr,
                    nullptr);

                break;
            }
            case WM_COMMAND:
            {
                int menuSelection = LOWORD(wParam);

                switch (menuSelection)
                {
                    case IDM_ABOUT:
                        DialogBox(g_instance, MAKEINTRESOURCE(IDD_ABOUTBOX), window, About);
                        break;
                    case IDM_EXIT:
                        DestroyWindow(window);
                        break;
                    case IDC_HELLOBUTTON:
                        MessageBox(window, GetWindowString(g_textBox).c_str(), L"HELLO", MB_OK);
                        break;
                }

                break;
            }
            case WM_TIMER:
            {
                ++g_elapsedTime;
                g_backgroundColor = FunkyColor();
                InvalidateRect(window, nullptr, false);
                return 0L;
            }
            case WM_PAINT:
            {
                PAINTSTRUCT paintStruct = { 0 };
                HDC context = BeginPaint(window, &paintStruct);
                HBRUSH brush = CreateSolidBrush(g_backgroundColor);
                FillRect(context, &paintStruct.rcPaint, brush);
                DeleteObject(brush);
                EndPaint(window, &paintStruct);
                return 0L;
            }
            case WM_DESTROY:
            {
                if (s_timer)
                {
                    KillTimer(window, s_timer);
                }

                PostQuitMessage(0);
                break;
            }
        }

        return DefWindowProc(window, message, wParam, lParam);
    }

    INT_PTR CALLBACK About(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
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

int APIENTRY wWinMain(
    _In_ HINSTANCE instance,
    _In_opt_ HINSTANCE previousInstance,
    _In_ LPWSTR commandLine,
    _In_ int showCommand)
{
    UNREFERENCED_PARAMETER(previousInstance);
    UNREFERENCED_PARAMETER(commandLine);

    Example::LoadStrings(instance);
    Example::MyRegisterClass(instance);

    if (!Example::InitInstance(instance, showCommand))
    {
        return FALSE;
    }

    HACCEL acceleratorTable = LoadAccelerators(instance, MAKEINTRESOURCE(IDC_EXAMPLEAPP));

    MSG message = { 0 };

    while (GetMessage(&message, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(message.hwnd, acceleratorTable, &message))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }

    return static_cast<int>(message.wParam);
}