; similar to nasm
; all numbers in hex

entry:
	mov eax 1
	mov edi 1
	mov esi 400000
	mov edx 171
	syscall
	mov eax 3C
	mov edi 1
	syscall

str:
	;  a  s  d  f
	db 61 73 64 66 00

