.global main
.text
main:
  mov r0, #0xFABC
  mov r1, #0x25C
  mov r2, #0x8
  SWP r0, r1, [r2]
end:
    swi 0x123456
