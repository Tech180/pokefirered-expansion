import re
with open("src/new_shop.c", "r") as f:
    text = f.read()

# 947:
text = text.replace('''        case NEW_SHOP_TYPE_DECOR ... NEW_SHOP_TYPE_DECOR2:
            return TRUE;''', '')

# 1374:
text = text.replace('''            case NEW_SHOP_TYPE_DECOR ... NEW_SHOP_TYPE_DECOR2:
            {
                PrintMoneyLocal(WIN_MULTI, RIGHT_ALIGNED_X, ITEM_PRICE_Y, price, COLORID_BLACK, STR_CONV_MODE_LEFT_ALIGN, FALSE);
                break;
            }''', '')

# 1486:
text = text.replace('''            case NEW_SHOP_TYPE_DECOR ... NEW_SHOP_TYPE_DECOR2:
                StringCopy(gStringVar1, gDecorations[item].name);
                break;''', '')

# 1629:
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

# 1680:
text = text.replace('''        case NEW_SHOP_TYPE_DECOR ... NEW_SHOP_TYPE_DECOR2:
            BuyMenuPrint(WIN_QUANTITY_PRICE, sText_CoinsVar1, x, 2, TEXT_SKIP_DRAW, COLORID_BLACK, FALSE);
            break;''', '')

with open("src/new_shop.c", "w") as f:
    f.write(text)

