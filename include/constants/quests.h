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
#define QUEST_THE_RUBY_AND_SAPPHIRE     10
#define QUEST_THE_EON_TICKET            11
#define QUEST_AURORA_TICKET             12
#define QUEST_MYSTIC_TICKET             13
#define QUEST_THE_OLD_AMBER             14
#define QUEST_BICYCLE_VOUCHER           15
#define QUEST_GOOD_ROD                  16
#define QUEST_SUPER_ROD                 17
#define QUEST_EXP_SHARE                 18
#define QUEST_ITEMFINDER                19
#define QUEST_AMULET_COIN               20
#define QUEST_ARTICUNOS_ROOST           21
#define QUEST_ZAPDOS_POWER              22
#define QUEST_MOLTRES_VOLCANO           23
#define QUEST_THE_UNKNOWN_DUNGEON       24
#define QUEST_THE_ROAMING_BEAST         25
#define QUEST_OLD_ROD                   26
#define QUEST_COIN_CASE                 27
#define QUEST_TEA_FOR_GUARDS            28
#define QUEST_LOST_GIRL                 29
#define QUEST_THE_BRAILLE_PUZZLE        30
#define QUEST_TEAM_ROCKETS_RETURN       31
#define QUEST_TRAINER_TOWER             32
#define QUEST_THE_ULTIMATE_COLLECTOR    33
#define QUEST_COUNT                     34

#define SUB_QUEST_COUNT                 0
#define QUEST_ARRAY_COUNT               QUEST_COUNT

#endif // GUARD_CONSTANTS_QUESTS_H