import os

bin_path = r'd:\STM32\LVGL\docs\lv_font_hanzi_16_bitmap.bin'
size = os.path.getsize(bin_path)
print(f'.bin size: {size} bytes (0x{size:X})')

# gid=3060 offset check
off = 0x300E1
print(f'\ngid=3060 offset=0x{off:X} = {off}')
print(f'  Exceeds file? {off >= size} (by {off - size} bytes)')

# gid=1811 check
off2 = 0x1BEBF
print(f'\ngid=1811 offset=0x{off2:X} = {off2}')
print(f'  Exceeds file? {off2 >= size}')

with open(bin_path, 'rb') as f:
    # gid=1811 at 0x1BEBF
    f.seek(0x1BEBF)
    d1 = f.read(16)
    h1 = ' '.join(f'{b:02X}' for b in d1)
    print(f'\ngid=1811 .bin:  {h1}')
    print(f'gid=1811 uart: FF FC 24 7C 1C DE 83 C0 1D B4 BB FF DD BC 34 5D')
    print(f'match: {h1 == "FF FC 24 7C 1C DE 83 C0 1D B4 BB FF DD BC 34 5D"}')

    # gid=1513 at 0x1726F
    f.seek(0x1726F)
    d3 = f.read(16)
    h3 = ' '.join(f'{b:02X}' for b in d3)
    print(f'\ngid=1513 .bin:  {h3}')
    print(f'gid=1513 uart: 47 1C 34 70 D1 FF FF FF 47 34 1C D0 70 40 FF FF')
    print(f'match: {h3 == "47 1C 34 70 D1 FF FF FF 47 34 1C D0 70 40 FF FF"}')

    # gid=3060 at 0x300E1 - read if possible
    if off < size:
        f.seek(off)
        d2 = f.read(16)
        h2 = ' '.join(f'{b:02X}' for b in d2)
        print(f'\ngid=3060 .bin:  {h2}')
        print(f'gid=3060 uart: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF')
        print(f'match: {h2 == "FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF"}')
    else:
        print(f'\ngid=3060: offset 0x{off:X} > file size 0x{size:X}, W25Q64 returns 0xFF')

# Estimate total bitmap size needed
# The largest glyph_dsc bitmap_index + glyph_size gives us the total
# Let's find the max offset from the font file
font_c_path = r'd:\STM32\LVGL\Core\lvgl\src\font\lv_font_hanzi_16.c'
with open(font_c_path, 'r', encoding='utf-8') as f:
    content = f.read()

# Find the last glyph_dsc entry
import re
# Find all bitmap_index values
indices = re.findall(r'\.bitmap_index\s*=\s*(\d+)', content)
if indices:
    indices = [int(x) for x in indices]
    max_idx = max(indices)
    print(f'\nMax bitmap_index in glyph_dsc: {max_idx} (0x{max_idx:X})')
    print(f'Min bitmap_index needed: {max_idx + 100} bytes (approx)')
    print(f'Current .bin: {size} bytes')
    print(f'Missing: {max_idx + 100 - size} bytes (approx)')
