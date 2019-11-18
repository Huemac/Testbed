#include "PCH.hpp"
#include "Resource.h"
#include "MainWindow.hpp"

namespace Example
{
    MainWindow* g_mainWindow = nullptr;

    inline BYTE FunkyColor(int offset, UINT elapsedTime)
    {
        return static_cast<BYTE>(std::sin(0.01 * elapsedTime + offset) * 127 + 128);
    }

    inline COLORREF FunkyColor(UINT elapsedTime)
    {
        return RGB(FunkyColor(0, elapsedTime), FunkyColor(2, elapsedTime), FunkyColor(4, elapsedTime));
    }

    std::wstring GetText(HWND window)
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

    MainWindow::MainWindow()
    {
        g_mainWindow = this;
    }

    MainWindow::~MainWindow()
    {
        g_mainWindow = nullptr;
    }

    bool MainWindow::LoadStrings(HINSTANCE instance)
    {
        m_title = LoadStdString(instance, IDS_APP_TITLE);
        m_windowClassName = LoadStdString(instance, IDC_EXAMPLEAPP);

        return !(m_title.empty() || m_windowClassName.empty());
    }

    ATOM MainWindow::Register(HINSTANCE instance)
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
        windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        windowClass.lpszMenuName = MAKEINTRESOURCEW(IDC_EXAMPLEAPP);
        windowClass.lpszClassName = m_windowClassName.c_str();
        windowClass.hIconSm = LoadIcon(instance, MAKEINTRESOURCE(IDI_EXAMPLEAPP));

        return RegisterClassEx(&windowClass);
    }

    bool MainWindow::InitInstance(HINSTANCE instance, int showCommand)
    {
        m_instance = instance;

        if (!LoadStrings(instance))
        {
            return false;
        }

        if (!Register(instance))
        {
            return false;
        }

        HWND window = CreateWindow(
            m_windowClassName.c_str(),
            m_title.c_str(),
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
            return false;
        }

        ShowWindow(window, showCommand);
        UpdateWindow(window);

        return true;
    }

    LRESULT CALLBACK MainWindow::WindowProcedure(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
            case WM_CREATE:
            {
                constexpr UINT framesPerSecond = 60;
                g_mainWindow->m_timer = SetTimer(window, 1, 1000 / framesPerSecond, nullptr);

                g_mainWindow->m_textBox = CreateWindow(
                    WC_EDIT,
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

                g_mainWindow->m_button = CreateWindow(
                    WC_BUTTON,
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

                g_mainWindow->m_progressBar = CreateWindow(
                    PROGRESS_CLASS,
                    L"",
                    WS_CHILD | WS_VISIBLE | WS_BORDER,
                    10,
                    40,
                    400,
                    20,
                    window,
                    nullptr,
                    nullptr,
                    nullptr);

                SendMessage(g_mainWindow->m_progressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
                SendMessage(g_mainWindow->m_progressBar, PBM_SETSTEP, (WPARAM)1, 0);

                break;
            }
            case WM_COMMAND:
            {
                int menuSelection = LOWORD(wParam);

                switch (menuSelection)
                {
                    case IDM_ABOUT:
                        DialogBox(g_mainWindow->m_instance, MAKEINTRESOURCE(IDD_ABOUTBOX), window, About);
                        break;
                    case IDM_EXIT:
                        DestroyWindow(window);
                        break;
                    case IDC_HELLOBUTTON:
                        MessageBox(window, GetText(g_mainWindow->m_textBox).c_str(), L"HELLO", MB_OK);
                        break;
                }

                break;
            }
            case WM_TIMER:
            {
                ++g_mainWindow->m_elapsedTime;
                g_mainWindow->m_backgroundColor = FunkyColor(g_mainWindow->m_elapsedTime);
                InvalidateRect(window, nullptr, false);

                SendMessage(g_mainWindow->m_progressBar, PBM_STEPIT, 0, 0);

                return 0L;
            }
            case WM_PAINT:
            {
                PAINTSTRUCT paintStruct = { 0 };
                HDC context = BeginPaint(window, &paintStruct);
                HBRUSH brush = CreateSolidBrush(g_mainWindow->m_backgroundColor);
                FillRect(context, &paintStruct.rcPaint, brush);
                DeleteObject(brush);
                EndPaint(window, &paintStruct);
                return 0L;
            }
            case WM_DESTROY:
            {
                if (g_mainWindow->m_timer)
                {
                    KillTimer(window, g_mainWindow->m_timer);
                }

                PostQuitMessage(0);
                break;
            }
        }

        return DefWindowProc(window, message, wParam, lParam);
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