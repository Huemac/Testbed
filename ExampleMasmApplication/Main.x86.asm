IFDEF X86
.386
.MODEL TINY, SYSCALL
.STACK 1024

EXTERN _GetStdHandle@4: PROC
EXTERN _WriteConsoleA@20: PROC
EXTERN _ExitProcess@4: PROC

.CONST
StdOutputHandle = -11 ; STD_OUTPUT_HANDLE
HelloMessage BYTE "Hello world!", 0
HelloMessageLen = $-HelloMessage

.DATA
bytesWritten DWORD 0

.CODE
HelloWorldMain PROC

	; HANDLE stdout = GetStdHandle(STD_OUTPUT_HANDLE);
	push StdOutputHandle
	call _GetStdHandle@4

	; WriteConsoleA(stdout, HelloMessage, HelloMessageLen, bytesWritten, 0);
	push 0
	push offset bytesWritten
	push HelloMessageLen - 1 ; Exclude null terminator
	push offset HelloMessage
	push eax
	call _WriteConsoleA@20

	; ExitProcess(0)
	push 0
	call _ExitProcess@4

HelloWorldMain ENDP

ENDIF
END