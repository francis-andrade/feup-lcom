.global _kbc_cmd_read_ass

.data

.text

_kbc_cmd_read_ass:
pushl %ebp
movl  %esp, %ebp
movl 8(%ebp), %edx //stores scancode address in edx
movl $10 , %ecx   //changes ecx to 10

cicle:

inb $0x64, %al
andb $0x01, %al//checks KBC_OBF
cmpb $0, %al
je next

inb $0x60, %al
andl $0x000000FF, %eax
movl %eax, (%edx) //stores the scancode in the corresponding variable


inb $0x64, %al
andb $0xC0, %al//check the parity and timeout error
cmpb $0, %al
jne error

next:
loop cicle

movl $0, %eax
jmp final

error:
movl $1, %eax
final:

leave
ret
