#include "global.h"
#include "event_data.h"
#include "constants/flags.h"
#include "quest_menu.h"

extern const struct SideQuest sSideQuests[QUEST_COUNT];

/**
 * Hidden Quest Discovery System + Vanilla+ Mode Integration
 * 
 * This file provides helper functions and integration layers
 * for the hidden quest discovery system and the Vanilla+ mode toggle.
 *
 * Hidden Quests:
 * - Quests with a discoveryFlag set can be "hidden" from the player
 *   until the discovery flag is triggered (by completing a related objective)
 * - Undiscovered quests show "? ??" as name, "Hidden..." as title,
 *   and "Discover this quest to learn more." as description
 *
 * Vanilla+ Mode:
 * - Custom side quests can be gated behind `optionsVanillaPlusMode`
 * - When Vanilla+ mode is off, side quests are completely hidden
 * - A Debug Menu toggle allows in-game switching for testing
 */

// ============================================================
// Masked / Hidden Quest Text Constants
// ============================================================

static const u8 sText_HiddenQuest[] = _("? ??");
static const u8 sText_HiddenTitle[] = _("Hidden...");
static const u8 sText_HiddenDesc[] = _("Discover this quest to learn more.");
static const u8 sText_HiddenLocation[] = _("???");
static const u8 sText_HiddenSprite[] = _("???");
static const u8 sText_DiscoveryNotAvailable[] = _("Not available in vanilla.");

// ============================================================
// Discovery Flag Utility Functions
// ============================================================

/**
 * Check if a quest has been discovered.
 * Returns TRUE if the quest is discovered or has no discovery flag.
 * Returns FALSE if the quest has a discovery flag that is not yet set.
 */
bool8 IsQuestDiscovered(u8 questId) {
    if (questId >= QUEST_COUNT)
        return TRUE;
    if (sSideQuests[questId].name == NULL)
        return TRUE;
    if (QuestMenu_GetSetQuestState(questId, FLAG_GET_UNLOCKED) ||
        QuestMenu_GetSetQuestState(questId, FLAG_GET_ACTIVE) ||
        QuestMenu_GetSetQuestState(questId, FLAG_GET_COMPLETED) ||
        QuestMenu_GetSetQuestState(questId, FLAG_GET_FAILED)) {
        return TRUE;
    }
    if (sSideQuests[questId].discoveryFlag == 0)
        return TRUE;
    return FlagGet(sSideQuests[questId].discoveryFlag);
}

/**
 * Trigger discovery of a quest.
 * Should be called at appropriate trigger points (e.g., after
 * completing a related main quest, or triggering a hidden event).
 */
void UnlockQuestDiscovery(u8 questId) {
    if (questId >= QUEST_COUNT)
        return;
    if (sSideQuests[questId].name == NULL)
        return;
    if (sSideQuests[questId].discoveryFlag == 0)
        return;
    
    // Only set if not already discovered
    if (!FlagGet(sSideQuests[questId].discoveryFlag)) {
        FlagSet(sSideQuests[questId].discoveryFlag);
    }
}

/**
 * Check if a quest is hidden (not yet discovered) and has a discovery flag.
 * This is the inverse of IsQuestDiscovered, useful for triggering
 * discovery events.
 */
bool8 IsQuestHidden(u8 questId) {
    return !IsQuestDiscovered(questId);
}

/**
 * Get the name to display for a quest.
 * Returns the actual name if discovered, or a masked name if not.
 */
const u8 *GetDisplayedQuestName(u8 questId) {
    if (questId >= QUEST_COUNT)
        return sText_HiddenQuest;
    if (sSideQuests[questId].name == NULL)
        return sText_HiddenQuest;
    if (!IsQuestDiscovered(questId)) {
        return sText_HiddenQuest;  // Returns "? ??"
    }
    return sSideQuests[questId].name;
}

/**
 * Get the title to display for a quest.
 * Returns the actual title if discovered, or a masked title if not.
 */
