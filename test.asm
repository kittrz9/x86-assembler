; similar to nasm
; all numbers in hex


entry:
	mov eax #1
	mov edi #1
	mov esi str
	mov edx #6
	syscall
	mov eax #1
	mov edi #1
	mov esi str2
	mov edx #7
	syscall
	jmp asdf
	db #AA #AA
asdf:
	mov eax #3C
	mov edi #0
	syscall

aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa:
str:
	db "luigi" #0A
str2:
	db "luigi2" #0A
