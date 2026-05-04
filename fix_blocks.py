with open(".tmp_pokeemerald_shop/src/new_shop.c", "r") as f:
    text = f.read()

import re
matches = re.findall(r'(        case NEW_SHOP_TYPE_DECOR \.\.\. NEW_SHOP_TYPE_DECOR2:.*?)        #ifdef MUDSKIP_OUTFIT_SYSTEM', text, re.DOTALL)
for i, match in enumerate(matches):
    print(f"MATCH {i}:")
    print(repr(match))
    
