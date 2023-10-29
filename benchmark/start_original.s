.global _start

_start:
    csrr    t0, mhartid             # read current hart id
    bnez    t0, park                # single core only, park hart != 0
    la      sp, STACK_TOP           # setup stack
    j       main                    # jump to c
    j       park

park:
    wfi
    j       park
