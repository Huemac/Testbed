#pragma once

class LogWrap
{
public:
	template<std::size_t N>
	LogWrap(const wchar_t(&function)[N], int line)
	{
		const auto now = std::chrono::system_clock::now();
		const auto since = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
		_buffer << since.count() << L' ' << function << L':' << line << L": ";
	}

	inline ~LogWrap()
	{
		_buffer << std::endl;
		OutputDebugString(_buffer.str().c_str());
	}

	template<typename T>
	LogWrap& operator << (const T& value)
	{
		_buffer << value;
		return *this;
	}

	template<std::size_t N>
	LogWrap& operator << (const wchar_t(&value)[N])
	{
		_buffer << value;
		return *this;
	}

private:
	std::wstringstream _buffer;
};

#define LOGD LogWrap(__FUNCTIONW__, __LINE__)