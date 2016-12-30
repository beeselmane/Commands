.extern _printf
.globl __asm
.align 4

__asm:
    pushq %rbp
    movq %rsp, %rbp
    movq $1, %rsi

    1:
        movq $3, %r8
        movq $5, %rcx
        movq $101, %r11
        pushq %rsi
        movq %rsi, %rax
        cltd
        divq %r8
        test %edx, %edx
        jnz 2
        xor %r10, %r10

    2:
        movq %rsi, %rax
        cltd
        divq %rcx
        test %edx, %edx
        jnz 3
        xor %r9, %r9
        test %r9, %r10
        jnz 3

        // Divisible By Both
        leaq _fbstr(%rip), %rdi
        callq _printf
        jmp 6

    3:
        test %r10, %r10
        jnz 4
        // Divisible by 5
        leaq _bstr(%rip), %rdi
        callq _printf
        jmp 6

    4:
        test %r9, %r9
        jnz 5
        // Divisible by 3
        leaq _fstr(%rip), %rdi
        callq _printf
        jmp 6

    5:
        // A Number
        leaq _nstr(%rip), %rdi
        callq _printf

    6:
        popq %rsi
        movq $1, %r10
        movq $1, %r9
        incq %rsi
        cmpq %r11, %rsi
        jne 1

    popq %rbp
    ret

.section __TEXT,__cstring,cstring_literals

_fstr:
    .asciz "Fizz"
    .byte 0x0A
    .byte 0x00

_bstr:
    .asciz "Buzz"
    .byte 0x0A
    .byte 0x00

_fbstr:
    .asciz "FizzBuzz"
    .byte 0x0A
    .byte 0x00

_nstr:
    .asciz "%d"
    .byte 0x0A
    .byte 0x00
