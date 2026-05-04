import re
with open("src/scrcmd.c", "r") as f:
    text = f.read()

# removing ScrCmd_pokemartdecoration
text = re.sub(r'bool8 ScrCmd_pokemartdecoration\(.*?return TRUE;\n}\n', '', text, flags=re.DOTALL)
text = re.sub(r'bool8 ScrCmd_pokemartdecoration2\(.*?return TRUE;\n}\n', '', text, flags=re.DOTALL)

with open("src/scrcmd.c", "w") as f:
    f.write(text)

