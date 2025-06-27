from sys import argv
base = int("0x40000000", base=16)

binfile = argv[1]
nwords = int(argv[2])

with open(binfile, "rb") as f:
    bindata = f.read()
    
for i in range(nwords):
    if i < (len(bindata) - base) // 4:
        w = bindata[4*i + base: 4*i+4 + base]
        w3 = format(w[3], '08b') 
        w2 = format(w[2], '08b') 
        w1 = format(w[1], '08b') 
        w0 = format(w[0], '08b') 
        print(w3+w2+w1+w0)
    else:
        print("00000000000000000000000000000000")