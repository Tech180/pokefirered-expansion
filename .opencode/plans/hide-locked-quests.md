# Plan: Hide Locked Quests in Quest Menu

## Goal
Only show unlocked quests in the quest menu. Locked quests should be completely hidden (not shown as "??" entries).

## Current Behavior
- All 42 quests appear in the list
- Locked quests are displayed as "??" with no details
- The quest count (e.g., "0/42") shows locked quests in the denominator

## Changes Needed

### 1. `src/quest_menu.c` - `GenerateList()` (line 1031)
**Add a locked filter before processing any quest:**

```c
// Add this check right after defining sortedQuestList, before the loop:
// Or at the start of the loop, before any other checks:
if (!QuestMenu_GetSetQuestState(selectedQuestId, FLAG_GET_UNLOCKED))
{
    continue;
}
```

This needs to be the **first** filter in the loop (before `isFiltered` and `DoesQuestMatchCategory` checks), so locked quests are completely skipped.

Currently the loop at line 1040-1070 processes every quest. The new unlock check goes before the `isFiltered` check.

### 2. `include/quest_menu.h` - Quest count headers (optional UX improvement)
The denominator currently shows total quests via `CountQuestsInCategory()`. Consider whether the header names should change:
- "All Missions" → "Missions" 
- Or leave as-is since it's cosmetic

## Files to Modify
| File | Change |
|------|--------|
| `src/quest_menu.c` ~line 1042 | Add `FLAG_GET_UNLOCKED` check as first filter in `GenerateList()` |

## No Data Changes Needed
- `src/data/quests.h` - No changes (quest definitions stay the same)
- `include/constants/quests.h` - No changes (constants stay the same)
