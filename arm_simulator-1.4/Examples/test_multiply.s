.global main
.text
main:
    mov r1, #0x8
    mov r2, #0xC
    mov r3, #0x2

    mul r0, r1, r2  @ 0x60
    mla r0, r1, r2, r3  @ 0x62

    mov r2, #0xF0000001
    mov r3, #0x3

    umull r0, r4, r2, r3    @ r4 <- 0x2, r0 <- 0xD0000003
    mov r4, #0x5
    umlal r0, r4, r2, r3    @ r4 <- 0x8, r0 <- 0xA0000006
end:
    swi 0x123456
