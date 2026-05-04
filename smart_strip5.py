import re

with open(".tmp_pokeemerald_shop/src/new_shop.c", "r") as f:
    text = f.read()

# Exact Includes
text = text.replace('#include "tv.h"\n', "")
text = text.replace('#include "decoration.h"\n', "")
text = text.replace('#include "decoration_inventory.h"\n', "")
text = text.replace('#include "constants/decorations.h"\n', "")

# Strip all MUDSKIP_OUTFIT_SYSTEM blocks
text = re.sub(r'#ifdef MUDSKIP_OUTFIT_SYSTEM\n#include "outfit_menu\.h"\n#endif\n', '', text)
text = re.sub(r'#ifdef MUDSKIP_OUTFIT_SYSTEM.*?#endif // MUDSKIP_OUTFIT_SYSTEM\n', '', text, flags=re.DOTALL)

# Strip the exact NEW_SHOP_TYPE_DECOR blocks with an exact non-greedy regex
# We must ensure we only match to the end of the break; or proper end of block.

text = re.sub(r'        case NEW_SHOP_TYPE_DECOR \.\.\. NEW_SHOP_TYPE_DECOR2:\n            return gDecorations\[sMartInfo.itemList\[id\]\]\.name;\n', '', text)
text = re.sub(r'        case NEW_SHOP_TYPE_DECOR \.\.\. NEW_SHOP_TYPE_DECOR2:\n            return gDecorations\[sMartInfo.itemList\[id\]\]\.description;\n', '', text)
text = re.sub(r'        case NEW_SHOP_TYPE_DECOR \.\.\. NEW_SHOP_TYPE_DECOR2:\n            return gDecorations\[sMartInfo.itemList\[id\]\]\.price;\n', '', text)

text = re.sub(r'        case NEW_SHOP_TYPE_DECOR \.\.\. NEW_SHOP_TYPE_DECOR2:\n        \{\n            // DECOR_NONE has the same value as ITEM_NONE but this is for clarity\n            if \(sMartInfo.itemList\[i\] == DECOR_NONE\)\n            \{\n                sShopData->gridItems->iconSpriteIds\[idx\] = AddItemIconSprite\(GFXTAG_ITEM \+ idx, PALTAG_ITEM \+ idx, ITEM_LIST_END\);\n                gSprites\[sShopData->gridItems->iconSpriteIds\[idx\]\]\.x = x;\n                gSprites\[sShopData->gridItems->iconSpriteIds\[idx\]\]\.y = y;\n            \}\n            else\n            \{\n                sShopData->gridItems->iconSpriteIds\[idx\] = AddDecorationIconObject\(sMartInfo.itemList\[i\], x, y, 2, GFXTAG_ITEM \+ idx, PALTAG_ITEM \+ idx\);\n            \}\n            break;\n        \}\n', '', text)

text = re.sub(r'        case NEW_SHOP_TYPE_DECOR \.\.\. NEW_SHOP_TYPE_DECOR2:\n            return TRUE;\n', '', text)

text = re.sub(r'            case NEW_SHOP_TYPE_DECOR \.\.\. NEW_SHOP_TYPE_DECOR2:\n            \{\n                PrintMoneyLocal\(WIN_MULTI, RIGHT_ALIGNED_X, ITEM_PRICE_Y, price, COLORID_BLACK, STR_CONV_MODE_LEFT_ALIGN, FALSE\);\n                break;\n            \}\n', '', text)

text = re.sub(r'            case NEW_SHOP_TYPE_DECOR \.\.\. NEW_SHOP_TYPE_DECOR2:\n                StringCopy\(gStringVar1, gDecorations\[item\]\.name\);\n                break;\n', '', text)

text = re.sub(r'        case NEW_SHOP_TYPE_DECOR \.\.\. NEW_SHOP_TYPE_DECOR2:\n            if \(RoomForDecoration\(sMartInfo\.itemList\[id\]\)\)\n            \{\n                if \(sMartInfo\.martType == NEW_SHOP_TYPE_DECOR2\)\n                    StringCopy\(gStringVar2, sText_Var1AndYouWantedVar2BP\);\n                else\n                    StringCopy\(gStringVar2, sText_Var1AndYouWantedVar2Coins\);\n            \}\n            else\n            \{\n                BuyMenuDisplayMessage\(taskId, Shop_GetSellerMessage\(SELLER_MSG_BUY_FAIL_NO_SPACE\), Task_ReturnToItemListWaitMsg\);\n                return TRUE;\n            \}\n            break;\n', '', text)

text = re.sub(r'        case NEW_SHOP_TYPE_DECOR \.\.\. NEW_SHOP_TYPE_DECOR2:\n            if \(RoomForDecoration\(sMartInfo\.itemList\[id\]\)\)\n            \{\n                AddDecoration\(sMartInfo\.itemList\[id\]\);\n                BuyMenuSubtractMoney\(\);\n                BuyMenuDisplayMessage\(taskId, Shop_GetSellerMessage\(SELLER_MSG_BUY_SUCCESS\), Task_ReturnToItemListWaitMsg\);\n                RecordItemPurchase\(taskId\);\n            \}\n            else\n            \{\n                BuyMenuDisplayMessage\(taskId, Shop_GetSellerMessage\(SELLER_MSG_BUY_FAIL_NO_SPACE\), Task_ReturnToItemListWaitMsg\);\n            \}\n            break;\n', '', text)

text = re.sub(r'        case NEW_SHOP_TYPE_DECOR \.\.\. NEW_SHOP_TYPE_DECOR2:\n            BuyMenuPrint\(WIN_QUANTITY_PRICE, sText_CoinsVar1, x, 2, TEXT_SKIP_DRAW, COLORID_BLACK, FALSE\);\n            break;\n', '', text)

