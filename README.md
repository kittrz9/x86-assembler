# x86 assembler
a very basic x86 assembler that creates an executable elf file for linux<br>
<br>
it currently only supports the instructions `mov`, `jmp`, and `syscall`, and allows bytes and strings to be added directly with `db`<br>
<br>
all numbers are in hexadecimal with the prefix `#`<br>
<br>
the only registers it currently recognizes are the 32 bit ones (`eax`, `ebx`, etc.)<br>
