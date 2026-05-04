import re

with open(".tmp_pokeemerald_shop/src/new_shop.c", "r") as f:
    lines = f.readlines()

out_lines = []
skip_outfit = False
for line in lines:
    if '#include "tv.h"' in line or '#include "decoration.h"' in line or '#include "decoration_inventory.h"' in line or '#include "constants/decorations.h"' in line or '#include "outfit_menu.h"' in line:
        continue
        
    if '#ifdef MUDSKIP_OUTFIT_SYSTEM' in line:
        skip_outfit = True
        continue
    if skip_outfit and line.strip().startswith('#endif'):
        skip_outfit = False
        continue
    if skip_outfit:
        continue
        
    out_lines.append(line)

text = "".join(out_lines)

# EXACT replacements for Decor matching .tmp_pokeemerald_shop code exactly:
text = text.replace('''        case NEW_SHOP_TYPE_DECOR ... NEW_SHOP_TYPE_DECOR2:
            return gDecorations[sMartInfo.itemList[id]].name;''', '')
text = text.replace('''        case NEW_SHOP_TYPE_DECOR ... NEW_SHOP_TYPE_DECOR2:
            return gDecorations[sMartInfo.itemList[id]].description;''', '')
text = text.replace('''        case NEW_SHOP_TYPE_DECOR ... NEW_SHOP_TYPE_DECOR2:
            return gDecorations[sMartInfo.itemList[id]].price;''', '')
text = text.replace('''        case NEW_SHOP_TYPE_DECOR ... NEW_SHOP_TYPE_DECOR2:
            sShopData->gridItems->iconSpriteIds[idx] = AddDecorationIconObject(sMartInfo.itemList[i], x, y, 2, GFXTAG_ITEM + idx, PALTAG_ITEM + idx);
            break;''', '')
text = text.replace('''        case NEW_SHOP_TYPE_DECOR ... NEW_SHOP_TYPE_DECOR2:
            return TRUE;''', '')
text = text.replace('''            case NEW_SHOP_TYPE_DECOR ... NEW_SHOP_TYPE_DECOR2:
            {
                PrintMoneyLocal(WIN_MULTI, RIGHT_ALIGNED_X, ITEM_PRICE_Y, price, COLORID_BLACK, STR_CONV_MODE_LEFT_ALIGN, FALSE);
                break;
            }''', '')
text = text.replace('''            case NEW_SHOP_TYPE_DECOR ... NEW_SHOP_TYPE_DECOR2:
                StringCopy(gStringVar1, gDecorations[item].name);
                break;''', '')
text = text.replace('''        case NEW_SHOP_TYPE_DECOR ... NEW_SHOP_TYPE_DECOR2:
            if (RoomForDecoration(sMartInfo.itemList[id]))
            {
                if (sMartInfo.martType == NEW_SHOP_TYPE_DECOR2)
                    StringCopy(gStringVar2, sText_Var1AndYouWantedVar2BP);
                else
                    StringCopy(gStringVar2, sText_Var1AndYouWantedVar2Coins);
            }
            else
            {
                BuyMenuDisplayMessage(taskId, Shop_GetSellerMessage(SELLER_MSG_BUY_FAIL_NO_SPACE), Task_ReturnToItemListWaitMsg);
                return TRUE;
            }
            break;''', '')
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
text = text.replace('''        case NEW_SHOP_TYPE_DECOR ... NEW_SHOP_TYPE_DECOR2:
            BuyMenuPrint(WIN_QUANTITY_PRICE, sText_CoinsVar1, x, 2, TEXT_SKIP_DRAW, COLORID_BLACK, FALSE);
            break;''', '')
text = text.replace('''                if (sMartInfo.martType == NEW_SHOP_TYPE_DECOR2)
                    StringCopy(gStringVar2, sText_Var1AndYouWantedVar2BP);
                else''', '')
text = text.replace('CreateShopMenu(NEW_SHOP_TYPE_DECOR);', '')
text = text.replace('CreateShopMenu(NEW_SHOP_TYPE_DECOR2);', '')
text = text.replace('sMartInfo.itemCount++; // for ITEM_NONE / DECOR_NONE', 'sMartInfo.itemCount++; // for ITEM_NONE')
text = text.replace('// DECOR_NONE has the same value as ITEM_NONE but this is for clarity\n', '')
text = text.replace('if (sMartInfo.itemList[i] == DECOR_NONE)', 'if (0)')
text = text.replace('*itemList = ITEM_NONE;\n    *itemPriceList = ITEM_NONE;', '*itemList = ITEM_NONE;\n    *itemPriceList = 0;')

