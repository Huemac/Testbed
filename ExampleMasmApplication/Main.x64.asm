IFDEF X64

EXTERN GetStdHandle: PROC
EXTERN WriteConsoleA: PROC
EXTERN ExitProcess: PROC

.CONST
StdOutputHandle = -11 ; STD_OUTPUT_HANDLE
HelloMessage BYTE "Hello world!", 0
HelloMessageLen = $-HelloMessage

.DATA
stdout DWORD 0
bytesWritten DWORD 0

.CODE
HelloWorldMain PROC
	; HANDLE stdout = GetStdHandle(STD_OUTPUT_HANDLE);
	mov rcx, StdOutputHandle
	call GetStdHandle
	mov stdout, eax

	; WriteConsoleA(stdout, HelloMessage, HelloMessageLen, bytesWritten, 0);
	mov ecx, stdout
	mov rdx, offset HelloMessage
	mov r8, HelloMessageLen -1 ; Exclude null terminator
	mov r9, offset bytesWritten
	push 0
	call WriteConsoleA

	; ExitProcess(0)
	mov rcx, 0
	call ExitProcess

HelloWorldMain ENDP
ENDIF
END