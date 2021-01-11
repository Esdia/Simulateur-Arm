.global main
main:
    mov r0, #0
    B test_3

end:
    mov r0, #6
    swi 0x123456

test_1:
    mov r0, #1
test_2:
    mov r0, #2
    bx lr
test_3:
    mov r0, #3
    BL test_2
test_4:
    mov r0, #4
test_5:
    mov r0, #5
    B end