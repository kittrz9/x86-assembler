; similar to nasm
; all numbers in hex

str:
	db "luigi" 0A

entry:
	mov eax 1
	mov edi 1
	mov esi @str
	mov edx 7
	syscall
	mov eax 3C
	mov edi 2
	syscall

