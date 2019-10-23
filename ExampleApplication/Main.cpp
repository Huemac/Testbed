#include "PCH.hpp"
#include "Timing.hpp"

#include "../ExampleLibrary/ExampleLibrary.h"

int wmain(/*int argc, wchar_t* argv[], wchar_t* envp[]*/)
{
    using namespace Timing;

    {
        Timer timer;

        ExampleLibraryHello();
    }

    return 0;
}