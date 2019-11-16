#include "PCH.hpp"
#include "Timing.hpp"

#include "../ExampleSharedLibrary/ExampleSharedLibrary.h"
#include "../ExampleStaticLibrary/ExampleStaticLibrary.hpp"

int wmain(/*int argc, wchar_t* argv[], wchar_t* envp[]*/)
{
    using namespace Timing;

    {
        Timer timer;

        ExampleSharedLibraryHello();
    }
    {
        Timer timer;

        ExampleStaticLibrary::Hello();
    }

    return 0;
}