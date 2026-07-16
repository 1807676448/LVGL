"""
Extract glyph_bitmap[] binary data from the original lv_font_hanzi_16.c
and write to a .bin file for burning to W25Q64 Flash.
"""
import re
import os

src_path = r'D:\STM32\LVGL\lv_font_hanzi_16.c'
dst_path = r'D:\STM32\LVGL\docs\lv_font_hanzi_16_bitmap.bin'

with open(src_path, 'r', encoding='utf-8') as f:
    content = f.read()

# Find the glyph_bitmap[] array bounds
start_marker = 'static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {'
start_idx = content.find(start_marker)
if start_idx < 0:
    print("ERROR: glyph_bitmap[] not found!")
    exit(1)

# Find the closing }; of the array
# Skip past the opening brace
brace_start = content.find('{', start_idx)
depth = 0
end_idx = brace_start
for i in range(brace_start, len(content)):
    if content[i] == '{':
        depth += 1
    elif content[i] == '}':
        depth -= 1
        if depth == 0:
            end_idx = i
            break

array_body = content[brace_start+1:end_idx]
print(f"Array body: {len(array_body)} chars")

# Extract all hex values: 0xNN or 0xN (1-2 hex digits)
hex_pattern = re.compile(r'0x([0-9a-fA-F]{1,2})\b')
matches = hex_pattern.findall(array_body)
print(f"Found {len(matches)} hex bytes")

# Convert to bytes
data = bytes(int(m, 16) for m in matches)

# Verify against font_dsc max bitmap_index
# Find max bitmap_index in the file
idx_pattern = re.compile(r'\.bitmap_index\s*=\s*(\d+)')
indices = [int(x) for x in idx_pattern.findall(content)]
if indices:
    max_idx = max(indices)
    # Estimate total needed: max_index + typical glyph size (~100 bytes)
    needed = max_idx + 200
    print(f"Max bitmap_index: {max_idx} (0x{max_idx:X})")
    print(f"Estimated total needed: ~{needed} bytes")
    print(f"Extracted data size: {len(data)} bytes")
    if len(data) < max_idx:
        print(f"WARNING: Extracted data ({len(data)}) < max bitmap_index ({max_idx})!")
        print(f"         Missing approximately {max_idx - len(data)} bytes")

# Write binary
os.makedirs(os.path.dirname(dst_path), exist_ok=True)
with open(dst_path, 'wb') as f:
    f.write(data)

print(f"\nWritten: {dst_path}")
print(f"Size: {len(data)} bytes ({len(data)/1024:.1f} KB)")
