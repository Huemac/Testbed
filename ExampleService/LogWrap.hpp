#pragma once

#include <Windows.h>
#include <sstream>

class LogWrap
{
public:
    template<std::size_t N>
    LogWrap(const wchar_t(&function)[N], int line)
    {
        m_buffer << function << ':' << line << L": ";
    }

    inline ~LogWrap()
    {
        m_buffer << std::endl;
        OutputDebugString(m_buffer.str().c_str());
    }

    template<typename T>
    LogWrap& operator << (const T& value)
    {
        m_buffer << value;
        return *this;
    }

    template<std::size_t N>
    LogWrap& operator << (const wchar_t(&value)[N])
    {
        m_buffer << value;
        return *this;
    }

private:
    std::wstringstream m_buffer;
};

#define LOGD LogWrap(__FUNCTIONW__, __LINE__)