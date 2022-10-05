.global _start

_start:
    # csrr    ct0, mhartid             # read current hart id
    # cspecialr    ct0, mhartid             # read current hart id
    # bnez    ct0, park                # single core only, park hart != 0

    lc      csp, 0x80000800           # setup stack
    j       main                    # jump to c

park:
    # wfi
    j       park
