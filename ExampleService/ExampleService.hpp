#pragma once

#include "../ExampleSharedCode/AutoHandle.hpp"

class ExampleService
{
public:
	static DWORD Start();

private:
	ExampleService();
	ExampleService(const ExampleService&) = delete;
	ExampleService(ExampleService&&) = delete;
	ExampleService& operator = (const ExampleService&) = delete;
	ExampleService& operator = (ExampleService&&) = delete;
	~ExampleService();

	bool IsPendingOperation();
	bool CanAcceptStop();

	void UpdateStatus(DWORD currentState, DWORD exitCode = NO_ERROR, DWORD waitHint = 0);

	static void WINAPI ServiceMain(DWORD, LPWSTR*);
	static DWORD WINAPI ServiceHandler(DWORD, DWORD, void*, void*);
	static DWORD WINAPI ServiceWorkerThread(void*);

	SERVICE_STATUS _status = { SERVICE_WIN32_OWN_PROCESS, 0, 0, 0, 0, 0, 0 };
	ExampleSharedCode::ServiceStatusHandle _statusHandle;
	ExampleSharedCode::EventHandle _stopEvent;
	ExampleSharedCode::ThreadHandle _thread;
};