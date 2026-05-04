import re

with open(".tmp_pokeemerald_shop/src/new_shop.c", "r") as f:
    text = f.read()

# Includes
text = text.replace('#include "tv.h"\n', "")
text = text.replace('#include "decoration.h"\n', "")
text = text.replace('#include "decoration_inventory.h"\n', "")
text = text.replace('#include "constants/decorations.h"\n', "")
text = text.replace('#include "outfit_menu.h"\n', "")

# outfit blocks
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

# strings
text = text.replace("gText_ShopBuy", "sText_ShopBuy")
text = text.replace("gText_ShopSell", "sText_ShopSell")
text = text.replace("gText_ShopQuit", "sText_ShopQuit")
text = text.replace("gText_QuitShopping", "sText_QuitShopping")
text = text.replace('static EWRAM_DATA u8 sPurchaseHistoryId = 0;', 'static EWRAM_DATA u8 sPurchaseHistoryId = 0;\nstatic const u8 sText_ShopBuy[] = _("BUY");\nstatic const u8 sText_ShopSell[] = _("SELL");\nstatic const u8 sText_ShopQuit[] = _("QUIT");\nstatic const u8 sText_QuitShopping[] = _("Quit shopping.");\n')

# decor
# let's be extremely precise with replacements
text = text.replace('''        case NEW_SHOP_TYPE_DECOR ... NEW_SHOP_TYPE_DECOR2:
            return gDecorations[sMartInfo.itemList[id]].name;''', '')
text = text.replace('''        case NEW_SHOP_TYPE_DECOR ... NEW_SHOP_TYPE_DECOR2:
            return gDecorations[sMartInfo.itemList[id]].description;''', '')
text = text.replace('''        case NEW_SHOP_TYPE_DECOR ... NEW_SHOP_TYPE_DECOR2:
            return gDecorations[sMartInfo.itemList[id]].price;''', '')
text = text.replace('''        case NEW_SHOP_TYPE_DECOR ... NEW_SHOP_TYPE_DECOR2:
            sShopData->gridItems->iconSpriteIds[idx] = AddDecorationIconObject(sMartInfo.itemList[i], x, y, 2, GFXTAG_ITEM + idx, PALTAG_ITEM + idx);
            break;''', '')

# BuyMenuTryMakePurchase decor case
text = text.replace('''        case NEW_SHOP_TYPE_DECOR ... NEW_SHOP_TYPE_DECOR2:
            if (RoomForDecoration(sMartInfo.itemList[id]))
            {
                AddDecoration(sMartInfo.itemList[id]);
                BuyMenuSubtractMoney();
                BuyMenuDisplayMessage(taskId, Shop_GetSellerMessage(SELLER_MSG_BUY_SUCCESS), Task_ReturnToItemListWaitMsg);
                RecordItemPurchase(taskId);
            }
            else
            {
                BuyMenuDisplayMessage(taskId, Shop_GetSellerMessage(SELLER_MSG_BUY_FAIL_NO_SPACE), Task_ReturnToItemListWaitMsg);
            }
            break;''', '')


# other single replacements
text = text.replace('CreateShopMenu(NEW_SHOP_TYPE_DECOR);', '')
text = text.replace('CreateShopMenu(NEW_SHOP_TYPE_DECOR2);', '')
text = text.replace('sMartInfo.itemCount++; // for ITEM_NONE / DECOR_NONE', 'sMartInfo.itemCount++; // for ITEM_NONE')
text = text.replace('// DECOR_NONE has the same value as ITEM_NONE but this is for clarity\n', '')
text = text.replace('*itemList = ITEM_NONE;\n    *itemPriceList = ITEM_NONE;', '*itemList = ITEM_NONE;\n    *itemPriceList = 0;')
text = text.replace('if (sMartInfo.itemList[i] == DECOR_NONE)', 'if (0)')

# functions to delete completely: NewShop_CreateDecorationShop1Menu and NewShop_CreateDecorationShop2Menu
# We can use Regex for this, it's safe if we match the closing brace at the beginning of the line
text = re.sub(r'void NewShop_CreateDecorationShop1Menu.*?\n}\n', '', text, flags=re.DOTALL)
text = re.sub(r'void NewShop_CreateDecorationShop2Menu.*?\n}\n', '', text, flags=re.DOTALL)


