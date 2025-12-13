'''
 bit | GPIO
  0     PA9
  1     PC7
  2     PA10
  3     PB3
  4     PB5
  5     PB4
  6     PB10
  7     PA8
'''

lut = []

for data in range(256):
    bsrr_a = 0
    bsrr_b = 0
    bsrr_c = 0

    # PA9  Bit 0 LOW  BSRR = 1 << (9 + 16)
    # PA9  Bit 0 HIGH BSRR = 1 << 9
    bit0 = (data >> 0) & 1
    if bit0:
        bsrr_a |= 1 << 9
    else:
        bsrr_a |= 1 << (9 + 16)
 
    # PC7 Bit 1 LOW  BSRR = 1 << (7 + 16)
    # PC7 Bit 1 HIGH BSRR = 1 << 7
    bit1 = (data >> 1) & 1
    if bit1:
        bsrr_c |= 1 << 7
    else:
        bsrr_c |= 1 << (7 + 16)
 
    # PA10 Bit 2 LOw  BSRR = 1 << (10 + 16)
    # PA10 Bit 2 HIGH BSRR = 1 << 10
    bit2 = (data >> 2) & 1
    if bit2:
        bsrr_a |= 1 << 10
    else:
        bsrr_a |= 1 << (10 + 16)

    # PB3  Bit 3 LOW BSRR = 1 << (3 + 16)
    # PB3  Bit 3 HIGH BSRR = 1 << 3
    bit3 = (data >> 3) & 1
    if bit3:
        bsrr_b |= 1 << 3
    else:
        bsrr_b |= 1 << (3 + 16)

    # PB5  Bit 4 LOW BSRR = 1 << (5 + 16)
    # PB5  Bit 4 HIGH BSRR = 1 << 5
    bit4 = (data >> 4) & 1
    if bit4:
        bsrr_b |= 1 << 5
    else:
        bsrr_b |= 1 << (5 + 16)
 
    # PB4  Bit 5 LOW BSRR = 1 << (4 + 16)
    # PB4  Bit 5 HIGH BSRR = 1 << 4
    bit5 = (data >> 5) & 1
    if bit5:
        bsrr_b |= 1 << 4
    else:
        bsrr_b |= 1 << (4 + 16)
 
    # PB10 Bit 6 LOW BSRR = 1 << (10 + 16)
    # PB10 Bit 6 HIGH BSRR = 1 << 10
    bit6 = (data >> 6) & 1
    if bit6:
        bsrr_b |= 1 << 10
    else:
        bsrr_b |= 1 << (10 + 16)
 
    # PA8  Bit 7 LOw  BSRR = 1 << (8 + 16)
    # PA8  Bit 7 HIGH BSRR = 1 << 8
    bit7 = (data >> 7) & 1
    if bit7:
        bsrr_a |= 1 << 8
    else:
        bsrr_a |= 1 << (8 + 16)

    lut.append((bsrr_a, bsrr_b, bsrr_c))

with open("db_lut.h", "w") as f:
    f.write("#ifndef DB_LUT_H\r\n")
    f.write("#define DB_LUT_H\r\n")
    f.write("\r\n#include <stdint.h>\r\n")
    f.write("""
typedef struct {
    uint32_t bsrrA;
    uint32_t bsrrB;
    uint32_t bsrrC;
} DBLutEntry;

// bit | GPIO
//  0     PA9
//  1     PC7
//  2     PA10
//  3     PB3
//  4     PB5
//  5     PB4
//  6     PB10
//  7     PA8
""")
    f.write("static const DBLutEntry g_dbLut[256] = {\r\n")

    for entry in lut:
        bsrr_a, bsrr_b, bsrr_c = entry
        f.write(f"    {{0x{bsrr_a:08X}, 0x{bsrr_b:08X}, 0x{bsrr_c:08x}}},\r\n")

    f.write("};\r\n\r\n")
    f.write("#endif\r\n")
    
