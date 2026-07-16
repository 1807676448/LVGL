"""
同步字体数据: 将原始 lv_font_hanzi_16.c 的 glyph_dsc, cmaps, kern 数据
复制到编译版本中，确保与 W25Q64 中的 bitmap 数据对齐。
"""
import re

ORIG = r'D:\STM32\LVGL\lv_font_hanzi_16.c'
DEST = r'd:\STM32\LVGL\Core\lvgl\src\font\lv_font_hanzi_16.c'

with open(ORIG, 'r', encoding='utf-8') as f:
    orig = f.read()

with open(DEST, 'r', encoding='utf-8') as f:
    dest = f.read()

# 在原始文件中定位各数据段
def find_section(content, marker, end_marker='};'):
    """Find a section starting after marker comment, ending with end_marker"""
    idx = content.find(marker)
    if idx < 0:
        print(f'  ERROR: marker "{marker}" not found')
        return None, None
    # Find the actual data start (after the comment, at the next C statement)
    # Look for the declaration line
    start = content.find('{', idx)
    if start < 0:
        return None, None
    # Count braces
    depth = 0
    end = start
    for i in range(start, len(content)):
        if content[i] == '{':
            depth += 1
        elif content[i] == '}':
            depth -= 1
            if depth == 0:
                end = i
                break
    # Include the closing }; or just }
    section = content[start+1:end]  # Content between { and }
    return start, end, section

# Extract sections from original file
print('Extracting from original...')
# glyph_dsc[]
gdsc_start, gdsc_end, gdsc_body = find_section(orig, 'glyph_dsc[]')
print(f'  glyph_dsc: {len(gdsc_body)} chars')

# cmaps[]
cmap_start, cmap_end, cmap_body = find_section(orig, 'cmaps[]')
print(f'  cmaps: {len(cmap_body)} chars')

# kern_pairs[] or kern_classes
kern_start, kern_end, kern_body = find_section(orig, 'kern_pairs')
if kern_start is None:
    kern_start, kern_end, kern_body = find_section(orig, 'kern_classes')
print(f'  kern: {len(kern_body) if kern_body else 0} chars')

# Now replace in dest file
# 1. Replace glyph_dsc
print('\nReplacing in destination...')
gdsc_dest_start, gdsc_dest_end, _ = find_section(dest, 'glyph_dsc[]')
if gdsc_dest_start and gdsc_start:
    dest = dest[:gdsc_dest_start+1] + gdsc_body + dest[gdsc_dest_end:]
    print(f'  glyph_dsc replaced: {len(gdsc_body)} chars')

# 2. Replace cmaps
cmap_dest_start, cmap_dest_end, _ = find_section(dest, 'cmaps[]')
if cmap_dest_start and cmap_start:
    dest = dest[:cmap_dest_start+1] + cmap_body + dest[cmap_dest_end:]
    print(f'  cmaps replaced: {len(cmap_body)} chars')

# 3. Replace kern_pairs
kern_dest_start, kern_dest_end, _ = find_section(dest, 'kern_pairs')
if kern_dest_start and kern_start:
    dest = dest[:kern_dest_start+1] + kern_body + dest[kern_dest_end:]
    print(f'  kern replaced: {len(kern_body)} chars')

# Also update the font_dsc if needed - check kern_scale, cmap_num, etc.
# The original might have different values

# Write back
with open(DEST, 'w', encoding='utf-8') as f:
    f.write(dest)

print(f'\nDone! Written to {DEST}')
print(f'File size: {len(dest)} chars')

# Verify: check first few glyph_dsc entries in the updated file
print('\nVerification - first 10 glyph_dsc in updated file:')
gdsc_dest_start2, gdsc_dest_end2, _ = find_section(dest, 'glyph_dsc[]')
lines = dest[gdsc_dest_start2:gdsc_dest_end2].split('\n')
count = 0
for line in lines:
    idx_m = re.search(r'bitmap_index\s*=\s*(\d+)', line)
    bw_m = re.search(r'box_w\s*=\s*(\d+)', line)
    bh_m = re.search(r'box_h\s*=\s*(\d+)', line)
    if idx_m:
        bi = idx_m.group(1)
        bw = bw_m.group(1) if bw_m else '?'
        bh = bh_m.group(1) if bh_m else '?'
        print(f'  bitmap_index={bi} {bw}x{bh}')
        count += 1
        if count >= 10:
            break
