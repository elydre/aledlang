.section .data
print_format:
  .asciz "%u\n"
cput_format:
  .asciz "%c"
jump_table:
  .zero 2048

.section .text
.globl main

main:

# tab 0
movl $jmp0, %eax
movl $jump_table+0, %ebx
movl %eax, (%ebx)

# tab 1
movl $jmp1, %eax
movl $jump_table+4, %ebx
movl %eax, (%ebx)

# tab 2
movl $jmp2, %eax
movl $jump_table+8, %ebx
movl %eax, (%ebx)

# tab 3
movl $jmp3, %eax
movl $jump_table+12, %ebx
movl %eax, (%ebx)

# tab 4
movl $jmp4, %eax
movl $jump_table+16, %ebx
movl %eax, (%ebx)

# 2
pushl $2

jmp4:
# 2
pushl $2

jmp0:
# KW_DUP2
pushl 4(%esp)
pushl 4(%esp)

# OP_MOD
popl %ebx
popl %eax
cltd
idiv %ebx
pushl %edx

# 0
pushl $0

# OP_EQ
popl %eax
popl %ebx
cmpl %ebx, %eax
movl $0, %ecx
movl $1, %edx
cmove %edx, %ecx
pushl %ecx

# KW_JIF (fast)
popl %eax
test %eax, %eax
jnz jmp1

# 1
pushl $1

# OP_ADD
popl %eax
popl %ebx
addl %eax, %ebx
pushl %ebx

# KW_DUP2
pushl 4(%esp)
pushl 4(%esp)

# OP_LTE
popl %eax
popl %ebx
cmpl %ebx, %eax
movl $0, %ecx
movl $1, %edx
cmovge %edx, %ecx
pushl %ecx

# KW_JIF (fast)
popl %eax
test %eax, %eax
jnz jmp2

# KW_GOTO (fast)
jmp jmp0

jmp1:
# KW_POP
popl %eax

# KW_GOTO (fast)
jmp jmp3

jmp2:
# KW_PRINT
pushl $print_format
call printf
addl $8, %esp

jmp3:
# 1
pushl $1

# OP_ADD
popl %eax
popl %ebx
addl %eax, %ebx
pushl %ebx

# KW_GOTO (fast)
jmp jmp4

# call exit
pushl $0
call exit
