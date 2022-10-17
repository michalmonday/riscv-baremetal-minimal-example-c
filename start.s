.global _start

_start:
    #csrr    t0, mhartid             # read current hart id
    # cspecialr    ct0, mhartid             # read current hart id
    #bnez    t0, park                # single core only, park hart != 0

    #lc      csp, 0x80000800           # setup stack
    #auipcc csp, 0x80000
    #auipcc csp, 1
    auipcc csp, 512

    nop
    nop 
    nop
    ccall test
    #csc ca2, 0(csp)
    #clc 
    nop
    nop 

    # auipcc csp, 0
    # lui sp, 0x80000
    # addi sp, sp, 0x400
    # addi sp, sp, 0x400
    # j       main                    # jump to c

    ccall     main                    # jump to c

test:
    csc csp, 0(csp)
    clc ca2, 0(csp)
    csetbounds ca2, ca2, 4
    #csetbounds ca2, csp, 4
    cret

park:
    # wfi
    j       park
