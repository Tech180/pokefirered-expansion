#ifndef GUARD_QUEST_MENU_DISCOVERY_H
#define GUARD_QUEST_MENU_DISCOVERY_H

// Hidden Quest Discovery System
bool8 IsQuestDiscovered(u8 questId);
bool8 IsQuestHidden(u8 questId);
void UnlockQuestDiscovery(u8 questId);
const u8 *GetDisplayedQuestName(u8 questId);
const u8 *GetDisplayedQuestTitle(u8 questId);
const u8 *GetDisplayedQuestDesc(u8 questId);
const u8 *GetDisplayedQuestLocation(u8 questId);
u8 GetDisplayedQuestCountForCategory(u8 category);

// Vanilla+ Mode Toggle
bool8 GetVanillaPlusMode(void);
void SetVanillaPlusMode(bool8 enabled);
void ToggleVanillaPlusMode(void);
bool8 IsQuestVisibleInCurrentMode(u8 questId);
bool8 IsQuestPostGame(u8 questId);

#endif // GUARD_QUEST_MENU_DISCOVERY_H
