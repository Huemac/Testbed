#include "PCH.hpp"
#include "ExampleService.hpp"
#include "LogWrap.hpp"

namespace
{
	ExampleService* g_instance = nullptr;
}

ExampleService::ExampleService()
{
	LOGD << "ExampleService";
	g_instance = this;
}

ExampleService::~ExampleService()
{
	LOGD << "~ExampleService";
	g_instance = nullptr;
}

DWORD ExampleService::Start()
{
	LOGD << L"Enter";

	const SERVICE_TABLE_ENTRY serviceTable[] =
	{
		{
			m_serviceName,
			reinterpret_cast<LPSERVICE_MAIN_FUNCTION>(ServiceMain)
		},
		{
			nullptr,
			nullptr
		}
	};

	if (!StartServiceCtrlDispatcher(serviceTable))
	{
		DWORD error = GetLastError();
		LOGD << L"StartServiceCtrlDispatcher failed: " << error;
		return error;
	}

	LOGD << L"Exit";
	return ERROR_SUCCESS;
}

bool ExampleService::IsPendingOperation()
{
	// See https://docs.microsoft.com/en-us/windows/win32/api/winsvc/ns-winsvc-service_status
	return m_serviceStatus.dwCurrentState == SERVICE_START_PENDING ||
		m_serviceStatus.dwCurrentState == SERVICE_STOP_PENDING ||
		m_serviceStatus.dwCurrentState == SERVICE_CONTINUE_PENDING ||
		m_serviceStatus.dwCurrentState == SERVICE_PAUSE_PENDING;
}

bool ExampleService::CanAcceptStop()
{
	return m_serviceStatus.dwCurrentState == SERVICE_RUNNING ||
		m_serviceStatus.dwCurrentState == SERVICE_PAUSED;
}

void ExampleService::UpdateStatus(DWORD currentState, DWORD exitCode, DWORD waitHint)
{
	static DWORD checkPoint = 1;

	m_serviceStatus.dwCurrentState = currentState;
	m_serviceStatus.dwControlsAccepted = CanAcceptStop() ? SERVICE_ACCEPT_STOP : 0;
	m_serviceStatus.dwWin32ExitCode = exitCode;
	m_serviceStatus.dwCheckPoint = IsPendingOperation() ? checkPoint++ : 0;
	m_serviceStatus.dwWaitHint = waitHint;

	if (!m_statusHandle.SetStatus(&m_serviceStatus))
	{
		LOGD << L"SetServiceStatus failed!";
	}
}

void WINAPI ExampleService::ServiceMain(DWORD, LPWSTR*)
{
	LOGD << L"Enter";
	assert(g_instance);

	g_instance->m_statusHandle =
		RegisterServiceCtrlHandler(g_instance->m_serviceName, ServiceControlHandler);

	if (!g_instance->m_statusHandle.IsValid())
	{
		LOGD << L"RegisterServiceCtrlHandler failed!";
		return;
	}

	// Tell the service controller we are starting
	g_instance->m_serviceStatus = { 0 };
	g_instance->m_serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	g_instance->UpdateStatus(SERVICE_START_PENDING, NO_ERROR, 0);

	// Perform tasks neccesary to start the service here
	LOGD << L"Starting service";
	g_instance->m_serviceStopEvent = CreateEvent(nullptr, true, false, nullptr);

	if (!g_instance->m_serviceStopEvent.IsValid())
	{
		LOGD << "CreateEvent failed!";
		g_instance->UpdateStatus(SERVICE_STOPPED, GetLastError());
		return;
	}

	// Tell the service controller we are started
	g_instance->UpdateStatus(SERVICE_RUNNING);

	// Start the thread that will perform the main task of the service
	g_instance->m_thread = CreateThread(nullptr, 0, ServiceWorkerThread, nullptr, 0, nullptr);

	if (!g_instance->m_thread.IsValid())
	{
		LOGD << L"CreateThread failed!";
		return;
	}

	LOGD << L"Service started!";
	g_instance->m_thread.Wait();
	LOGD << L"Stop signaled";

	g_instance->UpdateStatus(SERVICE_STOPPED);

	LOGD << L"Exit";
}

void WINAPI ExampleService::ServiceControlHandler(DWORD requestCode)
{
	LOGD << L"Enter";
	assert(g_instance);

	switch (requestCode)
	{
		case SERVICE_CONTROL_STOP:
		{
			LOGD << L"SERVICE_CONTROL_STOP requested";

			if (g_instance->m_serviceStatus.dwCurrentState != SERVICE_RUNNING)
			{
				break;
			}

			g_instance->UpdateStatus(SERVICE_STOP_PENDING, NO_ERROR, 10000);
			g_instance->m_serviceStopEvent.Set();
			break;
		}
	}

	LOGD << L"Exit";
}

DWORD WINAPI ExampleService::ServiceWorkerThread(void*)
{
	using namespace std::chrono_literals;

	LOGD << L"Enter";
	assert(g_instance);

	while (g_instance->m_serviceStopEvent.Wait(5000ms) != WAIT_OBJECT_0)
	{
		LOGD << "Working...";
		// TODO: add code your service functionality here!
	}

	LOGD << L"Exit";

	return ERROR_SUCCESS;
}