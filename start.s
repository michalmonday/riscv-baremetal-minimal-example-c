.global _start

_start:
    #csrr    ct0, mhartid             # read current hart id
    #bnez    ct0, park                # single core only, park hart != 0

    #lc      csp, stack_top           # setup stack
    j       main                    # jump to c

park:
    # wfi
    j       park
