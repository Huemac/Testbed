#include "PCH.hpp"
#include "ExampleSharedLibrary.h"

namespace
{
    constexpr SHORT Width = 0x60;
    constexpr SHORT Height = 0x20;
    constexpr SMALL_RECT WindowSize = { 0, 0, Width - 1, Height - 1 };
    constexpr COORD BufferSize = { Width, Height };
    constexpr COORD CharacterBufferSize = { Width, Height };
    constexpr COORD CharacterPosition = { 0, 0 };

    WORD RandomColor()
    {
        static std::random_device randomDevice;
        static std::default_random_engine randomEngine(randomDevice());
        static const std::uniform_int_distribution<WORD> distribution(0, 256);
        return distribution(randomEngine);
    }
}

void __stdcall ExampleSharedLibraryHello()
{
    SetConsoleTitle(L"HELLO WORLD!");

    const HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleWindowInfo(outputHandle, TRUE, &WindowSize);
    SetConsoleScreenBufferSize(outputHandle, BufferSize);

    CHAR_INFO characters[Width * Height] = { 0 };

    for (CHAR_INFO& info : characters)
    {
        info.Char.UnicodeChar = L' ';
        info.Attributes = RandomColor();
    }

    SMALL_RECT ConsoleWriteArea = { 0, 0, Width - 1, Height - 1 };

    WriteConsoleOutput(outputHandle, characters, CharacterBufferSize, CharacterPosition, &ConsoleWriteArea);
}