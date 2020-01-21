#pragma once

#include <Windows.h>
#include <array>
#include "../ExampleSharedCode/AutoHandle.hpp"

class ExampleService
{
public:
	ExampleService();
	ExampleService(const ExampleService&) = delete;
	ExampleService(ExampleService&&) = delete;
	ExampleService& operator = (const ExampleService&) = delete;
	ExampleService& operator = (ExampleService&&) = delete;
	~ExampleService();

	DWORD Start();

private:
	bool IsPendingOperation();
	bool CanAcceptStop();

	void UpdateStatus(DWORD currentState, DWORD exitCode = NO_ERROR, DWORD waitHint = 0);

	static void WINAPI ServiceMain(DWORD, LPWSTR*);
	static void WINAPI ServiceControlHandler(DWORD);
	static DWORD WINAPI ServiceWorkerThread(void*);

	const std::wstring m_serviceName = L"ExampleService";
	SERVICE_STATUS m_serviceStatus = { 0 };
	ExampleSharedCode::ServiceStatusHandle m_statusHandle;
	ExampleSharedCode::EventHandle m_serviceStopEvent;
	ExampleSharedCode::ThreadHandle m_thread;
};