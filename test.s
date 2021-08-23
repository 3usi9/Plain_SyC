
.macro mov32I, reg, val
    movw \reg, #:lower16:\val
    movt \reg, #:upper16:\val
.endm

.text
.global main
.type main, %function
main:
    SUB      sp, sp, #4
    MOV      r0, #10
    MOV      r0, #2
    MOV      r0, #12
    ADD      sp, sp, #4
    MOV      pc, lr
