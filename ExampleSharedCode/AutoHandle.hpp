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
            return m_handle != Invalid;
        }

        // Can't get this templated
        virtual void operator = (T handle) = 0;

    protected:
        void Exhange(T handle)
        {
            if (m_handle != handle)
            {
                Close();
                std::exchange(m_handle, handle);
            }
        }

        void Move(T&& handle)
        {
        }

        T m_handle = Invalid;

    private:
        void Close()
        {
            if (IsValid())
            {
                m_deleter(m_handle);
                m_handle = Invalid;
            }
        }

        DeleteFunction m_deleter;
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
            return WaitForSingleObject(m_handle, timeout.count());
        }

        inline bool Set()
        {
            return SetEvent(m_handle);
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
            return WaitForSingleObject(m_handle, timeout.count());
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
            return SetServiceStatus(m_handle, status);
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
            return WriteFile(m_handle, buffer, numberOfBytesToWrite, numberOfBytesWritten, overlapped);
        }

        inline bool Read(LPVOID buffer, DWORD numberOfBytesToRead, LPDWORD numberOfBytesRead, LPOVERLAPPED overlapped)
        {
            return ReadFile(m_handle, buffer, numberOfBytesToRead, numberOfBytesRead, overlapped);
        }
    };
}