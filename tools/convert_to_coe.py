import sys

ITEM_SIZE = 4 # bytes

def extend(s):
    b = s
    if len(s) < ITEM_SIZE*8:
        b = (ITEM_SIZE*8 - len(b)) * '0' + b
    return b

def extend_bin(s):
    a = bin(s)[2:]
    return extend(a)

with open(sys.argv[1], 'w') as f, open(sys.argv[2], 'rb') as g:
    f.write('memory_initialization_radix = 16;\nmemory_initialization_vector =\n')
    i = 0
    while(True):
        s = g.read(ITEM_SIZE)[::-1]
        if s:
            f.write(s.hex())
            f.write(',\n')
        else:
            f.write(';')
            break
