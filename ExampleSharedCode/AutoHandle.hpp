#pragma once

#include <Windows.h>
#include <chrono>

namespace ExampleSharedCode
{
	using MilliSeconds = std::chrono::duration<DWORD, std::ratio<1, 1000>>;

	template<typename T, T Invalid, typename DeleteFunction>
	class AutoHandle
	{
	public:
		AutoHandle() = default;
		AutoHandle(const AutoHandle&) = delete;
		AutoHandle(AutoHandle&&) = delete;
		AutoHandle& operator = (const AutoHandle& other) = delete;
		AutoHandle& operator = (AutoHandle&& other) = delete;

		virtual ~AutoHandle()
		{
			Close();
		}

		bool IsValid() const
		{
			return _handle != Invalid;
		}

		// Can't get this templated
		virtual void operator = (T handle) = 0;

	protected:
		void Exhange(T handle)
		{
			if (_handle != handle)
			{
				Close();
				std::exchange(_handle, handle);
			}
		}

		void Move(T&& handle)
		{
		}

		T _handle = Invalid;

	private:
		void Close()
		{
			if (IsValid())
			{
				_deleter(_handle);
				_handle = Invalid;
			}
		}

		DeleteFunction _deleter;
	};

	struct HandleDeleter
	{
		inline void operator()(HANDLE handle)
		{
			CloseHandle(handle);
		}
	};

	struct StatusHandleDeleter
	{
		inline void operator()(SERVICE_STATUS_HANDLE)
		{
		}
	};

	class EventHandle : public AutoHandle<HANDLE, nullptr, HandleDeleter>
	{
	public:
		inline void operator = (HANDLE handle) override
		{
			Exhange(handle);
		}

		inline DWORD Wait(MilliSeconds timeout = MilliSeconds(INFINITE))
		{
			return WaitForSingleObject(_handle, timeout.count());
		}

		inline bool Set()
		{
			return SetEvent(_handle);
		}
	};

	class ThreadHandle : public AutoHandle<HANDLE, nullptr, HandleDeleter>
	{
	public:
		inline void operator = (HANDLE handle) override
		{
			Exhange(handle);
		}

		inline DWORD Wait(MilliSeconds timeout = MilliSeconds(INFINITE))
		{
			return WaitForSingleObject(_handle, timeout.count());
		}
	};

	class ServiceStatusHandle : public AutoHandle<SERVICE_STATUS_HANDLE, nullptr, StatusHandleDeleter>
	{
	public:
		inline void operator = (SERVICE_STATUS_HANDLE handle) override
		{
			Exhange(handle);
		}

		inline bool SetStatus(LPSERVICE_STATUS status)
		{
			return SetServiceStatus(_handle, status);
		}
	};

	class FileHandle : AutoHandle<HANDLE, INVALID_HANDLE_VALUE, HandleDeleter>
	{
	public:
		inline void operator = (HANDLE handle) override
		{
			Exhange(handle);
		}

		inline bool Write(LPCVOID buffer, DWORD numberOfBytesToWrite, LPDWORD numberOfBytesWritten, LPOVERLAPPED overlapped)
		{
			return WriteFile(_handle, buffer, numberOfBytesToWrite, numberOfBytesWritten, overlapped);
		}

		inline bool Read(LPVOID buffer, DWORD numberOfBytesToRead, LPDWORD numberOfBytesRead, LPOVERLAPPED overlapped)
		{
			return ReadFile(_handle, buffer, numberOfBytesToRead, numberOfBytesRead, overlapped);
		}
	};
}