#include "PCH.hpp"
#include "LogWrap.hpp"

namespace ExampleService
{
    SERVICE_STATUS g_serviceStatus = { 0 };
    SERVICE_STATUS_HANDLE g_statusHandle = nullptr;
    HANDLE g_serviceStopEvent = INVALID_HANDLE_VALUE;
    wchar_t ServiceName[] = L"ExampleService";

    DWORD WINAPI ServiceWorkerThread(void* /*lpParam*/)
    {
        LOGD << L"Enter";

        while (WaitForSingleObject(g_serviceStopEvent, 0) != WAIT_OBJECT_0)
        {
            // Add real work here...
            Sleep(3000);
        }

        LOGD << L"Exit";

        return ERROR_SUCCESS;
    }

    void WINAPI ServiceControlHandler(DWORD requestCode)
    {
        LOGD << L"Enter";

        switch (requestCode)
        {
            case SERVICE_CONTROL_STOP:

                LOGD << L"SERVICE_CONTROL_STOP requested";

                if (g_serviceStatus.dwCurrentState != SERVICE_RUNNING)
                {
                    break;
                }

                //  Perform tasks neccesary to stop the service here
                g_serviceStatus.dwControlsAccepted = 0;
                g_serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
                g_serviceStatus.dwWin32ExitCode = 0;
                g_serviceStatus.dwCheckPoint = 4;

                if (SetServiceStatus(g_statusHandle, &g_serviceStatus) == FALSE)
                {
                    LOGD << L"SetServiceStatus failed!";
                }

                // This will signal the worker thread to start shutting down
                SetEvent(g_serviceStopEvent);
                break;

            default:
                break;
        }

        LOGD << L"Exit";
    }

    void WINAPI ServiceMain()
    {
        LOGD << L"Enter";

        g_statusHandle = RegisterServiceCtrlHandler(ServiceName, ServiceControlHandler);

        if (g_statusHandle == nullptr)
        {
            LOGD << L"RegisterServiceCtrlHandler failed!";
            return;
        }

        // Tell the service controller we are starting
        ZeroMemory(&g_serviceStatus, sizeof(g_serviceStatus));
        g_serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
        g_serviceStatus.dwControlsAccepted = 0;
        g_serviceStatus.dwCurrentState = SERVICE_START_PENDING;
        g_serviceStatus.dwWin32ExitCode = 0;
        g_serviceStatus.dwServiceSpecificExitCode = 0;
        g_serviceStatus.dwCheckPoint = 0;

        if (SetServiceStatus(g_statusHandle, &g_serviceStatus) == FALSE)
        {
            LOGD << L"SetServiceStatus failed!";
        }

        // Perform tasks neccesary to start the service here
        LOGD << L"Starting service";
        g_serviceStopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

        if (g_serviceStopEvent == nullptr)
        {
            LOGD << "CreateEvent failed!";

            g_serviceStatus.dwControlsAccepted = 0;
            g_serviceStatus.dwCurrentState = SERVICE_STOPPED;
            g_serviceStatus.dwWin32ExitCode = GetLastError();
            g_serviceStatus.dwCheckPoint = 1;

            if (SetServiceStatus(g_statusHandle, &g_serviceStatus) == FALSE)
            {
                LOGD << L"SetServiceStatus failed!";
            }

            return;
        }

        // Tell the service controller we are started
        g_serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
        g_serviceStatus.dwCurrentState = SERVICE_RUNNING;
        g_serviceStatus.dwWin32ExitCode = 0;
        g_serviceStatus.dwCheckPoint = 0;

        if (SetServiceStatus(g_statusHandle, &g_serviceStatus) == FALSE)
        {
            LOGD << L"SetServiceStatus failed!";
        }

        // Start the thread that will perform the main task of the service
        HANDLE thread = CreateThread(nullptr, 0, ServiceWorkerThread, nullptr, 0, nullptr);

        if (!thread)
        {
            LOGD << L"CreateThread failed!";
            return;
        }

        LOGD << L"Service started!";
        WaitForSingleObject(thread, INFINITE);
        LOGD << L"Stop signaled";

        // Perform any cleanup tasks
        CloseHandle(g_serviceStopEvent);

        g_serviceStatus.dwControlsAccepted = 0;
        g_serviceStatus.dwCurrentState = SERVICE_STOPPED;
        g_serviceStatus.dwWin32ExitCode = 0;
        g_serviceStatus.dwCheckPoint = 3;

        if (SetServiceStatus(g_statusHandle, &g_serviceStatus) == FALSE)
        {
            LOGD << L"SetServiceStatus failed!";
        }

        LOGD << L"Exit";
        return;
    }

    DWORD Start()
    {
        LOGD << L"Enter";;

        SERVICE_TABLE_ENTRY ServiceTable[] =
        {
            { ServiceName, reinterpret_cast<LPSERVICE_MAIN_FUNCTION>(ServiceMain) },
            { nullptr, nullptr }
        };

        if (StartServiceCtrlDispatcher(ServiceTable) == FALSE)
        {
            DWORD error = GetLastError();
            LOGD << L"StartServiceCtrlDispatcher failed: " << error;
            return error;
        }

        LOGD << L"Exit";
        return ERROR_SUCCESS;
    }
}

int wmain(/*int argc, wchar_t* argv[], wchar_t* envp[]*/)
{
    return ExampleService::Start();
}