text = text.replace('CreateShopMenu(NEW_SHOP_TYPE_DECOR);', '')
text = text.replace('CreateShopMenu(NEW_SHOP_TYPE_DECOR2);', '')
text = text.replace('sMartInfo.itemCount++; // for ITEM_NONE / DECOR_NONE', 'sMartInfo.itemCount++; // for ITEM_NONE')
text = text.replace('// DECOR_NONE has the same value as ITEM_NONE but this is for clarity\n', '')
text = text.replace('if (sMartInfo.itemList[i] == DECOR_NONE)', 'if (0)')
text = text.replace('*itemList = ITEM_NONE;\n    *itemPriceList = ITEM_NONE;', '*itemList = ITEM_NONE;\n    *itemPriceList = 0;')

text = re.sub(r'void NewShop_CreateDecorationShop1Menu\(const u16 \*itemsForSale\)\n\{\n    CreateShopMenu\(NEW_SHOP_TYPE_DECOR\);\n    SetShopItemsForSale\(itemsForSale\);\n    SetShopMenuCallback\(ScriptContext_Enable\);\n\}\n', '', text)
text = re.sub(r'void NewShop_CreateDecorationShop2Menu\(const u16 \*itemsForSale\)\n\{\n    CreateShopMenu\(NEW_SHOP_TYPE_DECOR2\);\n    SetShopItemsForSale\(itemsForSale\);\n    SetShopMenuCallback\(ScriptContext_Enable\);\n\}\n', '', text)

text = re.sub(r'void ClearItemPurchases\(void\)\s*\{.*?\}', 'void ClearItemPurchases(void){}', text, flags=re.DOTALL)
text = re.sub(r'static void RecordItemPurchase\(u8 taskId\)\s*\{.*?\}', 'static void RecordItemPurchase(u8 taskId){}', text, flags=re.DOTALL)

text = text.replace("gText_ShopBuy", "sText_ShopBuy")
text = text.replace("gText_ShopSell", "sText_ShopSell")
text = text.replace("gText_ShopQuit", "sText_ShopQuit")
text = text.replace("gText_QuitShopping", "sText_QuitShopping")
text = text.replace('gText_AnythingElseICanHelp', 'sText_AnythingElseICanHelp')

static_strings = '''static const u8 sText_ShopBuy[] = _("BUY");
static const u8 sText_ShopSell[] = _("SELL");
static const u8 sText_ShopQuit[] = _("QUIT");
static const u8 sText_QuitShopping[] = _("Quit shopping.");
static const u8 sText_AnythingElseICanHelp[] = _("Is there anything else I can help\\nyou with?");
'''
text = text.replace('static EWRAM_DATA u8 sPurchaseHistoryId = 0;', 'static EWRAM_DATA u8 sPurchaseHistoryId = 0;\n' + static_strings)

text = text.replace('!IsEnoughCoins(sShopData->totalCost)', '(GetCoins() < sShopData->totalCost)')
text = text.replace('!IsEnoughBattlePoints(sShopData->totalCost)', '(0)')
text = text.replace('GetBattlePoints()', '0')
text = text.replace('RemoveBattlePoints(sShopData->totalCost);', '/* BP unsupported */')

# Safe replace for GetItemPrice that doesn't hit BuyMenuGetItemPrice
text = text.replace('GetItemCoinPrice(', 'GetItemPrice(')
text = text.replace('GetItemBpPrice(', 'GetItemPrice(')

text = text.replace('PrintMenuTable(sMartInfo.windowId, numMenuItems, sMartInfo.menuActions);', 'PrintMenuActionTexts(sMartInfo.windowId, FONT_NORMAL, 8, 2, 0, 16, numMenuItems, sMartInfo.menuActions, NULL);')
text = text.replace('IsTextPrinterActive(WIN_ITEM_DESCRIPTION)', 'IsTextPrinterActiveOnWindow(WIN_ITEM_DESCRIPTION)')
text = text.replace('TryPutSmartShopperOnAir();\n', '')
text = text.replace('DisplayItemMessageOnField(taskId, Shop_GetSellerMessage(SELLER_MSG_RETURN_TO_FIELD), ShowShopMenuAfterExitingBuyOrSellMenu);', 'DisplayItemMessageOnField(taskId, 1, Shop_GetSellerMessage(SELLER_MSG_RETURN_TO_FIELD), ShowShopMenuAfterExitingBuyOrSellMenu);')
text = text.replace('SE_RG_BAG_CURSOR', 'SE_SELECT')
text = text.replace('LZDecompressWram(', 'LZ77UnCompWram(')
text = text.replace('u32 numDigits = CountDigits(amount);', 'u32 numDigits = 1; u32 temp = amount; while(temp /= 10) numDigits++;')
text = text.replace('sShopData->totalCost = (cost >> IsPokeNewsActive(POKENEWS_SLATEPORT));', 'sShopData->totalCost = cost;')
text = text.replace('sShopData->totalCost = (price >> IsPokeNewsActive(POKENEWS_SLATEPORT)) * tItemCount;', 'sShopData->totalCost = price * tItemCount;')

if '#include "field_fadetransition.h"' not in text:
    text = text.replace('#include "text_window.h"', '#include "text_window.h"\n#include "international_string_util.h"\n#include "money.h"\n#include "party_menu.h"\n#include "menu.h"\n#include "field_fadetransition.h"\n#include "item_menu.h"\n')

text = text.replace('''        case NEW_SHOP_TYPE_DECOR ... NEW_SHOP_TYPE_DECOR2:
        case NEW_SHOP_TYPE_NORMAL:''', '        case NEW_SHOP_TYPE_NORMAL:')


with open("src/new_shop.c", "w") as f:
    f.write(text)

