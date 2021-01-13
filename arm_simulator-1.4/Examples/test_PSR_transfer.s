.global main
.text
main:
  mov r0, #0xFF
  MSR CPSR, r0
  MRS r1, CPSR
end:
    swi 0x123456
