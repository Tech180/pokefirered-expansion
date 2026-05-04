import re

with open("src/new_shop.c", "r") as f:
    text = f.read()

# Includes
text = text.replace('#include "tv.h"', "")
text = text.replace('#include "decoration.h"', "")
text = text.replace('#include "decoration_inventory.h"', "")
text = text.replace('#include "constants/decorations.h"', "")
text = text.replace('#include "outfit_menu.h"', "")

# All outfit blocks:
# We find exactly what they are. Let's just remove them line by line
lines = text.split('\n')
out_lines = []
skip = False
for line in lines:
    if line.strip().startswith('#ifdef MUDSKIP_OUTFIT_SYSTEM'):
        skip = True
        continue
    if skip and line.strip().startswith('#endif'):
        skip = False
        continue
    if skip:
        continue
    out_lines.append(line)

text = '\n'.join(out_lines)

# Now DECOR stuff
text = re.sub(r'[ \t]*case NEW_SHOP_TYPE_DECOR \.\.\. NEW_SHOP_TYPE_DECOR2:\n[ \t]*return gDecorations\[.*?\]\..*?;\n', '', text)

text = re.sub(r'[ \t]*case NEW_SHOP_TYPE_DECOR \.\.\. NEW_SHOP_TYPE_DECOR2:.*?break;', '', text, flags=re.DOTALL)

# Delete definitions of NewShop_CreateDecorationShop1Menu and 2
text = re.sub(r'void NewShop_CreateDecorationShop1Menu.*?}\n', '', text, flags=re.DOTALL)
text = re.sub(r'void NewShop_CreateDecorationShop2Menu.*?}\n', '', text, flags=re.DOTALL)

# Also single statements:
text = text.replace('CreateShopMenu(NEW_SHOP_TYPE_DECOR);', '')
text = text.replace('CreateShopMenu(NEW_SHOP_TYPE_DECOR2);', '')
text = text.replace('if (sMartInfo.itemList[i] == DECOR_NONE)', 'if(0)')
text = text.replace('sMartInfo.itemCount++; // for ITEM_NONE / DECOR_NONE', 'sMartInfo.itemCount++; // for ITEM_NONE')
text = text.replace('// DECOR_NONE has the same value as ITEM_NONE but this is for clarity', '')
text = text.replace('*itemList = ITEM_NONE;\n    *itemPriceList = ITEM_NONE;', '*itemList = ITEM_NONE;\n    *itemPriceList = 0;')

# strings
text = text.replace("gText_ShopBuy", "sText_ShopBuy")
text = text.replace("gText_ShopSell", "sText_ShopSell")
text = text.replace("gText_ShopQuit", "sText_ShopQuit")
text = text.replace("gText_QuitShopping", "sText_QuitShopping")
text = text.replace('static EWRAM_DATA u8 sPurchaseHistoryId = 0;', 'static EWRAM_DATA u8 sPurchaseHistoryId = 0;\nstatic const u8 sText_ShopBuy[] = _("BUY");\nstatic const u8 sText_ShopSell[] = _("SELL");\nstatic const u8 sText_ShopQuit[] = _("QUIT");\nstatic const u8 sText_QuitShopping[] = _("Quit shopping.");\n')

# POKENEWS is not in FireRed
text = text.replace('sShopData->totalCost = (cost >> IsPokeNewsActive(POKENEWS_SLATEPORT));', 'sShopData->totalCost = cost;')
text = text.replace('sShopData->totalCost = (price >> IsPokeNewsActive(POKENEWS_SLATEPORT)) * tItemCount;', 'sShopData->totalCost = price * tItemCount;')

# FireRed specific function differences
text = text.replace('IsEnoughCoins', 'CheckCoins')
text = text.replace('IsEnoughBattlePoints', 'CheckBattlePoints')
text = text.replace('GetBattlePoints()', 'GetBattlePoints()') # ensure it's correct? We'll leave it for now
text = text.replace('RemoveBattlePoints', 'RemoveBattlePoints')
text = text.replace('GetItemCoinPrice', 'GetItemPrice') # no such thing as coin price by default usually, unless user has it
text = text.replace('GetItemBpPrice', 'GetItemPrice')

with open("src/new_shop.c", "w") as f:
    f.write(text)

