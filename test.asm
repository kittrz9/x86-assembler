; similar to nasm
; all numbers in hex


entry:
	mov eax #1
	mov edi #1
	mov esi str
	mov edx #C
	syscall
	mov eax #1
	mov edi #1
	mov esi str2
	mov edx #14
	syscall
	jmp asdf
	db #AA #AA
asdf:
	mov eax #3C
	mov edi #0
	syscall

aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa:
str:
	db "test string" #0A
str2:
	db "another test string" #0A
