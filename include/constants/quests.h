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
#define QUEST_SEVII_ISLANDS             25
#define QUEST_COUNT                     26

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

#define SUB_QUEST_COUNT                 44
#define QUEST_ARRAY_COUNT               QUEST_COUNT

#endif // GUARD_CONSTANTS_QUESTS_H