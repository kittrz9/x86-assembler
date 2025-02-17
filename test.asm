; similar to nasm
; all numbers in hex


entry:
	mov eax #1
	mov edi #1
	mov esi str
	mov edx #c
	syscall
	mov eax str
	mov ebx *eax
	add ebx #1
	mov *eax ebx
	; I should eventually add a way to write to the memory address a register points to with a specific size instead of just always writing 4 bytes
	; idk if I'll just do what nasm does or try to devise my own weird notation
	;mov *eax #61616161
	;mov *eax ebx
	mov eax ecx
	mov eax #1
	syscall
	mov eax #1
	mov edi #1
	mov esi str2
	mov edx #14
	syscall
	jmp asdf
	db #aa #aa
asdf:
	mov eax #3c
	mov edi #0
	syscall

str:
	db "test string" #a
str2:
	db "another test string" #a
