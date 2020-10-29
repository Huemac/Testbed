.386
.MODEL TINY, SYSCALL
.STACK 1024

EXTERN _GetStdHandle@4: PROC
EXTERN _WriteConsoleA@20: PROC
EXTERN _ExitProcess@4: PROC

_DATA SEGMENT
	StdOutputHandle = -11
	HelloMessage db "Hello world!", 0
	HelloMessageLen = $-HelloMessage
_DATA ENDS

_TEXT SEGMENT
HelloWorldMain PROC

	; HANDLE stdout = GetStdHandle(STD_OUTPUT_HANDLE);
	push StdOutputHandle
	call _GetStdHandle@4

	; WriteConsoleA(stdout, HelloMessage, HelloMessageLen, 0, 0);
	push 0
	push 0
	push HelloMessageLen
	push offset HelloMessage
	push eax
	call _WriteConsoleA@20

	; ExitProcess(0)
	push 0
	call _ExitProcess@4

HelloWorldMain ENDP
_TEXT ENDS
END