quests = [
    ("THE_JOURNEY_BEGINS", "The Journey Begins", "Deliver Oak's Parcel to Prof. Oak.", "You delivered the parcel and\\nreceived the Pokédex!", "Pallet Town", "ITEM_OAKS_PARCEL", "ITEM"),
    ("THE_FOSSIL_NERD", "The Fossil Nerd", "Defeat the Super Nerd in\\nMt. Moon.", "You chose a fossil in Mt. Moon!", "Mt. Moon", "ITEM_DOME_FOSSIL", "ITEM"),
    ("BILLS_PREDICAMENT", "Bill's Predicament", "Help Bill at the Sea Cottage on\\nRoute 25.", "You helped Bill and received\\nthe S.S. Ticket!", "Route 25", "ITEM_SS_TICKET", "ITEM"),
    ("THE_SS_ANNE", "The S.S. Anne", "Find the Captain on the S.S. Anne\\nand learn Cut.", "You rubbed the Captain's back\\nand got HM01 Cut!", "Vermilion City", "ITEM_HM01", "ITEM"),
    ("SILPH_SCOPE_RESCUE", "Silph Scope Rescue", "Defeat Team Rocket in the Game\\nCorner hideout.", "You retrieved the Silph Scope!", "Celadon City", "ITEM_SILPH_SCOPE", "ITEM"),
    ("RESTLESS_SPIRITS", "Restless Spirits", "Rescue Mr. Fuji from the\\nPokémon Tower.", "You rescued Mr. Fuji and\\ngot the Poké Flute!", "Lavender Town", "ITEM_POKE_FLUTE", "ITEM"),
    ("SAFARI_WARDEN", "Safari Warden", "Find the Warden's Gold Teeth\\nin the Safari Zone.", "You gave the Warden his teeth\\nand got HM04!", "Fuchsia City", "ITEM_GOLD_TEETH", "ITEM"),
    ("SILPH_CO_LIBERATION", "Silph Co. Liberation", "Defeat Giovanni at Silph Co.", "You saved Silph Co. and got\\nthe Master Ball!", "Saffron City", "ITEM_MASTER_BALL", "ITEM"),
    ("CINNABAR_MANSION", "Cinnabar Mansion", "Find the Secret Key in the\\nPokémon Mansion.", "You found the Secret Key to\\nBlaine's Gym!", "Cinnabar Island", "ITEM_SECRET_KEY", "ITEM"),
    ("THE_POKEMON_LEAGUE", "The Pokémon League", "Defeat the Elite Four and\\nbecome the Champion.", "You became the Pokémon League\\nChampion!", "Indigo Plateau", "ITEM_POTION", "ITEM"),
    ("THE_RUBY_AND_SAPPHIRE", "The Ruby & Sapphire", "Help Celio connect the PC network.", "You gave Celio the Ruby and\\nSapphire!", "One Island", "ITEM_RUBY", "ITEM"),
    ("THE_EON_TICKET", "The Eon Ticket", "Use the Eon Ticket to visit\\nSouthern Island.", "You encountered the Legendary\\nPokémon Latias/Latios!", "Southern Island", "ITEM_EON_TICKET", "ITEM"),
    ("AURORA_TICKET", "Aurora Ticket", "Use the Aurora Ticket to visit\\nBirth Island.", "You encountered Deoxys!", "Birth Island", "ITEM_AURORA_TICKET", "ITEM"),
    ("MYSTIC_TICKET", "Mystic Ticket", "Use the Mystic Ticket to visit\\nNavel Rock.", "You encountered Ho-Oh and Lugia!", "Navel Rock", "ITEM_MYSTIC_TICKET", "ITEM"),
    ("THE_OLD_AMBER", "The Old Amber", "Find the Old Amber in the\\nPewter City museum.", "You received the Old Amber!", "Pewter City", "ITEM_OLD_AMBER", "ITEM"),
    ("BICYCLE_VOUCHER", "Bicycle Voucher", "Listen to the Pokémon Fan Club\\nChairman.", "You got the Bicycle Voucher!", "Vermilion City", "ITEM_BICYCLE", "ITEM"),
    ("GOOD_ROD", "Good Rod", "Get the Good Rod in Fuchsia City.", "You received the Good Rod!", "Fuchsia City", "ITEM_GOOD_ROD", "ITEM"),
    ("SUPER_ROD", "Super Rod", "Get the Super Rod on Route 12.", "You received the Super Rod!", "Route 12", "ITEM_SUPER_ROD", "ITEM"),
    ("EXP_SHARE", "Exp. Share", "Catch 50 species of Pokémon.", "You received the Exp. Share!", "Route 15", "ITEM_EXP_SHARE", "ITEM"),
    ("ITEMFINDER", "Itemfinder", "Catch 30 species of Pokémon.", "You received the Itemfinder!", "Route 11", "ITEM_ITEMFINDER", "ITEM"),
    ("AMULET_COIN", "Amulet Coin", "Catch 40 species of Pokémon.", "You received the Amulet Coin!", "Route 16", "ITEM_AMULET_COIN", "ITEM"),
    ("ARTICUNOS_ROOST", "Articuno's Roost", "Navigate the Seafoam Islands.", "You encountered Articuno!", "Seafoam Islands", "ITEM_MYSTIC_WATER", "ITEM"),
    ("ZAPDOS_POWER", "Zapdos's Power", "Navigate the Power Plant.", "You encountered Zapdos!", "Power Plant", "ITEM_MAGNET", "ITEM"),
    ("MOLTRES_VOLCANO", "Moltres's Volcano", "Navigate Mt. Ember.", "You encountered Moltres!", "Mt. Ember", "ITEM_CHARCOAL", "ITEM"),
    ("THE_UNKNOWN_DUNGEON", "The Unknown Dungeon", "Explore the Cerulean Cave.", "You encountered Mewtwo!", "Cerulean Cave", "ITEM_MASTER_BALL", "ITEM"),
    ("THE_ROAMING_BEAST", "The Roaming Beast", "Find the legendary beast roaming\\nKanto.", "You encountered the roaming beast!", "Kanto Region", "ITEM_FULL_RESTORE", "ITEM"),
    ("OLD_ROD", "Old Rod", "Get the Old Rod in Vermilion City.", "You received the Old Rod!", "Vermilion City", "ITEM_OLD_ROD", "ITEM"),
    ("COIN_CASE", "Coin Case", "Get the Coin Case in Celadon City.", "You received the Coin Case!", "Celadon City", "ITEM_COIN_CASE", "ITEM"),
    ("TEA_FOR_GUARDS", "Tea for Guards", "Give Tea to the Saffron City\\nguards.", "You opened the Saffron City\\ngates!", "Saffron City", "ITEM_TEA", "ITEM"),
    ("LOST_GIRL", "Lost Girl", "Rescue Lostelle in the Berry Forest.", "You rescued Lostelle!", "Berry Forest", "ITEM_IAPAPA_BERRY", "ITEM"),
    ("THE_BRAILLE_PUZZLE", "The Braille Puzzle", "Complete the Dotted Hole puzzle.", "You solved the Braille puzzle!", "Six Island", "ITEM_SAPPHIRE", "ITEM"),
    ("TEAM_ROCKETS_RETURN", "Team Rocket's Return", "Defeat Team Rocket at the\\nRocket Warehouse.", "You defeated Team Rocket on\\nFive Island!", "Five Island", "ITEM_LIFT_KEY", "ITEM"),
    ("TRAINER_TOWER", "Trainer Tower", "Challenge the Trainer Tower.", "You cleared the Trainer Tower!", "Seven Island", "ITEM_UP_GRADE", "ITEM"),
    ("THE_ULTIMATE_COLLECTOR", "The Ultimate Collector", "Complete the Kanto Pokédex.", "You completed the Kanto Pokédex!", "Celadon City", "ITEM_TOWN_MAP", "ITEM"),
]

output = ""

for q in quests:
    output += f'static const u8 sQuestName_{q[0]}[] = _("{q[1]}");\n'
    output += f'static const u8 sQuestDesc_{q[0]}_1[] = _("{q[2]}");\n'
    output += f'static const u8 sQuestDesc_{q[0]}_2[] = _("{q[3]}");\n'
    output += f'static const u8 sQuestMap_{q[0]}[] = _("{q[4]}");\n'

output += "\nstatic const struct SideQuest sSideQuests[QUEST_COUNT] = {\n"
for q in quests:
    output += f"    [QUEST_{q[0]}] = {{\n"
    output += f"        .name = sQuestName_{q[0]},\n"
    output += f"        .desc = {{sQuestDesc_{q[0]}_1}},\n"
    output += f"        .donedesc = sQuestDesc_{q[0]}_2,\n"
    output += f"        .map = {{sQuestMap_{q[0]}}},\n"
    output += f"        .sprite = {{{q[5]}}},\n"
    output += f"        .spritetype = {{{q[6]}}},\n"
    output += f"        .questVariable = 0\n"
    output += f"    }},\n"
output += "};\n"

with open('src/data/quests.h', 'w') as f:
    f.write(output)
