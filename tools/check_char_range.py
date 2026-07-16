"""分析 lv_font_hanzi_16 字体的字符覆盖范围"""
import re

with open(r'd:\STM32\LVGL\Core\lvgl\src\font\lv_font_hanzi_16.c', 'r', encoding='utf-8') as f:
    content = f.read()

# Find cmaps section
cmap_start = content.find('static const lv_font_fmt_txt_cmap_t cmaps[]')
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

# Parse range_start and range_length pairs
ranges = re.findall(r'\.range_start\s*=\s*(\d+).*?\.range_length\s*=\s*(\d+)', cmap_body, re.DOTALL)

print('=== CMAP 字符覆盖范围 ===')
total = 0
all_chars = []
for rs, rl in ranges:
    rs_int = int(rs)
    rl_int = int(rl)
    end = rs_int + rl_int - 1
    total += rl_int
    all_chars.append((rs_int, end, rl_int))
    
    desc = ''
    if 0x20 <= rs_int <= 0x7E:
        desc = '[ASCII]'
    elif 0x4E00 <= rs_int <= 0x9FFF:
        desc = '[CJK汉字]'
    elif 0x3000 <= rs_int <= 0x303F:
        desc = '[CJK标点]'
    elif 0xFF00 <= rs_int <= 0xFFEF:
        desc = '[全角/半角]'
    elif 0x2000 <= rs_int <= 0x206F:
        desc = '[通用标点]'
    elif 0x00A0 <= rs_int <= 0x00FF:
        desc = '[拉丁补充]'
    
    print(f'  U+{rs_int:04X} ~ U+{end:04X}  ({rl_int:5d} chars) {desc}')

print(f'\n总字符数: {total}')

# Check common fullwidth characters
print('\n=== 常用全角字符缺失分析 ===')
checks = [
    (0x3000, 0x3002, 'CJK ideographic space/comma/period'),
    (0xFF01, 0xFF5E, 'Fullwidth ASCII (! to ~)'),
    (0xFF0C, 0xFF0C, 'Fullwidth comma'),
    (0x300A, 0x300B, 'Book title marks'),
    (0x2018, 0x2019, 'Single quotation marks'),
    (0x201C, 0x201D, 'Double quotation marks'),
    (0xFF08, 0xFF09, 'Fullwidth parentheses'),
    (0x3001, 0x3001, 'CJK comma'),
    (0xFF1A, 0xFF1A, 'Fullwidth colon'),
    (0xFF1B, 0xFF1B, 'Fullwidth semicolon'),
    (0xFF0E, 0xFF0E, 'Fullwidth full stop'),
    (0xFF01, 0xFF01, 'Fullwidth exclamation'),
    (0xFF1F, 0xFF1F, 'Fullwidth question'),
    (0x2014, 0x2015, 'Em dash'),
    (0x2026, 0x2026, 'Horizontal ellipsis'),
    (0x3003, 0x3003, 'Ditto mark'),
    (0x2103, 0x2103, 'Degree Celsius'),
    (0x00B0, 0x00B0, 'Degree sign'),
    (0x00D7, 0x00D7, 'Multiplication sign'),
]

missing = []
for start, end, desc in checks:
    found = False
    for rs, re_end, _ in all_chars:
        if rs <= end and re_end >= start:
            found = True
            break
    if not found:
        missing.append((start, end, desc))
        print(f'  MISSING: U+{start:04X}~U+{end:04X} {desc}')

if not missing:
    print('  全部覆盖！')

print(f'\n缺失 {len(missing)} 个范围')
