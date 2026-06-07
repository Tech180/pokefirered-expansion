#ifndef GUARD_CONSTANTS_QUESTS_H
#define GUARD_CONSTANTS_QUESTS_H

// questmenu scripting command params
#define QUEST_MENU_OPEN 0               // opens the quest menu (questId = 0)
#define QUEST_MENU_UNLOCK_QUEST 1       // questId = QUEST_X (0-indexed)
#define QUEST_MENU_SET_ACTIVE 2         // questId = QUEST_X (0-indexed)
#define QUEST_MENU_SET_REWARD 3         // questId = QUEST_X (0-indexed)
#define QUEST_MENU_COMPLETE_QUEST 4     // questId = QUEST_X (0-indexed)
#define QUEST_MENU_CHECK_UNLOCKED 5     // checks if questId has been unlocked. Returns result to gSpecialVar_Result
#define QUEST_MENU_CHECK_INACTIVE 6     // check if a questId is inactive. Returns result to gSpecialVar_Result
#define QUEST_MENU_CHECK_ACTIVE 7       // checks if questId has been unlocked. Returns result to gSpecialVar_Result
#define QUEST_MENU_CHECK_REWARD 8       // checks if questId is in Reward state. Returns result to gSpecialVar_Result
#define QUEST_MENU_CHECK_COMPLETE 9     // checks if questId has been completed. Returns result to gSpecialVar_Result
#define QUEST_MENU_BUFFER_QUEST_NAME 10 // buffers a quest name to gStringVar1
#define QUEST_MENU_COMPLETE_SUBQUEST 11 // completes a subquest
#define QUEST_MENU_CHECK_SUBQUEST_COMPLETE 12 // checks if a subquest is completed
#define QUEST_MENU_SET_FAILED 13        // questId = QUEST_X (0-indexed)
#define QUEST_MENU_CHECK_FAILED 14      // checks if questId is in Failed state. Returns result to gSpecialVar_Result
#define QUEST_MENU_UNLOCK_DISCOVERY 15  // unlocks quest discovery flag

// quest number defines
#define QUEST_NONE                      0xFFFF
#define QUEST_THE_JOURNEY_BEGINS        0
#define QUEST_THE_FOSSIL_NERD           1
#define QUEST_BILLS_PREDICAMENT         2
#define QUEST_THE_SS_ANNE               3
#define QUEST_SILPH_SCOPE_RESCUE        4
#define QUEST_RESTLESS_SPIRITS          5
#define QUEST_SAFARI_WARDEN             6
#define QUEST_SILPH_CO_LIBERATION       7
#define QUEST_CINNABAR_MANSION          8
#define QUEST_THE_POKEMON_LEAGUE        9
#define QUEST_THE_OLD_AMBER             10
#define QUEST_BICYCLE_VOUCHER           11
#define QUEST_GOOD_ROD                  12
#define QUEST_SUPER_ROD                 13
#define QUEST_EXP_SHARE                 14
#define QUEST_ITEMFINDER                15
#define QUEST_AMULET_COIN               16
#define QUEST_OLD_ROD                   17
#define QUEST_COIN_CASE                 18
#define QUEST_TEA_FOR_GUARDS            19
#define QUEST_THE_ULTIMATE_COLLECTOR    20
#define QUEST_GYM_CHALLENGES            21
#define QUEST_LEGENDARY_POKEMON         22
#define QUEST_FIRERED_EXCLUSIVES        23
#define QUEST_LEAFGREEN_EXCLUSIVES      24
#define DEX_REGION_NONE       0
#define DEX_REGION_KANTO      1
#define DEX_REGION_JOHTO      2
#define DEX_REGION_HOENN      3
#define DEX_REGION_SINNOH     4
#define DEX_REGION_UNOVA      5
#define DEX_REGION_KALOS      6
#define DEX_REGION_ALOLA      7
#define DEX_REGION_GALAR      8
#define DEX_REGION_PALDEA     9

