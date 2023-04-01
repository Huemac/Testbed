#include "PCH.hpp"
#include "ExampleService.hpp"
#include "LogWrap.hpp"

namespace
{
	wchar_t _serviceName[15] = L"ExampleService";
}

DWORD ExampleService::Start()
{
	LOGD << L"Enter";

	const SERVICE_TABLE_ENTRY _serviceTable[2] =
	{
		{
			_serviceName,
			reinterpret_cast<LPSERVICE_MAIN_FUNCTION>(ServiceMain)
		},
		{
			nullptr,
			nullptr
		}
	};

	if (!StartServiceCtrlDispatcher(_serviceTable))
	{
		DWORD error = GetLastError();
		LOGD << L"StartServiceCtrlDispatcher failed: " << error;
		return error;
	}

	LOGD << L"Exit";
	return ERROR_SUCCESS;
}

ExampleService::ExampleService()
{
	LOGD << "ExampleService";
}

ExampleService::~ExampleService()
{
	LOGD << "~ExampleService";
}

bool ExampleService::IsPendingOperation()
{
	// See https://docs.microsoft.com/en-us/windows/win32/api/winsvc/ns-winsvc-service_status
	return _status.dwCurrentState == SERVICE_START_PENDING ||
		_status.dwCurrentState == SERVICE_STOP_PENDING ||
		_status.dwCurrentState == SERVICE_CONTINUE_PENDING ||
		_status.dwCurrentState == SERVICE_PAUSE_PENDING;
}

bool ExampleService::CanAcceptStop()
{
	return _status.dwCurrentState == SERVICE_RUNNING ||
		_status.dwCurrentState == SERVICE_PAUSED;
}

void ExampleService::UpdateStatus(DWORD currentState, DWORD exitCode, DWORD waitHint)
{
	static DWORD checkPoint = 1;

	_status.dwCurrentState = currentState;
	_status.dwControlsAccepted = CanAcceptStop() ? SERVICE_ACCEPT_STOP : 0;
	_status.dwWin32ExitCode = exitCode;
	_status.dwCheckPoint = IsPendingOperation() ? checkPoint++ : 0;
	_status.dwWaitHint = waitHint;

	if (!_statusHandle.SetStatus(&_status))
	{
		LOGD << L"SetServiceStatus failed!";
	}
}

void WINAPI ExampleService::ServiceMain(DWORD, LPWSTR*)
{
	LOGD << L"Enter";

	ExampleService service;

	service._statusHandle =
		RegisterServiceCtrlHandlerEx(_serviceName, ServiceHandler, &service);

	if (!service._statusHandle.IsValid())
	{
		LOGD << L"RegisterServiceCtrlHandler failed!";
		return;
	}

	// Tell the service controller we are starting
	service._status = { 0 };
	service._status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	service.UpdateStatus(SERVICE_START_PENDING, NO_ERROR, 0);

	// Perform tasks neccesary to start the service here
	LOGD << L"Starting service";
	service._stopEvent = CreateEvent(nullptr, true, false, nullptr);

	if (!service._stopEvent.IsValid())
	{
		LOGD << "CreateEvent failed!";
		service.UpdateStatus(SERVICE_STOPPED, GetLastError());
		return;
	}

	// Tell the service controller we are started
	service.UpdateStatus(SERVICE_RUNNING);

	// Start the thread that will perform the main task of the service
	service._thread = CreateThread(nullptr, 0, ServiceWorkerThread, &service, 0, nullptr);

	if (!service._thread.IsValid())
	{
		LOGD << L"CreateThread failed!";
		return;
	}

	LOGD << L"Service started!";
	service._thread.Wait();
	LOGD << L"Stop signaled";

	service.UpdateStatus(SERVICE_STOPPED);

	LOGD << L"Exit";
}

// https://learn.microsoft.com/en-us/windows/win32/api/winsvc/nc-winsvc-lphandler_function_ex
DWORD WINAPI ExampleService::ServiceHandler(DWORD requestCode, DWORD, void*, void* context)
{
	LOGD << L"Enter";

	auto service = reinterpret_cast<ExampleService*>(context);

	_ASSERT(service);

	switch (requestCode)
	{
		case SERVICE_CONTROL_STOP:
		{
			LOGD << L"SERVICE_CONTROL_STOP requested";

			if (service->_status.dwCurrentState != SERVICE_RUNNING)
			{
				return ERROR_SERVICE_NOT_ACTIVE;
			}

			service->UpdateStatus(SERVICE_STOP_PENDING, NO_ERROR, 10000);
			service->_stopEvent.Set();
			return NO_ERROR;
		}
		case SERVICE_CONTROL_INTERROGATE:
		{
			return NO_ERROR;
		}
	}

	return ERROR_CALL_NOT_IMPLEMENTED;
}

DWORD WINAPI ExampleService::ServiceWorkerThread(void* context)
{
	using namespace std::chrono_literals;

	LOGD << L"Enter";

	auto service = reinterpret_cast<ExampleService*>(context);

	_ASSERT(service);

	while (service->_stopEvent.Wait(5000ms) != WAIT_OBJECT_0)
	{
		LOGD << "Working...";
		// TODO: add code your service functionality here!
	}

	LOGD << L"Exit";

	return ERROR_SUCCESS;
}
