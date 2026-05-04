with open(".tmp_pokeemerald_shop/src/new_shop.c", "r") as f:
    lines = f.readlines()

new_lines = []
skip_outfit = False
skip_decor = False
decor_brace_level = 0
skip_decor_func = False
decor_func_brace_level = 0

for line in lines:
    if '#include "tv.h"' in line or '#include "decoration.h"' in line or '#include "decoration_inventory.h"' in line or '#include "constants/decorations.h"' in line or '#include "outfit_menu.h"' in line:
        continue
        
    # ONLY trigger outfit skip on exact MUDSKIP_OUTFIT_SYSTEM
    if '#ifdef MUDSKIP_OUTFIT_SYSTEM' in line:
        skip_outfit = True
        continue
        
    # When skipping outfit, wait for #endif
    if skip_outfit:
        if line.strip().startswith('#endif'):
            skip_outfit = False
        continue
        
    # Handle Decor Functions
    if 'void NewShop_CreateDecorationShop' in line:
        skip_decor_func = True
        decor_func_brace_level = 0
        
    if skip_decor_func:
        decor_func_brace_level += line.count('{')
        decor_func_brace_level -= line.count('}')
        if decor_func_brace_level <= 0 and line.strip() == "}":
            skip_decor_func = False
        continue

    # Handle Decor Case blocks
    if 'case NEW_SHOP_TYPE_DECOR' in line:
        skip_decor = True
        decor_brace_level = 0
        
    if skip_decor:
        decor_brace_level += line.count('{')
        decor_brace_level -= line.count('}')
        if decor_brace_level <= 0 and 'break;' in line:
            skip_decor = False
            continue
        if decor_brace_level == 0 and 'return ' in line:
            skip_decor = False
            continue
        continue

    # Replacements & Removals on the surviving lines
    line = line.replace('CreateShopMenu(NEW_SHOP_TYPE_DECOR);', '')
    line = line.replace('CreateShopMenu(NEW_SHOP_TYPE_DECOR2);', '')
    line = line.replace('sMartInfo.itemCount++; // for ITEM_NONE / DECOR_NONE', 'sMartInfo.itemCount++; // for ITEM_NONE')
    line = line.replace('// DECOR_NONE has the same value as ITEM_NONE but this is for clarity\n', '')
    line = line.replace('if (sMartInfo.itemList[i] == DECOR_NONE)', 'if (0)')
    
    # Text
    line = line.replace('gText_ShopBuy', 'sText_ShopBuy')
    line = line.replace('gText_ShopSell', 'sText_ShopSell')
    line = line.replace('gText_ShopQuit', 'sText_ShopQuit')
    line = line.replace('gText_QuitShopping', 'sText_QuitShopping')
    line = line.replace('gText_AnythingElseICanHelp', 'sText_AnythingElseICanHelp')
    if 'static EWRAM_DATA u8 sPurchaseHistoryId = 0;' in line:
        line = 'static EWRAM_DATA u8 sPurchaseHistoryId = 0;\nstatic const u8 sText_ShopBuy[] = _("BUY");\nstatic const u8 sText_ShopSell[] = _("SELL");\nstatic const u8 sText_ShopQuit[] = _("QUIT");\nstatic const u8 sText_QuitShopping[] = _("Quit shopping.");\nstatic const u8 sText_AnythingElseICanHelp[] = _("Is there anything else I can help\\nyou with?");\n'
    
    # BP and Coins logic mappings (FireRed specific)
    line = line.replace('!IsEnoughCoins(sShopData->totalCost)', '(GetCoins() < sShopData->totalCost)')
    line = line.replace('!IsEnoughBattlePoints(sShopData->totalCost)', '(0)')
    line = line.replace('GetBattlePoints()', '0')
    line = line.replace('RemoveBattlePoints(sShopData->totalCost);', '/* BP unsupported */')
    line = line.replace('GetItemCoinPrice(', 'GetItemPrice(')
    line = line.replace('GetItemBpPrice(', 'GetItemPrice(')
    
    # PrintMenuTable
    line = line.replace('PrintMenuTable(sMartInfo.windowId, numMenuItems, sMartInfo.menuActions);', 'PrintMenuActionTexts(sMartInfo.windowId, FONT_NORMAL, 8, 2, 0, 16, numMenuItems, sMartInfo.menuActions, NULL);')
    
    line = line.replace('IsTextPrinterActive(WIN_ITEM_DESCRIPTION)', 'IsTextPrinterActiveOnWindow(WIN_ITEM_DESCRIPTION)')
    line = line.replace('TryPutSmartShopperOnAir();', '')
    line = line.replace('DisplayItemMessageOnField(taskId, Shop_GetSellerMessage(SELLER_MSG_RETURN_TO_FIELD), ShowShopMenuAfterExitingBuyOrSellMenu);', 'DisplayItemMessageOnField(taskId, 1, Shop_GetSellerMessage(SELLER_MSG_RETURN_TO_FIELD), ShowShopMenuAfterExitingBuyOrSellMenu);')
    
    line = line.replace('SE_RG_BAG_CURSOR', 'SE_SELECT')
    line = line.replace('LZDecompressWram(', 'LZ77UnCompWram(')
    line = line.replace('u32 numDigits = CountDigits(amount);', 'u32 numDigits = 1; u32 temp = amount; while(temp /= 10) numDigits++;')
    
    # PokeNews
    line = line.replace('sShopData->totalCost = (cost >> IsPokeNewsActive(POKENEWS_SLATEPORT));', 'sShopData->totalCost = cost;')
    line = line.replace('sShopData->totalCost = (price >> IsPokeNewsActive(POKENEWS_SLATEPORT)) * tItemCount;', 'sShopData->totalCost = price * tItemCount;')
    
    # Purchase History
    if 'void ClearItemPurchases(void)' in line:
        skip_decor_func = True
        decor_func_brace_level = 0
        line = 'void ClearItemPurchases(void){}\n'
        new_lines.append(line)
        continue
    if 'static void RecordItemPurchase(u8 taskId)' in line:
        skip_decor_func = True
        decor_func_brace_level = 0
        line = 'static void RecordItemPurchase(u8 taskId){}\n'
        new_lines.append(line)
        continue
        
    line = line.replace('*itemList = ITEM_NONE;\n    *itemPriceList = ITEM_NONE;', '*itemList = ITEM_NONE;\n    *itemPriceList = 0;')
    
    new_lines.append(line)

text = "".join(new_lines)

# Safe replace for GetItemPrice that doesn't hit BuyMenuGetItemPrice
import re
text = re.sub(r'(?<!BuyMenu)GetItemPrice\(', 'ItemId_GetPrice(', text)
text = text.replace('GetItemPrice(', 'ItemId_GetPrice(') # Wait, no, GetItemPrice IS correct for FireRed!

# Revert my ItemId_GetPrice back to GetItemPrice!
text = text.replace('ItemId_GetPrice(', 'GetItemPrice(')


if '#include "field_fadetransition.h"' not in text:
    text = text.replace('#include "text_window.h"', '#include "text_window.h"\n#include "international_string_util.h"\n#include "money.h"\n#include "party_menu.h"\n#include "menu.h"\n#include "field_fadetransition.h"')

with open("src/new_shop.c", "w") as f:
    f.write(text)

