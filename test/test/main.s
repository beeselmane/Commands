.section __TEXT,__cstring

_cstr:
    .asciz "0x%08X\n"

.section __DATA,__data

_buffer:
    .asciz "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefjdhgxcghjok[i9u8y7t6ftdcgvhbji9u8yt7fytvhbjokjaihugsyhvbjnkjogyfcgvhbjiy8ftadgsvhbjiycgfvbjadsiuyftgvhbjhuyftdcgvhuyt6resdxft6r5e4waescftreshuytfbhkoiuhyfghijklmnopqrstuvwxyz0123456789"

.section __TEXT,__text
.align 8

.extern _printf

.globl _main

_main:
    pushq %rbp
    movq %rsp, %rbp

    leaq _buffer(%rip), %rcx
    movl $0xFFFFFFFF, %edx
    movl %edx, %edi
    movb (%rcx), %sil

    1:
        crc32b %sil, %edi
        incq %rcx
        movb (%rcx), %sil
        testb %sil, %sil
        jnz 1b

    movl %edx, %eax
    movl %edi, %esi
    xorl %eax, %esi
    leaq _cstr(%rip), %rdi
    callq _printf

    xorq %rax, %rax
    movq %rbp, %rsp
    popq %rbp
    ret
