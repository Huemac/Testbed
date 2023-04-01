# ExampleService

- This is an example Windows service
- See: https://en.wikipedia.org/wiki/Windows_service

## Debugging:

- Start Visual Studio with administrator privileges... if you dare
- Make sure ``DebugBreak()`` call exists in main() function
- Run ``EnableDebug.cmd`` in the project folder
	- Example: ``EnableDebug.cmd X:\Full\Path\To\ExampleService.exe``
	- NOTE: this only needs to be done once
- Run ``C:\Windows\System32\sc.exe start ExampleService``

- Once you are done, you can cleanup using
	- ``DisableDebug.cmd X:\Full\Path\To\ExampleService.exe``

- NOTE: you may not build the application while the service is running, so you need to stop it before re-compiling.
	- taskkill /f /pid <pid from sc start> might come in handy ;-)

- Refer to this document in case you run into troubles: https://docs.microsoft.com/en-us/windows/win32/services/debugging-a-service
