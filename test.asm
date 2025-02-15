; similar to nasm
; all numbers in hex


entry:
	mov eax #1
	mov edi #1
	mov esi str
	mov edx #c
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

aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa:
str:
	db "test string" #a
str2:
	db "another test string" #a
