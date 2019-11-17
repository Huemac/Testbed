# ExampleSharedCode

- This is a shared code project, i.e. it's meant for sharing code snippets where a static, let alone a shared library would be an overkill.

- Drawbacks:
	- All the same drawbacks as with static linking
		- See: ExampleStaticLibrary and ExampleSharedLibrary
	- Even longer build times because there is no compiled unit to be shared, e.g. .dll or .lib