# FireRed missing functions mapping:
text = text.replace('!IsEnoughCoins(sShopData->totalCost)', '(GetCoins() < sShopData->totalCost)')
text = text.replace('!IsEnoughBattlePoints(sShopData->totalCost)', '(0)')
text = text.replace('GetBattlePoints()', '0')
text = text.replace('RemoveBattlePoints(sShopData->totalCost);', '/* BP unsupported */')
text = text.replace('GetItemPrice(', 'ItemId_GetPrice(')

# FireRed specifics missing definitions from log:
# 1. PrintMenuTable => in FireRed, PrintMenuTable doesn't exist, it's just PutWindowTilemap and CopyWindowToVram?
# Wait! In FireRed, PrintTextOnWindow etc. PokeEmerald's `PrintMenuTable` draws the actions. PokeFireRed `shop.c` uses `PrintMenuActionGrid` or `PrintMenuTable`?
# In FR it's `PrintMenuActionTexts`. Let's just define a dummy or use `DrawStdWindowFrame`. Wait, PokeFireRed has `PrintMenuActionTexts` possibly? Let's check FireRed menu.h
# 2. TryPutSmartShopperOnAir => doesn't exist in FR. Just remove it.
text = text.replace('TryPutSmartShopperOnAir();', '')

# 3. FadeInFromBlack => in FR it's `FadeInFromBlack()`? Wait, FR has `FadeScreen(1, 0)`?
# Let's search fade in FR.
# 4. DisplayItemMessageOnField(taskId, fontId, src, callback);
# In the file it was called as DisplayItemMessageOnField(taskId, Shop_GetSellerMessage(...), Callback);
# We must inject FONT_NORMAL as second param!
text = text.replace('DisplayItemMessageOnField(taskId, Shop_GetSellerMessage(SELLER_MSG_RETURN_TO_FIELD), ShowShopMenuAfterExitingBuyOrSellMenu);', 'DisplayItemMessageOnField(taskId, 1, Shop_GetSellerMessage(SELLER_MSG_RETURN_TO_FIELD), ShowShopMenuAfterExitingBuyOrSellMenu);')

# 5. SE_RG_BAG_CURSOR => just SE_SELECT or SE_SHOP?
text = text.replace('SE_RG_BAG_CURSOR', 'SE_SELECT')

# 6. LZDecompressWram => in FR it's LZDecompressWram? Wait, it's probably LZ77UnCompWram.
text = text.replace('LZDecompressWram(', 'LZ77UnCompWram(')

# 7. CountDigits => missing in FR? We can use ConvertIntToDecimalStringN then get length. Or just implement CountDigits manually as local function.
text = text.replace('u32 numDigits = CountDigits(amount);', 'u32 numDigits = 1; u32 temp = amount; while(temp /= 10) numDigits++;')

# 8. IsTextPrinterActive => in FR it's IsTextPrinterActive(id)? FR probably has TextPrinterWait or IsTextPrinterActive is just IsTextPrinterActive? Wait, no. Run grep.
# 9. gMartPurchaseHistory => FireRed lacks this. Let's comment out ClearItemPurchases and RecordItemPurchase content.
text = re.sub(r'void ClearItemPurchases\(void\)\s*\{.*?\}', 'void ClearItemPurchases(void){}', text, flags=re.DOTALL)
text = re.sub(r'static void RecordItemPurchase\(u8 taskId\)\s*\{.*?\}', 'static void RecordItemPurchase(u8 taskId){}', text, flags=re.DOTALL)

# 10. gText_AnythingElseICanHelp => missing. We can use sText_AnythingElseICanHelp
text = text.replace('gText_AnythingElseICanHelp', 'sText_AnythingElseICanHelp')

# Let's define the missing strings:
sText_AnythingElseICanHelp_def = '''static const u8 sText_AnythingElseICanHelp[] = _("Is there anything else I can help\\nyou with?");\n'''
text = text.replace('static const u8 sText_ShopBuy[]', sText_AnythingElseICanHelp_def + 'static const u8 sText_ShopBuy[]')

# 11. PrintMenuTable => FR might have `PrintMenuTable` in `menu.c`? What does FR Shop use? Let's check `shop.c` for PrintMenu*.

# POKENEWS
text = text.replace('sShopData->totalCost = (cost >> IsPokeNewsActive(POKENEWS_SLATEPORT));', 'sShopData->totalCost = cost;')
text = text.replace('sShopData->totalCost = (price >> IsPokeNewsActive(POKENEWS_SLATEPORT)) * tItemCount;', 'sShopData->totalCost = price * tItemCount;')

with open("src/new_shop.c", "w") as f:
    f.write(text)

