#pragma once

#include <chrono>
#include <iostream>

namespace Timing
{
    using MicroSeconds = std::chrono::duration<uint64_t, std::ratio<1, 1000'000>>;
    using MilliSeconds = std::chrono::duration<uint64_t, std::ratio<1, 1000>>;
    using Seconds = std::chrono::duration<uint64_t, std::ratio<1>>;
    using Minutes = std::chrono::duration<uint64_t, std::ratio<60, 1>>;
    using Hours = std::chrono::duration<uint64_t, std::ratio<3600, 1>>;

    std::ostream& operator << (std::ostream& os, MicroSeconds us) { return os << us.count() << "us"; }
    std::ostream& operator << (std::ostream& os, MilliSeconds ms) { return os << ms.count() << "ms"; }
    std::ostream& operator << (std::ostream& os, Seconds s) { return os << s.count() << "s"; }
    std::ostream& operator << (std::ostream& os, Minutes s) { return os << s.count() << "m"; }
    std::ostream& operator << (std::ostream& os, Hours s) { return os << s.count() << "s"; }

    template <typename T>
    class Timer
    {
    public:
        Timer() = default;
        Timer(const Timer&) = delete;
        Timer(Timer&&) = delete;
        Timer& operator=(const Timer&) = delete;
        Timer& operator=(Timer&&) = delete;

        ~Timer()
        {
            std::cout << Duration() << std::endl;
        }

        T Duration() const
        {
            const auto diff = std::chrono::high_resolution_clock::now() - m_start;
            return std::chrono::duration_cast<T>(diff);
        }

    private:
        const std::chrono::high_resolution_clock::time_point m_start = std::chrono::high_resolution_clock::now();
    };
}