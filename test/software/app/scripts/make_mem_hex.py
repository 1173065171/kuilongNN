#!/usr/bin/env python3
#
# This is free and unencumbered software released into the public domain.
#
# Anyone is free to copy, modify, publish, use, compile, sell, or
# distribute this software, either in source code form or as a compiled
# binary, for any purpose, commercial or non-commercial, and by any
# means.

from sys import argv

base = int("0x10000000", base=16)
# print(base)
binfile = argv[1]
nwords = int(argv[2])

with open(binfile, "rb") as f:
    bindata = f.read()

# print(len(bindata))

# assert len(bindata) < 4*nwords
# assert len(bindata) % 4 == 0

for i in range(nwords):
    if i < (len(bindata) - base) // 4:
        w = bindata[4*i + base: 4*i+4 + base]
        if i == nwords - 1:
            print("%02x%02x%02x%02x" % (w[3], w[2], w[1], w[0]))
        else:
            print("%02x%02x%02x%02x" % (w[3], w[2], w[1], w[0]))
    elif i == nwords - 1:
            print("00000000")
    else:
        print("00000000")