# Decor Functions to delete
text = re.sub(r'void NewShop_CreateDecorationShop1Menu.*?}\n', '', text, flags=re.DOTALL)
text = re.sub(r'void NewShop_CreateDecorationShop2Menu.*?}\n', '', text, flags=re.DOTALL)

# Purchase History Functions empty 
text = re.sub(r'void ClearItemPurchases\(void\)\s*\{.*?\}', 'void ClearItemPurchases(void){}', text, flags=re.DOTALL)
text = re.sub(r'static void RecordItemPurchase\(u8 taskId\)\s*\{.*?\}', 'static void RecordItemPurchase(u8 taskId){}', text, flags=re.DOTALL)

# String Fixes
text = text.replace("gText_ShopBuy", "sText_ShopBuy")
text = text.replace("gText_ShopSell", "sText_ShopSell")
text = text.replace("gText_ShopQuit", "sText_ShopQuit")
text = text.replace("gText_QuitShopping", "sText_QuitShopping")
text = text.replace('gText_AnythingElseICanHelp', 'sText_AnythingElseICanHelp')

# Setup Static Strings
static_strings = '''static const u8 sText_ShopBuy[] = _("BUY");
static const u8 sText_ShopSell[] = _("SELL");
static const u8 sText_ShopQuit[] = _("QUIT");
static const u8 sText_QuitShopping[] = _("Quit shopping.");
static const u8 sText_AnythingElseICanHelp[] = _("Is there anything else I can help\\nyou with?");
'''
text = text.replace('static EWRAM_DATA u8 sPurchaseHistoryId = 0;', 'static EWRAM_DATA u8 sPurchaseHistoryId = 0;\n' + static_strings)

# Function Mismatches
text = text.replace('!IsEnoughCoins(sShopData->totalCost)', '(GetCoins() < sShopData->totalCost)')
text = text.replace('!IsEnoughBattlePoints(sShopData->totalCost)', '(0)')
text = text.replace('GetBattlePoints()', '0')
text = text.replace('RemoveBattlePoints(sShopData->totalCost);', '/* BP unsupported */')

# Safe Replace GetItemPrice
text = re.sub(r'(?<!BuyMenu)GetItemPrice\(', 'ItemId_GetPrice(', text)
text = text.replace('GetItemCoinPrice(', 'ItemId_GetPrice(')
text = text.replace('GetItemBpPrice(', 'ItemId_GetPrice(')

# PokeNews
text = text.replace('sShopData->totalCost = (cost >> IsPokeNewsActive(POKENEWS_SLATEPORT));', 'sShopData->totalCost = cost;')
text = text.replace('sShopData->totalCost = (price >> IsPokeNewsActive(POKENEWS_SLATEPORT)) * tItemCount;', 'sShopData->totalCost = price * tItemCount;')

# PokeEmerald Specific UI Missing Methods
text = text.replace('PrintMenuTable(sMartInfo.windowId, numMenuItems, sMartInfo.menuActions);', 'PrintMenuActionTexts(sMartInfo.windowId, FONT_NORMAL, 8, 2, 0, 16, numMenuItems, sMartInfo.menuActions, NULL);')
text = text.replace('IsTextPrinterActive(WIN_ITEM_DESCRIPTION)', 'IsTextPrinterActiveOnWindow(WIN_ITEM_DESCRIPTION)')
text = text.replace('TryPutSmartShopperOnAir();\n', '')
text = text.replace('DisplayItemMessageOnField(taskId, Shop_GetSellerMessage(SELLER_MSG_RETURN_TO_FIELD), ShowShopMenuAfterExitingBuyOrSellMenu);', 'DisplayItemMessageOnField(taskId, 1, Shop_GetSellerMessage(SELLER_MSG_RETURN_TO_FIELD), ShowShopMenuAfterExitingBuyOrSellMenu);')
text = text.replace('SE_RG_BAG_CURSOR', 'SE_SELECT')
text = text.replace('LZDecompressWram(', 'LZ77UnCompWram(')
text = text.replace('u32 numDigits = CountDigits(amount);', 'u32 numDigits = 1; u32 temp = amount; while(temp /= 10) numDigits++;')

# Missing Headers
if '#include "field_fadetransition.h"' not in text:
    text = text.replace('#include "text_window.h"', '#include "text_window.h"\n#include "international_string_util.h"\n#include "money.h"\n#include "party_menu.h"\n#include "menu.h"\n#include "field_fadetransition.h"')

with open("src/new_shop.c", "w") as f:
    f.write(text)

