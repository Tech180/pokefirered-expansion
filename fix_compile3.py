with open("src/new_shop.c", "r") as f:
    text = f.read()

text = text.replace('PrintMenuTable(sMartInfo.windowId, numMenuItems, sMartInfo.menuActions);', 'PrintMenuActionTexts(sMartInfo.windowId, FONT_NORMAL, 8, 2, 0, 16, numMenuItems, sMartInfo.menuActions, NULL);')
text = text.replace('IsTextPrinterActive(WIN_ITEM_DESCRIPTION)', 'IsTextPrinterActiveOnWindow(WIN_ITEM_DESCRIPTION)')

# Includes
if '#include "field_fadetransition.h"' not in text:
    text = text.replace('#include "text_window.h"', '#include "text_window.h"\n#include "field_fadetransition.h"')
if '#include "international_string_util.h"' not in text:
    text = text.replace('#include "text_window.h"', '#include "text_window.h"\n#include "international_string_util.h"\n#include "money.h"\n#include "party_menu.h"\n#include "menu.h"')

with open("src/new_shop.c", "w") as f:
    f.write(text)