#define QUEST_SEVII_ISLANDS             25
#define QUEST_ROUTE22_RIVAL             26
#define QUEST_THE_TREASURE_HUNTER      27
#define QUEST_SLEEPING_ON_THE_JOB      28
#define QUEST_REST_IN_PEACE            29
#define QUEST_THE_GYM_INSPECTOR        30
#define QUEST_VENDING_MACHINE_ROULETTE 31
#define QUEST_THE_GRAND_BLUE_MILE      32
#define QUEST_ROCK_AND_STONE           33
#define QUEST_THE_SAFARI_PURIST        34
#define QUEST_MUTUAL_ASSURANCE         35
#define QUEST_THE_FOSSIL_FANATIC       36
#define QUEST_THE_HOARDER              37
#define QUEST_THE_SEASICK_SAILOR       38
#define QUEST_FUCHSIA_ZOO_SIGNAGE      39
#define QUEST_EMF_LEVEL_5              40
#define QUEST_SPACE_EXHIBIT_VANDALISM  41
#define QUEST_COMPREHENSIVE_COVERAGE   42
#define QUEST_THE_S_S_ANNE_TRUCK       43
#define QUEST_ROUTE_1_LEDGE_MASTER     44
#define QUEST_CYCLING_ROAD_SPEED_TRAP  45
#define QUEST_THE_REPEL_MASTER         46
#define QUEST_THE_EEVEE_SCHOLAR        47
#define QUEST_SILPH_CO_NAPTIME         48
#define QUEST_SCAVENGER                49
#define QUEST_BIKE_SHOP_REBELLION      50
#define QUEST_SAFARI_SPEEDRUNNER       51
#define QUEST_FUCHSIA_INVISIBLE_WALLS  52
#define QUEST_THE_COUCH_POTATO         53
#define QUEST_CONSOLE_WARS             54
#define QUEST_DEFORESTATION            55
#define QUEST_BOULDER_BUSTER           56
#define QUEST_THIRSTY_WORK             57
#define QUEST_PLUSH_COLLECTOR          58
#define QUEST_BIRD_WATCHER             59
#define QUEST_BROKE_DESPERATE          60
#define QUEST_CERULEAN_BIKE_LOOPHOLE   61
#define QUEST_SCIENCE_IS_AMAZING       62
#define QUEST_THE_JACKPOT              63
#define QUEST_DUMPSTER_DIVER           64
#define QUEST_LORE_HISTORIAN           65
#define QUEST_PARANORMAL_INVESTIGATOR  66
#define QUEST_PIKACHU_S_BEACH          67
#define QUEST_FISHING_GURU_REUNION     68
#define QUEST_THE_COPYCAT_S_MIMIC      69
#define QUEST_BERRY_BLENDER            70
#define QUEST_LORELEI_S_MENAGERIE      71
#define QUEST_THE_TANOBY_DECIPHERER    72
#define QUEST_LOST_IN_THE_FOG          73
#define QUEST_THE_TIME_TRAVELER        74
#define QUEST_GENETIC_MEMORY           75
#define QUEST_DE_EXTINCTION_EXPERT     76
#define QUEST_THE_PERFECTIONIST        77
#define QUEST_THE_PACIFIST             78
#define QUEST_THE_INVISIBLE_PC         79
#define QUEST_RUBBER_DUCK_DEBUGGING    80
#define QUEST_THE_MAGIKARP_SWINDLE     81
#define QUEST_THE_UNUSED_BED           82
#define QUEST_LEAVE_NO_DWARF_BEHIND    83
#define QUEST_VERMILION_CONSTRUCTION   84
#define QUEST_THE_WARDEN_S_MESS        85
#define QUEST_GIOVANNI_S_THRONE        86
#define QUEST_THE_HYPNOTIZED_PATIENT   87
#define QUEST_THE_THIRSTY_GUARDS       88
#define QUEST_SAFFRON_CLUB_NATURES     89
#define QUEST_THE_FOSSILIZED_MUD       90
#define QUEST_CINNABAR_TRADE_REGRET    91
#define QUEST_CINNABAR_COASTLINE       92
#define QUEST_BILL_S_GARDEN_RUMOR      93
#define QUEST_SS_ANNE_DEPARTURE_SKIP   94
#define QUEST_THE_BLANK_SLATE          95
#define QUEST_THE_ALCHEMIST_S_TRASH    96
#define QUEST_THE_WITCHING_HOUR        97
#define QUEST_MORNING_DEW              98
#define QUEST_HIGH_TIDE_AT_SEAFOAM     99
#define QUEST_THIRSTY_GIRL_S_SECRET    100
#define QUEST_THE_SCULPTOR_S_MUSE      101
#define QUEST_THE_IMPOSTER_SYNDROME    102
#define QUEST_THE_ICE_SKATER           103
#define QUEST_THE_PEWTER_GEOLOGIST     104
#define QUEST_OAK_S_INBOX              105
#define QUEST_CINNABAR_ARCHIVIST       106
#define QUEST_PERFECT_FRIENDSHIP       107
#define QUEST_TRAINER_TOWER_PACIFISM   108
#define QUEST_THE_TANOBY_CIPHER        109
#define QUEST_MT_EMBER_S_CORE          110
#define QUEST_ICEFALL_WATERFALL        111
#define QUEST_THE_MANIAC_ON_THE_CAPE   112
#define QUEST_SIBLING_RIVALRY           113

