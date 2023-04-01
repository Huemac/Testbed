#include "PCH.hpp"

#include "../ExampleSharedLibrary/ExampleSharedLibrary.h"
#include "../ExampleStaticLibrary/ExampleStaticLibrary.hpp"

int wmain(/*int argc, wchar_t* argv[], wchar_t* envp[]*/)
{
	{
		auto before = std::chrono::high_resolution_clock::now();
		ExampleSharedLibraryHello();
		auto after = std::chrono::high_resolution_clock::now();

		std::cout << "ExampleSharedLibraryHello took " << after - before << std::endl;
	}
	{
		auto before = std::chrono::high_resolution_clock::now();
		ExampleStaticLibrary::Hello();
		auto after = std::chrono::high_resolution_clock::now();

		std::cout << "ExampleStaticLibrary::Hello took " << after - before << std::endl;
	}

	return 0;
}