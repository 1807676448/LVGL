"""
从原始 lv_font_hanzi_16.c 生成 Flash 版本：
1. 注释掉 glyph_bitmap 数组
2. 设置 get_glyph_bitmap = lv_font_get_bitmap_flash
3. 设置 user_data = (void*)0x000000
"""
import re

ORIG = r'D:\STM32\LVGL\lv_font_hanzi_16.c'
DEST = r'd:\STM32\LVGL\Core\lvgl\src\font\lv_font_hanzi_16.c'

with open(ORIG, 'r', encoding='utf-8') as f:
    content = f.read()

# 1. Fix include path for lvgl
content = content.replace('#include "lvgl/lvgl.h"', '#include "../../lvgl.h"')

# 2. Comment out the glyph_bitmap array
# Find the start of glyph_bitmap[] and wrap it in a comment
bitmap_start = content.find('static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[]')
bitmap_brace = content.find('{', bitmap_start)
# Find matching closing brace
depth = 0
bitmap_end = bitmap_brace
for i in range(bitmap_brace, len(content)):
    if content[i] == '{': depth += 1
    elif content[i] == '}':
        depth -= 1
        if depth == 0:
            bitmap_end = i
            break

# Replace glyph_bitmap with comment
before = content[:bitmap_start]
after = content[bitmap_end+1:]  # After the closing } of the array
# Add a comment
content = before + '/* glyph_bitmap[] stored in W25Q64 Flash at 0x000000 */\n' + after

# 3. Add #include for lv_font_flash.h
# Find the includes section
inc_end = content.find('/*-----------------')
if inc_end < 0:
    inc_end = content.find('/*Store the image')
if inc_end > 0:
    # Insert include before the bitmap comment
    content = content[:inc_end] + '\n#include "lv_font_flash.h"\n\n' + content[inc_end:]

# 4. Set get_glyph_bitmap callback and user_data
# Find the font struct
font_start = content.find('const lv_font_t lv_font_hanzi_16')
if font_start > 0:
    # Find .get_glyph_bitmap
    gbm_idx = content.find('.get_glyph_bitmap', font_start)
    if gbm_idx > 0:
        # Replace the line
        line_start = content.rfind('\n', 0, gbm_idx) + 1
        line_end = content.find('\n', gbm_idx)
        old_line = content[line_start:line_end]
        new_line = '    .get_glyph_bitmap = lv_font_get_bitmap_flash,    /*Function pointer to get glyph\'s bitmap*/'
        content = content[:line_start] + new_line + content[line_end:]
    
    # Find .user_data
    ud_idx = content.find('.user_data', font_start)
    if ud_idx > 0:
        line_start = content.rfind('\n', 0, ud_idx) + 1
        line_end = content.find('\n', ud_idx)
        old_line = content[line_start:line_end]
        new_line = '    .user_data = (void *)0x000000,  /* W25Q64 bitmap base addr */'
        content = content[:line_start] + new_line + content[line_end:]
    
    # Set .static_bitmap = 0
    sb_idx = content.find('.static_bitmap', font_start)
    if sb_idx > 0:
        line_start = content.rfind('\n', 0, sb_idx) + 1
        line_end = content.find('\n', sb_idx)
        old_line = content[line_start:line_end]
        new_line = '    .static_bitmap = 0,'
        content = content[:line_start] + new_line + content[line_end:]

# Write
with open(DEST, 'w', encoding='utf-8') as f:
    f.write(content)

print(f'Written: {DEST}')
print(f'Size: {len(content)} chars ({len(content)//1024} KB)')
print()
print('Changes made:')
print('  1. Fixed include path')
print('  2. Commented out glyph_bitmap[]')
print('  3. Added lv_font_flash.h include')
print('  4. Set get_glyph_bitmap = lv_font_get_bitmap_flash')
print('  5. Set user_data = (void*)0x000000')
