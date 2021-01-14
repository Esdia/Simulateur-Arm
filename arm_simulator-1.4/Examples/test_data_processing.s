.global main
.text
main:
    @ Test du mov + barrel shifter
    mov r1, #5
    mov r3, #7
    mov r2, r3
    mov r3, r2, lsl r1  @ Attendu : 0xE0
    mov r3, r2, lsl #1  @ Attendu : 0xE
    mov r3, r2, lsr #1  @ Attendu : 0x3
    mov r4, #0x3
    mov r3, r4, ror #1  @ Attendu : 0x80000001
    mov r4, #0xF0000000
    mov r3, r4, asr #1 @ Attendu : 0xF8000000

    @ r1 <- 5, r2 <- 7
    and r0, r1, r2  @ 0x5
    eor r0, r1, r2  @ 0x2
    sub r0, r1, r2  @ 0xFFFFFFFE
    rsb r0, r1, r2  @ 0x2
    add r0, r1, r2  @ 0xC
    adc r0, r1, r2  @ 0xC
    sbc r0, r1, r2  @ 0xFFFFFFFD
    rsc r0, r1, r2  @ 0x1
    tst r1, r2 @ Poids fort du CPSR : 0x0
    teq r1, r2 @ Poids fort du CPSR : 0x0
    cmp r1, r2 @ Poids fort du CPSR : 0x8
    cmn r1, r2 @ Poids fort du CPSR : 0x0
    orr r0, r1, r2 @ 0x7
    bic r0, r1, r2 @ 0x0
    mvn r0, r1 @ 0xFFFFFFFA

end:
    swi 0x123456
.data
