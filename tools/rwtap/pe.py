"""tiny PE reader: file bytes at a VA of the original MFL.exe (no pefile dependency)"""
import struct
class PE:
    def __init__(s, path):
        d = s.d = open(path, 'rb').read(); pe = struct.unpack_from('<I', d, 0x3c)[0]
        n = struct.unpack_from('<H', d, pe + 6)[0]; osz = struct.unpack_from('<H', d, pe + 20)[0]
        s.base = struct.unpack_from('<I', d, pe + 52)[0]; s.sec = []
        for i in range(n):
            o = pe + 24 + osz + 40 * i; vs, va, rs, ro = struct.unpack_from('<IIII', d, o + 8)
            s.sec.append((s.base + va, max(vs, rs), ro, rs))
    def read(s, va, n):
        for a, sz, ro, rs in s.sec:
            if a <= va < a + sz: return s.d[ro + va - a: ro + va - a + n]
        raise KeyError(hex(va))
