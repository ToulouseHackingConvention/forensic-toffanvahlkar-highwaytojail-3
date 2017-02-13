#!/bin/python3

import sys

if len(sys.argv) != 3:
    print("Usage: ", sys.argv[0], "dump-file", "dest-dir")
    exit(0)

f = open(sys.argv[1], "rb")
heap = f.read()
f.close()

keys = 0
key_path = sys.argv[2] + "/key_"

for off in range((len(heap)//16) - 1):
    display = True
    for i in range(31):
        display &= (heap[(off*0x10)+i] != heap[(off*0x10)+i+1])

    if display:
        keys += 1
        s = ''
        s += "{0:08x}".format(off*0x10) + "  "
        for i in range(32):
            if i == 8:
                s += ' '
            elif i == 16:
                s += '\n'
                s += "{0:08x}".format(off*0x10+16) + "  "
            elif i == 24:
                s += ' '
            s += "{0:02x}".format(heap[(off*0x10)+i]) + ' '
        s = s[:-1]
        print(s, end='\n\n')
        output = open(key_path + "{0:02d}".format(keys), "wb")
        output.write(heap[(off*0x10):(off*0x10)+0x20])
        output.close()

print("Found", keys, "keys !")
