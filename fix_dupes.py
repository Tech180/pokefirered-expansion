with open("src/new_shop.c", "r") as f:
    lines = f.readlines()

new_lines = []
last_line = ""
for line in lines:
    if line == last_line and "SetStandardWindowBorderStyle" in line:
        continue
    if line == last_line and "PrintMenuActionTexts" in line:
        continue
    if line == last_line and "InitMenuInUpperLeftCornerNormal" in line:
        continue
    if line == last_line and "PutWindowTilemap" in line:
        continue
    new_lines.append(line)
    last_line = line

with open("src/new_shop.c", "w") as f:
    f.writelines(new_lines)
