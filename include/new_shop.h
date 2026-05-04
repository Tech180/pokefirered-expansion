#ifndef GUARD_NEW_SHOP_H
#define GUARD_NEW_SHOP_H

#include "constants/shops.h"

#ifdef MUDSKIP_SHOP_UI
void NewShop_CreatePokemartMenu(const u16 *);
void CB2_ExitSellNewShopMenu(void);


void NewShop_CreateVariablePokemartMenu(const u16 *);
void NewShop_CreateCoinPokemartMenu(const u16 *);
void NewShop_CreatePointsPokemartMenu(const u16 *);
void NewShop_CreatePokemartMenuFromId(enum ShopID);

#endif // MUDSKIP_SHOP_UI

#endif // GUARD_NEW_SHOP_H
