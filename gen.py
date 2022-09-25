names = [
    "char",
    "short",
    "int",
    "long",
    "longlong",
    "int8",
    "int16",
    "int32",
    "int64",
    "uchar",
    "ushort",
    "uint",
    "ulong",
    "ulonglong",
    "uint8",
    "uint16",
    "uint32",
    "uint64",
    "float",
    "double",
]
types = [
    "char",
    "short",
    "int",
    "long",
    "long long",
    "int8_t",
    "int16_t",
    "int32_t",
    "int64_t",
    "unsigned char",
    "unsigned short",
    "unsigned int",
    "unsigned long",
    "unsigned long long",
    "uint8_t",
    "uint16_t",
    "uint32_t",
    "uint64_t",
    "float",
    "double",
]

with open("vector.h.tmpl", "r") as f:
    tmpl = f.read()

for i in range(len(names)):
    with open(f"{names[i]}vec.h", "w") as f:
        f.write(tmpl % {"name": names[i], "nameup": names[i].upper(), "type": types[i]})
