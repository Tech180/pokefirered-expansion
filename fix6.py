import re

with open("src/new_shop.c", "r") as f:
    text = f.read()

# Block 1
text = re.sub(r'        case NEW_SHOP_TYPE_DECOR \.\.\. NEW_SHOP_TYPE_DECOR2:\n        \{\n            // DECOR_NONE has the same value as ITEM_NONE but this is for clarity\n            if \(sMartInfo\.itemList\[i\] == DECOR_NONE\)\n            \{\n                sShopData->gridItems->iconSpriteIds\[idx\] = AddItemIconSprite\(GFXTAG_ITEM \+ idx, PALTAG_ITEM \+ idx, ITEM_LIST_END\);\n                gSprites\[sShopData->gridItems->iconSpriteIds\[idx\]\]\.x = x;\n                gSprites\[sShopData->gridItems->iconSpriteIds\[idx\]\]\.y = y;\n            \}\n            else\n            \{\n                sShopData->gridItems->iconSpriteIds\[idx\] = AddDecorationIconObject\(sMartInfo\.itemList\[i\], x, y, 2, GFXTAG_ITEM \+ idx, PALTAG_ITEM \+ idx\);\n            \}\n            break;\n        \}\n', '', text)

# Block 2
text = re.sub(r'            case NEW_SHOP_TYPE_DECOR \.\.\. NEW_SHOP_TYPE_DECOR2:\n            \{\n                StringCopy\(gStringVar1, gDecorations\[sShopData->currentItemId\]\.name\);\n                ConvertIntToDecimalStringN\(gStringVar2, sShopData->totalCost, STR_CONV_MODE_LEFT_ALIGN, MAX_MONEY_DIGITS\);\n                BuyMenuDisplayMessage\(taskId, str, BuyMenuConfirmPurchase\);\n                break;\n            \}\n', '', text)

# Block 3
text = re.sub(r'        case NEW_SHOP_TYPE_DECOR \.\.\. NEW_SHOP_TYPE_DECOR2:\n        \{\n            if \(DecorationAdd\(sShopData->currentItemId\)\)\n            \{\n                BuyMenuSubtractMoney\(taskId\);\n                BuyMenuDisplayMessage\(taskId, Shop_GetSellerMessage\(SELLER_MSG_BUY_SUCCESS\), Task_ReturnToItemListWaitMsg\);\n                RecordItemPurchase\(taskId\);\n            \}\n            else\n            \{\n                BuyMenuDisplayMessage\(taskId, Shop_GetSellerMessage\(SELLER_MSG_BUY_FAIL_NO_SPACE\), Task_ReturnToItemListWaitMsg\);\n            \}\n            break;\n        \}\n', '', text)

# Block 4
text = re.sub(r'        case NEW_SHOP_TYPE_DECOR \.\.\. NEW_SHOP_TYPE_DECOR2:\n            BuyMenuPrint\(WIN_QUANTITY_PRICE, sText_CoinsVar1, x, 2, TEXT_SKIP_DRAW, COLORID_BLACK, FALSE\);\n            break;\n', '', text)

# Block 5: The end of file `#endif` / curly brace missing issue:
# In `tmp_compile5` we saw:
# src/new_shop.c:1789:5: error: expected identifier or ‘(’ before ‘}’ token
# src/new_shop.c:1791:5: error: expected identifier or ‘(’ before ‘if’
# src/new_shop.c:1797:1: error: expected identifier or ‘(’ before ‘}’ token

with open("src/new_shop.c", "w") as f:
    f.write(text)