const u8 *GetDisplayedQuestTitle(u8 questId) {
    if (questId >= QUEST_COUNT)
        return sText_HiddenTitle;
    if (sSideQuests[questId].name == NULL)
        return sText_HiddenTitle;
    if (!IsQuestDiscovered(questId)) {
        return sText_HiddenTitle;  // Returns "Hidden..."
    }
    if (sSideQuests[questId].title == NULL) {
        return sSideQuests[questId].name;
    }
    return sSideQuests[questId].title;
}

/**
 * Get the flavor text to display for a quest.
 * Returns the actual description if discovered, or a masked hint if not.
 */
const u8 *GetDisplayedQuestDesc(u8 questId) {
    if (questId >= QUEST_COUNT)
        return sText_HiddenDesc;
    if (sSideQuests[questId].name == NULL)
        return sText_HiddenDesc;
    if (!IsQuestDiscovered(questId)) {
        return sText_HiddenDesc;  // Returns "Discover this quest to learn more."
    }
    if (sSideQuests[questId].desc[0] == NULL) {
        return sText_HiddenDesc;
    }
    return sSideQuests[questId].desc[0];
}

/**
 * Get the location to display for a quest.
 * Returns the actual location if discovered, "? ??" if not.
 */
const u8 *GetDisplayedQuestLocation(u8 questId) {
    if (questId >= QUEST_COUNT || sSideQuests[questId].name == NULL)
        return sText_HiddenLocation;
    if (!IsQuestDiscovered(questId)) {
        return sText_HiddenLocation;  // Returns "???"
    }
    if (sSideQuests[questId].map[0] == NULL) {
        return sText_HiddenLocation;
    }
    // Location is determined by game logic elsewhere
    return sSideQuests[questId].map[0];  // Returns first map entry
}

/**
 * Get the quest count displayed in the quest menu header (numerator).
 * Returns the count of quests that are both unlocked AND discovered.
 * If no discovery system is used, this matches the normal count.
 */
u8 GetDisplayedQuestCountForCategory(u8 category) {
    u8 count = 0;
    for (u8 i = 0; i < QUEST_COUNT; i++) {
        if (!QuestMenu_GetSetQuestState(i, FLAG_GET_UNLOCKED))
            continue;
        if (!IsQuestDiscovered(i))
            continue;
        count++;
    }
    return count;
}

// ============================================================
// Vanilla+ Mode Toggle Functions
// ============================================================

/**
 * Get the current state of Vanilla+ mode.
 */
bool8 GetVanillaPlusMode(void) {
    return gSaveBlock2Ptr->optionsVanillaPlusMode;
}

/**
 * Set the Vanilla+ mode flag.
 * @param enabled TRUE to enable Vanilla+ mode, FALSE to disable.
 */
void SetVanillaPlusMode(bool8 enabled) {
    gSaveBlock2Ptr->optionsVanillaPlusMode = enabled ? 1 : 0;
}

/**
 * Toggle the Vanilla+ mode flag.
 */
void ToggleVanillaPlusMode(void) {
    gSaveBlock2Ptr->optionsVanillaPlusMode ^= 1;
}

bool8 IsQuestPostGame(u8 questId) {
    if (questId == QUEST_EMF_LEVEL_5)
        return TRUE;
    if (questId >= QUEST_DUMPSTER_DIVER && questId <= QUEST_ICEFALL_WATERFALL)
        return TRUE;
    return FALSE;
}

/**
 * Check if a given quest should be visible based on the current mode.
 * In vanilla mode, main quests are visible; side quests are hidden.
 * In Vanilla+ mode, all quests are visible.
 */
bool8 IsQuestVisibleInCurrentMode(u8 questId) {
    if (questId >= QUEST_COUNT)
        return FALSE;
    if (sSideQuests[questId].name == NULL)
        return FALSE;
    
    // Post-game quests are hidden until the Hall of Fame is cleared
    if (IsQuestPostGame(questId) && !FlagGet(FLAG_SYS_GAME_CLEAR)) {
        return FALSE;
    }
    
    if (GetVanillaPlusMode()) {
        return TRUE;  // All quests visible in Vanilla+ mode
    }
    
    // In vanilla mode, only main quests are visible
    return !sSideQuests[questId].isSideQuest;
}
