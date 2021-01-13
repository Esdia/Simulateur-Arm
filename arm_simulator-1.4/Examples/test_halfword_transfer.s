.global main
.text
main:
  mov r0, #0x00020
  mov r1, #0xAB 
  strh r1, [r0]
  ldrh r2, [r0]
  swi 0x123456
.data
