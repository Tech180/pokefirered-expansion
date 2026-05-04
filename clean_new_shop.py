import re

with open("src/new_shop.c", "r") as f:
    lines = f.readlines()

new_lines = []
in_outfit = False
in_decor = False
decor_brace_level = 0
in_decor_case = False

includes_to_remove = ["tv.h", "decoration.h", "decoration_inventory.h", "outfit_menu.h", "constants/decorations.h"]

i = 0
while i < len(lines):
    line = lines[i]
    
    if any(inc in line for inc in includes_to_remove) and line.startswith("#include"):
        i += 1
        continue
        
    if "#ifdef MUDSKIP_OUTFIT_SYSTEM" in line:
        in_outfit = True
        i += 1
        continue
    
    if in_outfit and "#endif" in line:
        in_outfit = False
        i += 1
        continue
        
    if in_outfit:
        i += 1
        continue
        
    if "case NEW_SHOP_TYPE_DECOR ... NEW_SHOP_TYPE_DECOR2:" in line:
        in_decor_case = True
        decor_brace_level = 0
        i += 1
        continue
        
    if in_decor_case:
        if "{" in line:
            decor_brace_level += line.count("{")
        if "}" in line:
            decor_brace_level -= line.count("}")
            if decor_brace_level <= 0:
                in_decor_case = False
        if "break;" in line and decor_brace_level == 0:
            in_decor_case = False
        i += 1
        continue
        
    if "void NewShop_CreateDecorationShop1Menu(" in line or "void NewShop_CreateDecorationShop2Menu(" in line:
        in_decor_case = True
        decor_brace_level = 0
        i += 1
        continue

    # also remove single CreateShopMenu calls for decor
    if "CreateShopMenu(NEW_SHOP_TYPE_DECOR);" in line or "CreateShopMenu(NEW_SHOP_TYPE_DECOR2);" in line:
        i += 1
        continue
        
    # fix strings
    if "gText_ShopBuy" in line:
        line = line.replace("gText_ShopBuy", "sText_ShopBuy")
    if "gText_ShopSell" in line:
        line = line.replace("gText_ShopSell", "sText_ShopSell")
    if "gText_ShopQuit" in line:
        line = line.replace("gText_ShopQuit", "sText_ShopQuit")
    if "gText_QuitShopping" in line:
        line = line.replace("gText_QuitShopping", "sText_QuitShopping")
        
    new_lines.append(line)
    i += 1

text = "".join(new_lines)
# fix some strings that FireRed doesn't export in strings.h but are static in shop.c
text = text.replace('static EWRAM_DATA u8 sPurchaseHistoryId = 0;', 'static EWRAM_DATA u8 sPurchaseHistoryId = 0;\nstatic const u8 sText_ShopBuy[] = _("BUY");\nstatic const u8 sText_ShopSell[] = _("SELL");\nstatic const u8 sText_ShopQuit[] = _("QUIT");\nstatic const u8 sText_QuitShopping[] = _("Quit shopping.");\n')

with open("src/new_shop.c", "w") as f:
    f.write(text)

