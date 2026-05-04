with open("src/new_shop.c", "r") as f:
    text = f.read()

text = text.replace('!CheckCoins(sShopData->totalCost)', '(GetCoins() < sShopData->totalCost)')
text = text.replace('!CheckBattlePoints(sShopData->totalCost)', '(0) /* BP unsupported */')
text = text.replace('GetBattlePoints()', '0')
text = text.replace('RemoveBattlePoints(sShopData->totalCost);', '/* BP unsupported */')

# replace item price macros which might be pokeemerald specific
text = text.replace('GetItemPrice(sMartInfo.itemList[id])', 'ItemId_GetPrice(sMartInfo.itemList[id])')

with open("src/new_shop.c", "w") as f:
    f.write(text)

