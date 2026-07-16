import re

with open(r'D:\STM32\LVGL\lv_font_hanzi_16.c', 'r', encoding='utf-8') as f:
    content = f.read()

cmap_start = content.find('cmaps[]')
cmap_brace = content.find('{', cmap_start)
depth = 0
cmap_end = cmap_brace
for i in range(cmap_brace, len(content)):
    if content[i] == '{': depth += 1
    elif content[i] == '}':
        depth -= 1
        if depth == 0:
            cmap_end = i
            break
cmap_body = content[cmap_brace+1:cmap_end]

ranges = re.findall(r'\.range_start\s*=\s*(\d+).*?\.range_length\s*=\s*(\d+)', cmap_body, re.DOTALL)
print('CMAP ranges in updated file:')
total = 0
for rs, rl in ranges:
    rs_int = int(rs)
    rl_int = int(rl)
    end = rs_int + rl_int - 1
    total += rl_int
    print(f'  U+{rs_int:04X}~U+{end:04X} ({rl_int} chars)')
print(f'Total: {total}')

found_latin = False
found_punct = False
for rs, rl in ranges:
    rs_int = int(rs)
    end = rs_int + int(rl) - 1
    if rs_int <= 0x00FF and end >= 0x00A0:
        found_latin = True
    if rs_int <= 0x206F and end >= 0x2000:
        found_punct = True

print(f'\nLatin-1 (0xA0-0xFF): {"YES" if found_latin else "MISSING"}')
print(f'Punctuation (0x2000-0x206F): {"YES" if found_punct else "MISSING"}')
