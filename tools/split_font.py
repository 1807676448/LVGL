"""拆分 lv_font_hanzi_16.c: 位图→W25Q64, 描述表→固件"""
import os, re

SRC = r'd:\下载\lv_font_hanzi_16.c'
OUT = r'd:\STM32\LVGL\Core\lvgl\src\font\lv_font_hanzi_16.c'
BIN = r'd:\STM32\LVGL\docs\lv_font_hanzi_16_bitmap.bin'

with open(SRC, 'r', encoding='utf-8') as f:
    content = f.read()

# 1. 提取 glyph_bitmap[] 二进制
start_marker = 'static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {'
start = content.find(start_marker)
if start < 0:
    print('ERROR: glyph_bitmap not found!')
    exit(1)

depth = 0
end = start
for i in range(start, len(content)):
    if content[i] == '{':
        depth += 1
    elif content[i] == '}':
        depth -= 1
        if depth == 0:
            end = i + 2
            break

hex_vals = re.findall(r'0x([0-9a-fA-F]{2})', content[start:end])
binary_data = bytes(int(h, 16) for h in hex_vals)
print(f'Bitmap: {len(binary_data):,} bytes ({len(binary_data)/1024:.1f} KB)')

with open(BIN, 'wb') as f:
    f.write(binary_data)
print(f'Binary saved: {BIN}')

# 2. 生成 stripped C 文件
comment = '/* glyph_bitmap[] ({:.1f} KB) stored in W25Q64 Flash at 0x000000 */'.format(
    len(binary_data) / 1024.0)
stripped = content[:start] + comment + content[end:]

# 替换回调
stripped = stripped.replace(
    '.get_glyph_bitmap = lv_font_get_bitmap_fmt_txt',
    '.get_glyph_bitmap = lv_font_get_bitmap_flash')

# 设置 Flash 基地址
stripped = stripped.replace(
    '.user_data = NULL,',
    '.user_data = (void *)0x000000,  /* W25Q64 bitmap base addr */')

# 添加 include
insert_marker = '#if LV_FONT_HANZI_16'
insert_pos = stripped.find(insert_marker)
if insert_pos > 0:
    stripped = stripped[:insert_pos] + '#include "lv_font_flash.h"\n' + stripped[insert_pos:]

with open(OUT, 'w', encoding='utf-8') as f:
    f.write(stripped)

orig = os.path.getsize(SRC)
new = os.path.getsize(OUT)
print(f'\nOriginal: {orig:,} B ({orig/1024:.1f} KB)')
print(f'Stripped: {new:,} B ({new/1024:.1f} KB)')
print(f'Removed:  {orig-new:,} B ({(orig-new)/1024:.1f} KB)')
print(f'Saved: {OUT}')
print('\nDONE. Next steps:')
print('  1. Build firmware (should fit now)')
print('  2. Run burn_bitmap_to_flash.py to write bitmap to W25Q64')
