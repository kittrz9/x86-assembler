; similar to nasm
; all numbers in hex

str:
	;  a  s  d  f
	db 61 73 64 66 00

entry:
	mov eax 1
	mov edi 1
	mov esi @str
	mov edx 5
	syscall
	mov eax 3C
	mov edi 2
	syscall

