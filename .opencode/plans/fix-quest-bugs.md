# Quest System Bug Fixes

## Summary
4 bugs in the quest system causing:
1. All quests showing as "Active" (inactive filter includes locked quests)
2. "ALL ACTIVATING" text showing (denominator/count mismatch when locked)
3. LOCK_ALL not working (missing case 0 in state setter)
4. Inactive check not verifying unlocked state

## Changes

### Fix 1: FLAG_GET_INACTIVE must check unlock state (quest_menu.c:1214-1235)
- Currently only checks bits 1,2,3 are zero, never verifies bit 0 is set
- Fix: also check bit 0 (unlock) is set before returning true

### Fix 2: Add case 0 to QuestMenu_SetQuestState (quest_menu.c:1113-1159)
- LOCK_ALL calls SetQuestState(i, 0) but switch only has cases 1,2,3
- Fix: add case 0 that leaves all bits cleared

### Fix 3: CountQuestsInCategory must filter by unlocked (quest_menu.c:1264-1281)
- Counts all Main/Side quests regardless of state
- Fix: add FLAG_GET_UNLOCKED check

### Fix 4: CountInactiveQuests also broken (quest_menu.c:1295-1310)
- Uses FLAG_GET_INACTIVE which includes locked quests
- Fix: already handled by Fix 1

## Files Modified
- src/quest_menu.c
