#include "PCH.hpp"
#include "ExampleLibrary.h"

#include <random>

namespace
{
    const SHORT Width = 0x60;
    const SHORT Height = 0x20;
    const SMALL_RECT WindowSize = { 0, 0, Width - 1, Height - 1 };
    const COORD BufferSize = { Width, Height };
    const COORD CharacterBufferSize = { Width, Height };
    const COORD CharacterPosition = { 0, 0 };

    std::random_device RandomDevice;
    std::default_random_engine RandomEngine(RandomDevice());

    WORD RandomColor()
    {
        static std::uniform_int_distribution<WORD> distribution(0, 256);
        return distribution(RandomEngine);
    }
}

void __stdcall ExampleLibraryHello()
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