#define QUEST_COUNT                     114

// Subquests for Gym Challenges
#define SUB_QUEST_GYM_BROCK             0
#define SUB_QUEST_GYM_MISTY             1
#define SUB_QUEST_GYM_LTSURGE           2
#define SUB_QUEST_GYM_ERIKA             3
#define SUB_QUEST_GYM_KOGA              4
#define SUB_QUEST_GYM_SABRINA           5
#define SUB_QUEST_GYM_BLAINE            6
#define SUB_QUEST_GYM_GIOVANNI          7

// Subquests for Legendary Pokemon
#define SUB_QUEST_LEGENDARY_ARTICUNO    8
#define SUB_QUEST_LEGENDARY_ZAPDOS      9
#define SUB_QUEST_LEGENDARY_MOLTRES     10
#define SUB_QUEST_LEGENDARY_MEWTWO      11
#define SUB_QUEST_LEGENDARY_RAIKOU      12
#define SUB_QUEST_LEGENDARY_ENTEI       13
#define SUB_QUEST_LEGENDARY_SUICUNE     14
#define SUB_QUEST_LEGENDARY_LUGIA       15
#define SUB_QUEST_LEGENDARY_HO_OH       16

// Subquests for FireRed Exclusives
#define SUB_QUEST_FR_EKANS              17
#define SUB_QUEST_FR_ODDISH             18
#define SUB_QUEST_FR_MANKEY             19
#define SUB_QUEST_FR_GROWLITHE          20
#define SUB_QUEST_FR_SHELLDER           21
#define SUB_QUEST_FR_SCYTHER            22
#define SUB_QUEST_FR_ELECTABUZZ         23
#define SUB_QUEST_FR_MURKROW            24
#define SUB_QUEST_FR_QWILFISH           25
#define SUB_QUEST_FR_DELIBIRD           26
#define SUB_QUEST_FR_SKARMORY           27

// Subquests for LeafGreen Exclusives
#define SUB_QUEST_LG_SANDSHREW          28
#define SUB_QUEST_LG_BELLSPROUT         29
#define SUB_QUEST_LG_MEOWTH             30
#define SUB_QUEST_LG_VULPIX             31
#define SUB_QUEST_LG_STARYU             32
#define SUB_QUEST_LG_PINSIR             33
#define SUB_QUEST_LG_MAGMAR             34
#define SUB_QUEST_LG_MISDREAVUS         35
#define SUB_QUEST_LG_REMORAID           36
#define SUB_QUEST_LG_SNEASEL            37
#define SUB_QUEST_LG_MANTINE            38

// Subquests for Sevii Islands
#define SUB_QUEST_SEVII_CELIO           39
#define SUB_QUEST_SEVII_LOSTELLE        40
#define SUB_QUEST_SEVII_BRAILLE         41
#define SUB_QUEST_SEVII_ROCKET          42
#define SUB_QUEST_SEVII_TOWER           43

// Subquests for The Ultimate Collector
#define SUB_QUEST_DEX_KANTO             44
#define SUB_QUEST_DEX_JOHTO             45
#define SUB_QUEST_DEX_HOENN             46
#define SUB_QUEST_DEX_SINNOH            47
#define SUB_QUEST_DEX_UNOVA             48
#define SUB_QUEST_DEX_KALOS             49
#define SUB_QUEST_DEX_ALOLA             50
#define SUB_QUEST_DEX_GALAR             51
#define SUB_QUEST_DEX_PALDEA            52

#define SUB_QUEST_COUNT                 53
#define QUEST_ARRAY_COUNT               QUEST_COUNT

#endif // GUARD_CONSTANTS_QUESTS_H