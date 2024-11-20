; similar to nasm
; all numbers in hex

entry:
	mov eax 3C
	mov edi 1
	syscall

str:
	;  a  s  d  f
	db 61 73 64 66 00

