IFDEF X86
.486
.MODEL FLAT, C

EXTERN GetStdHandle@4: PROC
EXTERN WriteConsoleA@20: PROC
EXTERN ExitProcess@4: PROC

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
	call GetStdHandle@4

	; WriteConsoleA(stdout, HelloMessage, HelloMessageLen, bytesWritten, 0);
	push 0
	push offset bytesWritten
	push HelloMessageLen - 1 ; Exclude null terminator
	push offset HelloMessage
	push eax
	call WriteConsoleA@20

	; ExitProcess(0)
	push 0
	call ExitProcess@4

HelloWorldMain ENDP

ENDIF
END