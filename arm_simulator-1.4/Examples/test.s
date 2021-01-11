.global main
.text
main:
    mov r1, #5
    mov r3, #7
    mov r2, r3
    mov r3, r2, lsl r1
    mov r3, r2, lsl #1

    and r0, r1, r2
    eor r0, r1, r2
    sub r0, r1, r2
    rsb r0, r1, r2
    add r0, r1, r2
    adc r0, r1, r2
    sbc r0, r1, r2
    rsc r0, r1, r2
    tst r1, r2
    teq r1, r2
    cmp r1, r2
    cmn r1, r2
    orr r0, r1, r2
    bic r0, r1, r2
    mvn r0, r1

    mrs r1, cpsr_all
    msr cpsr_flg, r1

    mov r1, #5

    mul r0, r1, r2
    mla r0, r1, r2, r3

    umull r5, r6, r1, r2
    umlal r5, r6, r1, r2
    smull r5, r6, r1, r2
    smlal r5, r6, r1, r2

    ldr r1, [r2]
    str r1, [r2]
    ldrb r1, [r2]
    strb r1, [r2]
    ldrh r1, [r2]
    ldrsh r1, [r2]
    ldrsb r1, [r2]
    strh r1, [r2]
    
    ldmfd sp!, {r0, r1, r2}
    stmia r0, {r0-r15}

    swp r0, r1, [r2]
    swpb r0, r1, [r2]

    bx lr
    b end
    bl end

end:
    swi 0x123456
.data
