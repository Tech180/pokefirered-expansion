with open("src/new_shop.c", "r") as f:
    text = f.read()

text = text.replace('ItemId_GetPrice(', 'GetItemPrice(')

import re

# Block 1
text = re.sub(r'        case NEW_SHOP_TYPE_DECOR \.\.\. NEW_SHOP_TYPE_DECOR2:\n.*?break;\n', '', text, flags=re.DOTALL)

# Block 2
text = re.sub(r'            case NEW_SHOP_TYPE_DECOR \.\.\. NEW_SHOP_TYPE_DECOR2:\n.*?break;\n', '', text, flags=re.DOTALL)

# Let's remove any remaining NEW_SHOP_TYPE_DECOR completely!
with open("src/new_shop.c", "w") as f:
    f.write(text)

