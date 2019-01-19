.global _kbd_test_scan_ass

.data
.global _scancode
_scancode:.long 0
makecode: .string "Makecode: 0x%x\n"
breakcode: .string "Breakcode: 0x%x\n"
makecode2bytes: .string "Makecode: 0xE0%x\n"
breakcode2bytes: .string "Breakcode: 0XE0%x\n"

.text

_kbd_test_scan_ass:
pushl %ebp
movl  %esp, %ebp



inb $0x60, %al
andl $0x000000FF, %eax
movl $0x0000FFFF, %ecx
wait://waits equivalent to tick_delay
loop wait
movl %eax, _scancode //stores the scancode in the global variable


inb $0x64, %al
movl $0x0000FFFF, %ecx
wait2://waits equivalent to tick_delay
loop wait2
andb $0xC0, %al//check the parity and timeout error
cmpb $0, %al
jne error


movl _scancode, %eax //stores the scancode in the global variable
movl 8(%ebp), %edx//stores two_bytes in %edx
cmpb $0xE0, %al //checks if the byte received is the msb of a tow bytes scancode
jne notmsb
movl $1, (%edx)//changes two_bytes to 1
jmp end

notmsb:
cmpl $1, (%edx)
jne onlyonebyte
movl $2, (%edx)//changes two_bytes to 2
jmp print2

onlyonebyte:
movl $0, (%edx)
jmp print0

print0:
movb $0x80,%dl
andb %al, %dl
cmpb $0, %dl//checks if scancode is breakcode or makecode
je print0makecode

print0breakcode:
push %eax
push $breakcode
call _printf
jmp end

print0makecode:
push %eax
push $makecode
call _printf
jmp end

print2:
movb $0x80,%dl
andb %al, %dl
cmpb $0, %dl//checks if scancode is breakcode or makecode
je print2makecode

print2breakcode:
push %eax
push $breakcode2bytes
call _printf
jmp end

print2makecode:
push %eax
push $makecode2bytes
call _printf

end:
movl $0, %eax
jmp final

error:
movl $1, %eax
final:

leave
ret
