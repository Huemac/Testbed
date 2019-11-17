C:\Windows\System32\reg.exe add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\ExampleService.exe" /v Debugger /t REG_SZ /d C:\Windows\System32\vsjitdebugger.exe
C:\Windows\System32\sc.exe create ExampleService binpath= %1 DisplayName= "Example Service" start= demand
:: C:\Windows\System32\sc.exe start ExampleService
PAUSE