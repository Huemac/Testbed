C:\Windows\System32\reg.exe delete "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\ExampleService.exe" /va
C:\Windows\System32\sc.exe stop ExampleService
C:\Windows\System32\sc.exe delete ExampleService
PAUSE
