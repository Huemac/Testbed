# MinimalWin32Example

- This example shows how to build an executable to the smallest possible size
	- Pay attention to the compiler options
		- NOTE: this is not recommended as it strips down security features like DEP and ASLR
			- See https://docs.microsoft.com/en-us/windows/win32/memory/data-execution-prevention and https://en.wikipedia.org/wiki/Address_space_layout_randomization fore more details