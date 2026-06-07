#include "global.h"

#include "bg.h"
#include "constants/event_objects.h"
#include "constants/field_effects.h"
#include "constants/field_weather.h"
#include "constants/items.h"
#include "constants/flags.h"
#include "constants/rgb.h"
#include "constants/songs.h"
#include "constants/trainer_types.h"
#include "data.h"
#include "decompress.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_effect.h"
#include "field_effect_helpers.h"
#include "gpu_regs.h"
#include "graphics.h"
#include "item.h"
#include "item_icon.h"
#include "item_menu.h"
#include "item_menu_icons.h"
#include "item_use.h"
#include "list_menu.h"
#include "main.h"
#include "malloc.h"
#include "menu.h"
#include "menu_helpers.h"
#include "move.h"
#include "overworld.h"
#include "palette.h"
#include "party_menu.h"
#include "pokedex.h"
#include "pokemon_icon.h"
#include "quest_menu.h"
#include "quest_menu_discovery.h"
#include "scanline_effect.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "text_window.h"
#include "trainer_see.h"
#include "trig.h"

#include "random.h"

#define tPageItems data[4]
#define tItemPcParam data[6]

struct QuestMenuResources {
  MainCallback savedCallback;
  u8 moveModeOrigPos;
  u8 spriteIconSlot;
  u16 oldPaletteTag;
  u8 maxShowed;
  u8 nItems;
  u8 scrollIndicatorArrowPairId;
  s16 data[3];
  u8 filterMode;
  u8 parentQuest;
  bool8 restoreCursor;
  u8 selectorMainOrSide;
  bool8 isSelectorFocused;
};

struct QuestMenuStaticResources {
  MainCallback savedCallback;
  u16 scroll;
  u16 row;
  u8 initialized;
  u16 storedScrollOffset;
  u16 storedRowPosition;
};

// RAM
EWRAM_DATA static struct QuestMenuResources *sStateDataPtr = NULL;
EWRAM_DATA static u8 *sBg1TilemapBuffer = NULL;
EWRAM_DATA static struct ListMenuItem *sListMenuItems = NULL;
EWRAM_DATA static struct QuestMenuStaticResources sListMenuState = {0};
EWRAM_DATA static u8 sItemMenuIconSpriteIds[12] = {
    0}; // from pokefirered src/item_menu_icons.c
EWRAM_DATA static void *questNamePointer = NULL;
EWRAM_DATA static u8 **questNameArray = NULL;

// This File's Functions
void QuestMenu_Init(u8 a0, MainCallback callback);
static void MainCB(void);
static void VBlankCB(void);
static void RunSetup(void);

static bool8 SetupGraphics(void);
static bool8 LoadGraphics(void);
static void QuestMenu_InitWindows(void);
static void DrawSelectorBox(bool8 isFocused);
static bool8 InitBackgrounds(void);
static void InitItems(void);
static bool8 AllocateResourcesForListMenu(void);
static void AllocateMemoryForArray();
static void PlaceTopMenuScrollIndicatorArrows(void);
static void SetInitializedFlag(u8 a0);

static u8 GetCursorPosition(void);
static void SetCursorPosition(void);
static void SetScrollPosition(void);
static bool8 IfScrollIsOutOfBounds(void);
static bool8 IfRowIsOutOfBounds(void);
static void SaveScrollAndRow(s16 *data);

static void ClearModeOnStartup(void);
static u8 ManageMode(u8 action);
static u8 ToggleAlphaMode(u8 mode);
static u8 ToggleSubquestMode(u8 mode);
static u8 IncrementMode(u8 mode);
static u8 DecrementMode(u8 mode);
static bool8 IsSubquestMode(void);
static bool8 IsNotFilteredMode(void);
static bool8 IsAlphaMode(void);

static void BuildMenuTemplate(void);
static u8 GetModeAndGenerateList();
static u8 CountNumberListRows();
static u8 *DefineQuestOrder();
static u8 GenerateSubquestList();
static u8 GenerateList(bool8 isFiltered);
static void AssignCancelNameAndId(u8 numRow);

static u8 CountUnlockedQuests(void);
static u8 CountInactiveQuests(void);
static u8 CountActiveQuests(void);
static u8 CountRewardQuests(void);
static u8 CountCompletedQuests(void);
static u8 CountFailedQuests(void);
static u8 CountFavoriteQuests(void);

static void PopulateEmptyRow(u8 countQuest);
static void PrependQuestNumber(u8 countQuest);
static void SetFavoriteQuest(u8 countQuest);
static void PopulateQuestName(u8 countQuest);
static void PopulateSubquestName(u8 parentQuest, u8 countQuest);
static void PopulateListRowNameAndId(u8 row, u8 countQuest);
static bool8 DoesQuestHaveChildrenAndNotInactive(u16 itemId);
static void AddSubQuestButton(u8 countQuest);

static void QuestMenu_AddTextPrinterParameterized(u8 windowId, u8 fontId,
                                                  const u8 *str, u8 x, u8 y,
                                                  u8 letterSpacing,
                                                  u8 lineSpacing, u8 speed,
                                                  u8 colorIdx);

static void MoveCursorFunc(s32 itemIndex, bool8 onInit, struct ListMenu *list);
static void PlayCursorSound(bool8 firstRun);
static void PrintDetailsForCancel();
static void GenerateAndPrintQuestDetails(s32 questId);
static void GenerateQuestLocation(s32 questId);
static void PrintQuestLocation(s32 questId);
static void GenerateQuestFlavorText(s32 questId);
static void UpdateQuestFlavorText(s32 questId);
static void PrintQuestFlavorText(s32 questId);
static const u8 *GetQuestDesc(s32 questId);
static const u8 *GetQuestLocation(s32 questId);

static bool8 IsQuestUnlocked(s32 questId);
static bool8 IsQuestActiveState(s32 questId);
static bool8 IsQuestInactiveState(s32 questId);
static bool8 IsQuestRewardState(s32 questId);
static bool8 IsQuestFailedState(s32 questId);
static bool8 IsQuestCompletedState(s32 questId);
static bool8 IsSubquestCompletedState(s32 questId);

static void DetermineSpriteType(s32 questId);
static void QuestMenu_CreateSprite(u16 itemId, u8 idx, u8 spriteType);
static void ResetSpriteState(void);
static void QuestMenu_DestroySprite(u8 idx);
static u32 GetQuestSprite(s32 questId);
static u32 GetQuestSpriteType(s32 questId);

static void GenerateStateAndPrint(u8 windowId, u32 itemId, u8 y);
static u8 GenerateSubquestState(u8 questId);
static u8 GenerateQuestState(u8 questId);
static void PrintQuestState(u8 windowId, u8 y, u8 colorIndex);

static void GenerateAndPrintHeader(void);
static void GenerateDenominatorNumQuests(void);
static void GenerateNumeratorNumQuests(void);
static void GenerateMenuContext(void);
static void PrintNumQuests(void);
static void PrintMenuContext(void);
static void PrintTypeFilterButton(void);

static void Task_Main(u8 taskId);
static void ClaimQuestReward(u8 taskId, u8 questId);
static void ClaimSubquestReward(u8 taskId, u8 parentQuest, u8 subQuestIndex);
static void Task_ClaimRewardMessage(u8 taskId);
static void ManageFavorites(u8 index);
static void Task_QuestMenuCleanUp(u8 taskId);
static void RestoreSavedScrollAndRow(s16 *data);
static void ResetCursorToTop(s16 *data);
static void QuestMenu_RemoveScrollIndicatorArrowPair(void);
static void EnterSubquestModeAndCleanUp(u8 taskId, s16 *data, s32 input);
static void ChangeModeAndCleanUp(u8 taskId);
static void ChangeModeDecrementAndCleanUp(u8 taskId);
static void ToggleAlphaModeAndCleanUp(u8 taskId);
static void ToggleFavoriteAndCleanUp(u8 taskId, u8 selectedQuestId);
static bool8 CheckSelectedIsCancel(u8 selectedQuestId);
static void ReturnFromSubquestAndCleanUp(u8 taskId);

static void SetGpuRegBaseForFade(void);
static void InitFadeVariables(u8 taskId, u8 blendWeight, u8 frameDelay,
                              u8 frameTimerBase, u8 delta);
static void PrepareFadeOut(u8 taskId);
static bool8 HandleFadeOut(u8 taskId);
static void PrepareFadeIn(u8 taskId);
static bool8 HandleFadeIn(u8 taskId);
static void Task_FadeOut(u8 taskId);
static void Task_FadeIn(u8 taskId);

static void Task_QuestMenuWaitFadeAndBail(u8 taskId);
static void FadeAndBail(void);
static void FreeResources(void);
static void TurnOffQuestMenu(u8 taskId);
static void Task_QuestMenuTurnOff1(u8 taskId);
static void Task_QuestMenuTurnOff2(u8 taskId);
static void QuestMenu_DrawBottomRightBox(bool8 draw);
static void QuestMenu_CreateBottomRightRewardSprite(u16 itemId);
static void QuestMenu_DestroyBottomRightSprite(void);
static void QuestMenu_UpdateBottomRightReward(s32 questId);

// Quest icon functions
#if 0
static void SpawnQuestIconForObject(struct ObjectEvent*,  u32);
static void RemoveQuestIconFieldEffect(struct ObjectEvent *objectEvent);
static void SetQuestIconOnObject(struct ObjectEvent*);
static bool32 ObjectEventAlreadyHasQuest(bool32);
#endif

// Tiles, palettes and tilemaps for the Quest Menu
static const u32 sQuestMenuTiles[] =
    INCBIN_U32("graphics/quest_menu/menu.4bpp.lz");
static const u32 sQuestMenuBgPals[] =
    INCBIN_U32("graphics/quest_menu/menu.gbapal");
static const u32 sQuestMenuTilemap[] =
    INCBIN_U32("graphics/quest_menu/menu.bin.lz");

// Strings used for the Quest Menu
static const u8 sText_Empty[] = _("");
static const u8 sText_Beat[] = _("Beat");
static const u8 sText_AllHeader[] = _("All Missions");
static const u8 sText_InactiveHeader[] = _("Inactive Missions");
static const u8 sText_ActiveHeader[] = _("Active Missions");
static const u8 sText_RewardHeader[] = _("Reward Available");
static const u8 sText_CompletedHeader[] = _("Completed Missions");
static const u8 sText_FailedHeader[] = _("Failed Missions");
static const u8 sText_QuestNumberDisplay[] = _("{STR_VAR_1}/{STR_VAR_2}");
static const u8 sText_Unk[] = _("??????");
static const u8 sText_Active[] = _("Active");
static const u8 sText_Reward[] = _("Reward");
static const u8 sText_Complete[] = _("Done");
static const u8 sText_Failed[] = _("Failed");
static const u8 sText_StartForMore[] = _("Start for more details.");
static const u8 sText_ReturnRecieveReward[] =
    _("Return to {STR_VAR_2}\nto recieve your reward!");
static const u8 sText_SubQuestButton[] = _(" {A_BUTTON}");
static const u8 sText_LType[] = _("{L_BUTTON}");
static const u8 sText_TypeR[] = _("Type{R_BUTTON}");
static const u8 sText_MainSelector[] = _("  Main  ");
static const u8 sText_SideSelector[] = _("  Side  ");
static const u8 sText_MainSelectorFocused[] = _(" >Main  ");
static const u8 sText_SideSelectorFocused[] = _(" >Side  ");
static const u8 sText_Caught[] = _("Caught");
static const u8 sText_Found[] = _("Found");
static const u8 sText_Read[] = _("Read");
static const u8 sText_Back[] = _("Back");
static const u8 sText_DotSpace[] = _(". ");
static const u8 sText_Close[] = _("Close");
static const u8 sText_ColorGreen[] = _("{COLOR}{GREEN}");
static const u8 sText_AZ[] = _(" A-Z");

// Hidden quest strings (for undiscovered quests)
static const u8 sText_HiddenQuest[] = _("? ??");
static const u8 sText_HiddenTitle[] = _("Hidden...");
static const u8 sText_HiddenDesc[] = _("Discover this quest to learn more.");
static const u8 sText_HiddenLocation[] = _("???");

#include "data/quests.h"
///////////////////////////////////////////////////////////////////////////////

// BG layer defintions
static const struct BgTemplate sQuestMenuBgTemplates[2] = {
    {// All text and content is loaded to this window
     .bg = 0,
     .charBaseIndex = 0,
     .mapBaseIndex = 31,
     .priority = 1},
    {/// Backgrounds and UI elements are loaded to this window
     .bg = 1,
     .charBaseIndex = 3,
     .mapBaseIndex = 30,
     .priority = 2}};

// Window definitions
static const struct WindowTemplate sQuestMenuHeaderWindowTemplates[] = {
    {// 0: Content window
     .bg = 0,
     .tilemapLeft = 0,
     .tilemapTop = 2,
     .width = 30,
     .height = 8,
     .paletteNum = 15,
     .baseBlock = 1},
    {// 1: Footer window
     .bg = 0,
     .tilemapLeft = 0,
     .tilemapTop = 12,
     .width = 30,
     .height = 12,
     .paletteNum = 15,
     .baseBlock = 361},
    {// 2: Header window
     .bg = 0,
     .tilemapLeft = 0,
     .tilemapTop = 0,
     .width = 30,
     .height = 2,
     .paletteNum = 15,
     .baseBlock = 721},
    {// 3: Selector window
     .bg = 0,
     .tilemapLeft = 3,
     .tilemapTop = 0,
     .width = 8,
     .height = 2,
     .paletteNum = 15,
     .baseBlock = 781},
    DUMMY_WIN_TEMPLATE};

// Font color combinations for printed text
static const u8 sQuestMenuWindowFontColors[][4] = {
    {// Header of Quest Menu
     TEXT_COLOR_TRANSPARENT, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_TRANSPARENT},
    {// Reward state progress indicator
     TEXT_COLOR_TRANSPARENT, TEXT_COLOR_LIGHT_BLUE, TEXT_COLOR_TRANSPARENT},
    {// Done state progress indicator
     TEXT_COLOR_TRANSPARENT, TEXT_COLOR_GREEN, TEXT_COLOR_TRANSPARENT},
    {// Active state progress indicator
     TEXT_COLOR_TRANSPARENT, TEXT_COLOR_BLUE, TEXT_COLOR_TRANSPARENT},
    {// Footer flavor text
     TEXT_COLOR_TRANSPARENT, TEXT_COLOR_WHITE, TEXT_COLOR_TRANSPARENT},
    {// Failed state progress indicator (Red)
     TEXT_COLOR_TRANSPARENT, TEXT_COLOR_RED, TEXT_COLOR_TRANSPARENT},
};

// Functions begin here

// ported from firered by ghoulslash
void QuestMenu_Init(u8 a0, MainCallback callback) {
  u8 i;

  if (a0 >= 2) {
    SetMainCallback2(callback);
    return;
  }

  if ((sStateDataPtr = Alloc(sizeof(struct QuestMenuResources))) == NULL) {
    SetMainCallback2(callback);
    return;
  }

  if (a0 != 1) {
    sListMenuState.savedCallback = callback;
    sListMenuState.scroll = sListMenuState.row = 0;
  }

  sStateDataPtr->moveModeOrigPos = 0xFF;
  sStateDataPtr->spriteIconSlot = 0;
  sStateDataPtr->scrollIndicatorArrowPairId = 0xFF;
  sStateDataPtr->savedCallback = 0;
  for (i = 0; i < 3; i++) {
    sStateDataPtr->data[i] = 0;
  }

  SetMainCallback2(RunSetup);
}

static void MainCB(void) {
  RunTasks();
  AnimateSprites();
  BuildOamBuffer();
  DoScheduledBgTilemapCopiesToVram();
  UpdatePaletteFade();
}

static void VBlankCB(void) {
  LoadOam();
  ProcessSpriteCopyRequests();
  TransferPlttBuffer();
}

static void RunSetup(void) {
  while (1) {
    if (SetupGraphics() == TRUE) {
      break;
    }
  }
}

static bool8 SetupGraphics(void) {
  u8 taskId;
  switch (gMain.state) {
  case 0:
    SetVBlankHBlankCallbacksToNull();
    ClearScheduledBgCopiesToVram();
    gMain.state++;
    break;
  case 1:
    ScanlineEffect_Stop();
    gMain.state++;
    break;
  case 2:
    FreeAllSpritePalettes();
    gMain.state++;
    break;
  case 3:
    ResetPaletteFade();
    gMain.state++;
    break;
  case 4:
    ResetSpriteData();
    gMain.state++;
    break;
  case 5:
    ResetSpriteState();
    gMain.state++;
    break;
  case 6:
    ResetTasks();
    gMain.state++;
    break;
  case 7:
    if (InitBackgrounds()) {
      sStateDataPtr->data[0] = 0;
      gMain.state++;
    } else {
      FadeAndBail();
      return TRUE;
    }
    break;
  case 8:
    if (LoadGraphics() == TRUE) {
      gMain.state++;
    }
    break;
  case 9:
    QuestMenu_InitWindows();
    gMain.state++;
    break;
  case 10:
    ClearModeOnStartup();
    InitItems();
    SetCursorPosition();
    SetScrollPosition();
    gMain.state++;
    break;
  case 11:
    if (AllocateResourcesForListMenu()) {
      gMain.state++;
    } else {
      FadeAndBail();
      return TRUE;
    }
    break;
  case 12:
    AllocateMemoryForArray();
    BuildMenuTemplate();
    gMain.state++;
    break;
  case 13:
    GenerateAndPrintHeader();
    gMain.state++;
    break;
  case 14:
    gMain.state++;
    break;
  case 15:
    taskId = CreateTask(Task_Main, 0);
    gTasks[taskId].data[0] = ListMenuInit(
        &gMultiuseListMenuTemplate, sListMenuState.scroll, sListMenuState.row);
    gMain.state++;
    break;
  case 16:
    PlaceTopMenuScrollIndicatorArrows();
    gMain.state++;
    break;
  case 17:
    gMain.state++;
    break;
  case 18:
    BlendPalettes(PALETTES_ALL, 16, 0);
    gMain.state++;
    break;
  case 19:
    BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
    gPaletteFade.bufferTransferDisabled = FALSE;

    if (sListMenuState.initialized != 1)
      SetInitializedFlag(1);
    gMain.state++;
    break;
  default:
    SetVBlankCallback(VBlankCB);
    SetMainCallback2(MainCB);
    return TRUE;
  }
  return FALSE;
}

static bool8 LoadGraphics(void) {
  switch (sStateDataPtr->data[0]) {
  case 0:
    ResetTempTileDataBuffers();
    DecompressAndCopyTileDataToVram(1, sQuestMenuTiles, 0, 0, 0);
    sStateDataPtr->data[0]++;
    break;
  case 1:
    if (FreeTempTileDataBuffersIfPossible() != TRUE) {
      DecompressDataWithHeaderWram(sQuestMenuTilemap, sBg1TilemapBuffer);
      sStateDataPtr->data[0]++;
    }
    break;
  case 2:
    LoadPalette(sQuestMenuBgPals, 0x00, 0x60);
    sStateDataPtr->data[0]++;
    break;
  case 3:
    sStateDataPtr->data[0]++;
    break;
  default:
    sStateDataPtr->data[0] = 0;
    return TRUE;
  }
  return FALSE;
}

static void QuestMenu_InitWindows(void) {
  u8 i;

  InitWindows(sQuestMenuHeaderWindowTemplates);
  DeactivateAllTextPrinters();

  for (i = 0; i < 3; i++) {
    FillWindowPixelBuffer(i, 0x00);
    PutWindowTilemap(i);
  }

  DrawSelectorBox(sStateDataPtr->isSelectorFocused);

  ScheduleBgCopyTilemapToVram(0);
}

static bool8 InitBackgrounds(void) {
  ResetAllBgsCoordinates();
  sBg1TilemapBuffer = Alloc(0x800);
  if (sBg1TilemapBuffer == NULL) {
    return FALSE;
  }

  memset(sBg1TilemapBuffer, 0, 0x800);
  ResetBgsAndClearDma3BusyFlags(0);
  InitBgsFromTemplates(0, sQuestMenuBgTemplates,
                       ARRAY_COUNT(sQuestMenuBgTemplates));
  SetBgTilemapBuffer(1, sBg1TilemapBuffer);
  ScheduleBgCopyTilemapToVram(1);
  SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_1D_MAP | DISPCNT_OBJ_ON);
  SetGpuReg(REG_OFFSET_BLDCNT, 0);
  ShowBg(0);
  ShowBg(1);
  return TRUE;
}

static void InitItems(void) {
  sStateDataPtr->nItems = (CountNumberListRows()) - 1;

  sStateDataPtr->maxShowed =
      sStateDataPtr->nItems + 1 <= 4 ? sStateDataPtr->nItems + 1 : 4;
}

#define try_alloc(ptr__, size)                                                 \
  ({                                                                           \
    void **ptr = (void **)&(ptr__);                                            \
    *ptr = Alloc(size);                                                        \
    if (*ptr == NULL) {                                                        \
      FreeResources();                                                         \
      FadeAndBail();                                                           \
      return FALSE;                                                            \
    }                                                                          \
  })

static bool8 AllocateResourcesForListMenu(void) {
  try_alloc(sListMenuItems,
            sizeof(struct ListMenuItem) * CountNumberListRows() + 1);
  return TRUE;
}

void AllocateMemoryForArray(void) {
  u8 i;
  u8 allocateRows = QUEST_ARRAY_COUNT + 1;

  questNameArray = Alloc(sizeof(void *) * allocateRows);

  for (i = 0; i < allocateRows; i++) {
    questNameArray[i] = Alloc(sizeof(u8) * 48);
  }
}

static void PlaceTopMenuScrollIndicatorArrows(void) {
  u8 listSize = CountNumberListRows();

  if (listSize < sStateDataPtr->maxShowed) {
    listSize = sStateDataPtr->maxShowed;
  }

  sStateDataPtr->scrollIndicatorArrowPairId =
      AddScrollIndicatorArrowPairParameterized(
          2, 94, 8, 90, (listSize - sStateDataPtr->maxShowed), 110, 110,
          &sListMenuState.scroll);
}

static void SetInitializedFlag(u8 a0) { sListMenuState.initialized = a0; }

static u8 GetCursorPosition(void) {
  return sListMenuState.scroll + sListMenuState.row;
}

static void SetCursorPosition(void) {
  if (IfScrollIsOutOfBounds()) {
    sListMenuState.scroll =
        (sStateDataPtr->nItems + 1) - sStateDataPtr->maxShowed;
  }

  if (IfRowIsOutOfBounds()) {
    if (sStateDataPtr->nItems + 1 < 2) {
      sListMenuState.row = 0;
    } else {
      sListMenuState.row = sStateDataPtr->nItems;
    }
  }
}

static void SetScrollPosition(void) {
  u8 i;

  if (sListMenuState.row > 3) {
    for (i = 0; i <= sListMenuState.row - 3;
         sListMenuState.row--, sListMenuState.scroll++, i++) {
      if (sListMenuState.scroll + sStateDataPtr->maxShowed ==
          sStateDataPtr->nItems + 1) {
        break;
      }
    }
  }
}

bool8 IfScrollIsOutOfBounds(void) {
  if (sListMenuState.scroll != 0 &&
      sListMenuState.scroll + sStateDataPtr->maxShowed >
          sStateDataPtr->nItems + 1) {
    return TRUE;
  } else {
    return FALSE;
  }
}

bool8 IfRowIsOutOfBounds(void) {
  if (sListMenuState.scroll + sListMenuState.row >= sStateDataPtr->nItems + 1) {
    return TRUE;
  } else {
    return FALSE;
  }
}

static void SaveScrollAndRow(s16 *data) {
  ListMenuGetScrollAndRow(data[0], &sListMenuState.storedScrollOffset,
                          &sListMenuState.storedRowPosition);
}

void ClearModeOnStartup(void) {
  sStateDataPtr->filterMode = 0;
  sStateDataPtr->selectorMainOrSide = 0;
  sStateDataPtr->isSelectorFocused = FALSE;
}

static bool8 DoesQuestMatchCategory(u8 questId) {
  if (questId >= QUEST_COUNT || sSideQuests[questId].name == NULL)
    return FALSE;
  // Check if quest is visible in current mode before filtering
  if (!IsQuestVisibleInCurrentMode(questId))
    return FALSE;
  if (sStateDataPtr->selectorMainOrSide == 0) // Main
  {
    return !sSideQuests[questId].isSideQuest;
  } else // Side
  {
    return sSideQuests[questId].isSideQuest;
  }
}

static void DrawSelectorBox(bool8 isFocused) {
  // Selector text is now rendered on window 2 via GenerateAndPrintHeader
  return;
}

static u8 ManageMode(u8 action) {
  u8 mode = sStateDataPtr->filterMode;

  switch (action) {
  case SUB:
    mode = ToggleSubquestMode(mode);
    break;

  case ALPHA:
    mode = ToggleAlphaMode(mode);
    sStateDataPtr->restoreCursor = FALSE;
    break;

  case DECREMENT:
    mode = DecrementMode(mode);
    sStateDataPtr->restoreCursor = FALSE;
    break;

  default:
    mode = IncrementMode(mode);
    sStateDataPtr->restoreCursor = FALSE;
    break;
  }
  return mode;
}

u8 ToggleSubquestMode(u8 mode) {
  if (IsSubquestMode()) {
    mode -= SORT_SUBQUEST;
    sStateDataPtr->restoreCursor = TRUE;
  } else {
    mode += SORT_SUBQUEST;
    sStateDataPtr->restoreCursor = FALSE;
  }

  return mode;
}

u8 ToggleAlphaMode(u8 mode) {
  if (IsAlphaMode()) {
    mode -= SORT_DEFAULT_AZ;
  } else {
    mode += SORT_DEFAULT_AZ;
  }

  return mode;
}

u8 IncrementMode(u8 mode) {
  if (mode % 10 == SORT_FAILED) {
    mode -= SORT_FAILED;
  } else {
    mode++;
  }

  return mode;
}

static u8 DecrementMode(u8 mode) {
  if (mode % 10 == SORT_DEFAULT) {
    mode += SORT_FAILED;
  } else {
    mode--;
  }

  return mode;
}

static bool8 IsSubquestMode(void) {
  if (sStateDataPtr->filterMode > SORT_FAILED_AZ) {
    return TRUE;
  } else {
    return FALSE;
  }
}

static bool8 IsNotFilteredMode(void) {
  u8 mode = sStateDataPtr->filterMode % 10;

  if (mode == FLAG_GET_UNLOCKED) {
    return TRUE;
  } else {
    return FALSE;
  }
}

static bool8 IsAlphaMode(void) {
  if (sStateDataPtr->filterMode < SORT_SUBQUEST &&
      sStateDataPtr->filterMode > SORT_FAILED) {
    return TRUE;
  } else {
    return FALSE;
  }
}

static void BuildMenuTemplate(void) {
  u8 lastRow = GetModeAndGenerateList();

  AssignCancelNameAndId(lastRow);

  gMultiuseListMenuTemplate.totalItems = CountNumberListRows();
  gMultiuseListMenuTemplate.items = sListMenuItems;
  gMultiuseListMenuTemplate.windowId = 0;
  gMultiuseListMenuTemplate.header_X = 0;
  gMultiuseListMenuTemplate.cursor_X = 15;
  gMultiuseListMenuTemplate.item_X = 23;
  gMultiuseListMenuTemplate.lettersSpacing = 1;
  gMultiuseListMenuTemplate.itemVerticalPadding = 2;
  gMultiuseListMenuTemplate.upText_Y = 2;
  gMultiuseListMenuTemplate.maxShowed = sStateDataPtr->maxShowed;
  gMultiuseListMenuTemplate.fontId = 2;
  gMultiuseListMenuTemplate.cursorPal = 2;
  gMultiuseListMenuTemplate.fillValue = 0;
  gMultiuseListMenuTemplate.cursorShadowPal = 0;
  gMultiuseListMenuTemplate.moveCursorFunc = MoveCursorFunc;
  gMultiuseListMenuTemplate.itemPrintFunc = GenerateStateAndPrint;
  gMultiuseListMenuTemplate.scrollMultiple = LIST_MULTIPLE_SCROLL_DPAD;
  gMultiuseListMenuTemplate.cursorKind =
      sStateDataPtr->isSelectorFocused ? CURSOR_INVISIBLE : CURSOR_BLACK_ARROW;
}

u8 GetModeAndGenerateList() {
  if (IsSubquestMode()) {
    return GenerateSubquestList();
  } else {
    return GenerateList(!IsNotFilteredMode());
  }
}

static u8 CountMatchingQuests(void) {
  u8 count = 0;
  u8 i;
  u8 mode = sStateDataPtr->filterMode % 10;
  bool8 isFiltered = !IsNotFilteredMode();

  for (i = 0; i < QUEST_COUNT; i++) {
    if (!QuestMenu_GetSetQuestState(i, FLAG_GET_UNLOCKED)) {
      continue;
    }

    if (isFiltered && !QuestMenu_GetSetQuestState(i, mode)) {
      continue;
    }

    if (!DoesQuestMatchCategory(i)) {
      continue;
    }

    count++;
  }
  return count;
}

static u8 CountNumberListRows(void) {
  if (IsSubquestMode()) {
    u8 parentQuest = sStateDataPtr->parentQuest;
    u8 activeTab = sStateDataPtr->filterMode % 10;
    u8 count = 0;
    u8 i;

    for (i = 0; i < sSideQuests[parentQuest].numSubquests; i++) {
      if (activeTab == SORT_REWARD) {
        u16 rewardItem = sSideQuests[parentQuest].subquests[i].rewardItem;
        if (rewardItem == ITEM_NONE)
          continue;
        if (!QuestMenu_GetSetSubquestState(parentQuest, FLAG_GET_COMPLETED, i))
          continue;
        if (QuestMenu_GetSetSubquestState(parentQuest, FLAG_GET_REWARD, i))
          continue;
      }
      count++;
    }
    return count + 1;
  }

  return CountMatchingQuests() + 1;
}

u8 *DefineQuestOrder() {
  static u8 sortedList[QUEST_COUNT];
  u8 a, c, d;
  u8 placeholderVariable;

  for (a = 0; a < QUEST_COUNT; a++) {
    sortedList[a] = a;
  }

  if (IsAlphaMode()) {
    for (c = 0; c < QUEST_COUNT; c++) {
      for (d = c + 1; d < QUEST_COUNT; d++) {
        const u8 *nameC = sSideQuests[sortedList[c]].name ? sSideQuests[sortedList[c]].name : sText_Empty;
        const u8 *nameD = sSideQuests[sortedList[d]].name ? sSideQuests[sortedList[d]].name : sText_Empty;
        if (StringCompare(nameC, nameD) > 0) {
          placeholderVariable = sortedList[c];
          sortedList[c] = sortedList[d];
          sortedList[d] = placeholderVariable;
        }
      }
    }
  }

  return sortedList;
}

u8 GenerateSubquestList(void) {
  u8 parentQuest = sStateDataPtr->parentQuest;
  u8 lastRow = 0, numRow = 0, countQuest = 0;
  u8 activeTab = sStateDataPtr->filterMode % 10;

  for (countQuest = 0; countQuest < sSideQuests[parentQuest].numSubquests;
       countQuest++) {
    if (activeTab == SORT_REWARD) {
      u16 rewardItem =
          sSideQuests[parentQuest].subquests[countQuest].rewardItem;
      if (rewardItem == ITEM_NONE)
        continue;
      if (!QuestMenu_GetSetSubquestState(parentQuest, FLAG_GET_COMPLETED,
                                         countQuest))
        continue;
      if (QuestMenu_GetSetSubquestState(parentQuest, FLAG_GET_REWARD,
                                        countQuest))
        continue;
    }

    PrependQuestNumber(countQuest);
    PopulateSubquestName(parentQuest, countQuest);
    PopulateListRowNameAndId(numRow, countQuest);

    numRow++;
    lastRow = numRow;
  }
  return lastRow;
}

u8 GenerateList(bool8 isFiltered) {
  u8 mode = sStateDataPtr->filterMode % 10;
  u8 numRow = 0, offset = 0, newRow = 0, countQuest = 0, selectedQuestId = 0;
  u8 *sortedQuestList;

  sortedQuestList = DefineQuestOrder();

  for (countQuest = 0; countQuest < QUEST_COUNT; countQuest++) {
    selectedQuestId = *(sortedQuestList + countQuest);

    if (!QuestMenu_GetSetQuestState(selectedQuestId, FLAG_GET_UNLOCKED)) {
      continue;
    }

    if (isFiltered && !QuestMenu_GetSetQuestState(selectedQuestId, mode)) {
      continue;
    }

    if (!DoesQuestMatchCategory(selectedQuestId)) {
      continue;
    }

    PopulateEmptyRow(selectedQuestId);

    if (QuestMenu_GetSetQuestState(selectedQuestId, FLAG_GET_FAVORITE)) {
      SetFavoriteQuest(selectedQuestId);
      newRow = numRow;
      numRow++;
    } else {
      newRow = CountFavoriteQuests() + offset;
      offset++;
    }

    PopulateQuestName(selectedQuestId);
    PopulateListRowNameAndId(newRow, selectedQuestId);
  }
  return numRow + offset;
}

static void AssignCancelNameAndId(u8 numRow) {
  if (IsSubquestMode()) {
    sListMenuItems[numRow].name = sText_Back;
  } else {
    sListMenuItems[numRow].name = sText_Close;
  }

  sListMenuItems[numRow].id = LIST_CANCEL;
}

u8 QuestMenu_GetSetSubquestState(u8 quest, u8 caseId, u8 childQuest) {

  u8 uniqueId = sSideQuests[quest].subquests[childQuest].id;
  u8 index = uniqueId / 8; // 8 bits per byte
  u8 bit = uniqueId % 8;
  u8 mask = 1 << bit;

  switch (caseId) {
  case FLAG_GET_COMPLETED:
    if (sSideQuests[quest].subquests[childQuest].dexRegion > 0) {
      enum NationalDexOrder natNum;
      u8 region = sSideQuests[quest].subquests[childQuest].dexRegion;
      for (natNum = NATIONAL_DEX_START; natNum <= NATIONAL_DEX_COUNT; natNum++) {
        u8 dexRegion = 0;
        if ((s32)natNum <= 151)
          dexRegion = DEX_REGION_KANTO;
        else if (natNum <= 251)
          dexRegion = DEX_REGION_JOHTO;
        else if (natNum <= 386)
          dexRegion = DEX_REGION_HOENN;
        else if (natNum <= 493)
          dexRegion = DEX_REGION_SINNOH;
        else if (natNum <= 649)
          dexRegion = DEX_REGION_UNOVA;
        else if (natNum <= 721)
          dexRegion = DEX_REGION_KALOS;
        else if (natNum <= 809)
          dexRegion = DEX_REGION_ALOLA;
        else if (natNum <= 905)
          dexRegion = DEX_REGION_GALAR;
        else
          dexRegion = DEX_REGION_PALDEA;

        if (dexRegion != region)
          continue;
        if (GetSetPokedexFlag(natNum, FLAG_GET_CAUGHT) == FALSE)
          return FALSE;
      }
      return TRUE;
    }
    if (sSideQuests[quest].subquests[childQuest].spritetype == PKMN) {
      u16 species = sSideQuests[quest].subquests[childQuest].sprite;
      return GetSetPokedexFlag(SpeciesToNationalPokedexNum(species),
                               FLAG_GET_CAUGHT);
    }
    return gSaveBlock2Ptr->subQuests[index] & mask;
  case FLAG_SET_COMPLETED:
    gSaveBlock2Ptr->subQuests[index] |= mask;
    return 1;
  case FLAG_GET_REWARD:
    return gSaveBlock2Ptr->subQuests[index + SUB_FLAGS_COUNT] & mask;
  case FLAG_SET_REWARD:
    gSaveBlock2Ptr->subQuests[index + SUB_FLAGS_COUNT] |= mask;
    return 1;
  }

  return -1;
}

void QuestMenu_SetQuestState(u8 quest, u8 state) {
  u8 i;

  if (quest >= QUEST_COUNT)
    return;

  if (sSideQuests[quest].name == NULL && state != 0)
    return;

  // Clear all 6 bits first
  for (i = 0; i < 6; i++) {
    u8 curIndex = (quest * 6 + i) / 8;
    u8 curBit = (quest * 6 + i) % 8;
    gSaveBlock2Ptr->questData[curIndex] &= ~(1 << curBit);
  }

  // Set the appropriate bits based on state
  switch (state) {
  case 0: // Locked
    // All bits already cleared by loop above
    break;
  case 1: // Unlocked / Inactive
    // Only unlocked bit set
    {
      u8 curIndex = (quest * 6 + 0) / 8;
      u8 curBit = (quest * 6 + 0) % 8;
      gSaveBlock2Ptr->questData[curIndex] |= (1 << curBit);
    }
    break;
  case 2: // Active
    // Unlocked and Active bits set
    {
      u8 curIndex0 = (quest * 6 + 0) / 8;
      u8 curBit0 = (quest * 6 + 0) % 8;
      u8 curIndex1 = (quest * 6 + 1) / 8;
      u8 curBit1 = (quest * 6 + 1) % 8;
      gSaveBlock2Ptr->questData[curIndex0] |= (1 << curBit0);
      gSaveBlock2Ptr->questData[curIndex1] |= (1 << curBit1);
    }
    break;
  case 3: // Completed
    // Unlocked and Completed bits set
    {
      u8 curIndex0 = (quest * 6 + 0) / 8;
      u8 curBit0 = (quest * 6 + 0) % 8;
      u8 curIndex3 = (quest * 6 + 3) / 8;
      u8 curBit3 = (quest * 6 + 3) % 8;
      gSaveBlock2Ptr->questData[curIndex0] |= (1 << curBit0);
      gSaveBlock2Ptr->questData[curIndex3] |= (1 << curBit3);
    }
    break;
  case 4: // Failed
    // Unlocked and Failed bits set
    {
      u8 curIndex0 = (quest * 6 + 0) / 8;
      u8 curBit0 = (quest * 6 + 0) % 8;
      u8 curIndex4 = (quest * 6 + 4) / 8;
      u8 curBit4 = (quest * 6 + 4) % 8;
      gSaveBlock2Ptr->questData[curIndex0] |= (1 << curBit0);
      gSaveBlock2Ptr->questData[curIndex4] |= (1 << curBit4);
    }
    break;
  }
}

u8 QuestMenu_GetSetQuestState(u8 quest, u8 caseId) {
  u8 index;
  u8 bit;
  u8 mask = 0;

  if (quest >= QUEST_COUNT || sSideQuests[quest].name == NULL)
    return FALSE;

  if (IsQuestPostGame(quest) && !FlagGet(FLAG_SYS_GAME_CLEAR))
    return FALSE;

  switch (caseId) {
  case FLAG_GET_ACTIVE:
  case FLAG_GET_REWARD:
  case FLAG_GET_COMPLETED:
  case FLAG_GET_FAILED:
  case FLAG_GET_FAVORITE:
  case FLAG_GET_INACTIVE:
    if (!QuestMenu_GetSetQuestState(quest, FLAG_GET_UNLOCKED))
      return FALSE;
    break;
  }

  index = quest * 6 / 8;
  bit = quest * 6 % 8;

  // 0 : locked
  // 1 : active
  // 2 : reward
  // 3 : completed
  // 4 : failed
  // 5 : favorited

  switch (caseId) {
  case FLAG_GET_UNLOCKED:
  case FLAG_SET_UNLOCKED:
    break;
  case FLAG_GET_INACTIVE:
  case FLAG_GET_ACTIVE:
  case FLAG_SET_ACTIVE:
  case FLAG_REMOVE_ACTIVE:
    bit += 1;
    break;
  case FLAG_GET_REWARD:
  case FLAG_SET_REWARD:
  case FLAG_REMOVE_REWARD:
    bit += 2;
    break;
  case FLAG_GET_COMPLETED:
  case FLAG_SET_COMPLETED:
    bit += 3;
    break;
  case FLAG_GET_FAILED:
  case FLAG_SET_FAILED:
    bit += 4;
    break;
  case FLAG_GET_FAVORITE:
  case FLAG_SET_FAVORITE:
  case FLAG_REMOVE_FAVORITE:
    bit += 5;
    break;
  }
  if (bit >= 8) {
    index += 1;
    bit %= 8;
  }
  mask = 1 << bit;

  switch (caseId) {
  case FLAG_GET_UNLOCKED:
    if (sSideQuests[quest].numSubquests > 0) {
      if (gSaveBlock2Ptr->questData[index] & mask)
        return TRUE;
      {
        u8 i;
        for (i = 0; i < sSideQuests[quest].numSubquests; i++) {
          if (QuestMenu_GetSetSubquestState(quest, FLAG_GET_COMPLETED, i))
            return TRUE;
        }
      }
      return FALSE;
    }
    return gSaveBlock2Ptr->questData[index] & mask;
  case FLAG_SET_UNLOCKED:
    gSaveBlock2Ptr->questData[index] |= mask;
    return 1;
  case FLAG_GET_INACTIVE:
    // Must be unlocked first (bit 0)
    {
      u8 uq = quest * 6 / 8;
      u8 ub = quest * 6 % 8;
      u8 um = 1 << ub;
      if (!(gSaveBlock2Ptr->questData[uq] & um)) {
        return FALSE;
      }
      // Then check bits 1,2,3,4 (active, reward, completed, failed) are all zero
      u8 ab = bit;     // = bit from first switch (quest*6+1)
      u8 rb = bit + 1; // quest*6+2
      u8 cb = bit + 2; // quest*6+3
      u8 fb = bit + 3; // quest*6+4
      u8 ai = index, ri = index, ci = index, fi = index;
      if (ab >= 8) {
        ai += 1;
        ab %= 8;
      }
      if (rb >= 8) {
        ri += 1;
        rb %= 8;
      }
      if (cb >= 8) {
        ci += 1;
        cb %= 8;
      }
      if (fb >= 8) {
        fi += 1;
        fb %= 8;
      }
      if ((gSaveBlock2Ptr->questData[ai] & (1 << ab)) ||
          (gSaveBlock2Ptr->questData[ri] & (1 << rb)) ||
          (gSaveBlock2Ptr->questData[ci] & (1 << cb)) ||
          (gSaveBlock2Ptr->questData[fi] & (1 << fb))) {
        return FALSE;
      }
      return TRUE;
    }
  case FLAG_GET_ACTIVE:
    if (sSideQuests[quest].numSubquests > 0) {
      // Unlocked if explicitly unlocked OR any subquest is completed
      u8 unlockedIndex = (quest * 6 + 0) / 8;
      u8 unlockedBit = (quest * 6 + 0) % 8;
      u8 unlockedMask = 1 << unlockedBit;
      bool8 unlocked =
          (gSaveBlock2Ptr->questData[unlockedIndex] & unlockedMask) != 0;
      u8 i;

      if (!unlocked) {
        for (i = 0; i < sSideQuests[quest].numSubquests; i++) {
          if (QuestMenu_GetSetSubquestState(quest, FLAG_GET_COMPLETED, i)) {
            unlocked = TRUE;
            break;
          }
        }
      }

      if (!unlocked)
        return FALSE;

      if (QuestMenu_GetSetQuestState(quest, FLAG_GET_COMPLETED))
        return FALSE;

      return TRUE;
    }
    return gSaveBlock2Ptr->questData[index] & mask;
  case FLAG_SET_ACTIVE:
    gSaveBlock2Ptr->questData[index] |= mask;
    gSaveBlock2Ptr->questData[quest * 6 / 8] |= (1 << (quest * 6 % 8));
    {
      u8 rewardIndex = (quest * 6 + 2) / 8;
      u8 rewardBit = (quest * 6 + 2) % 8;
      u8 completedIndex = (quest * 6 + 3) / 8;
      u8 completedBit = (quest * 6 + 3) % 8;
      u8 failedIndex = (quest * 6 + 4) / 8;
      u8 failedBit = (quest * 6 + 4) % 8;
      gSaveBlock2Ptr->questData[rewardIndex] &= ~(1 << rewardBit);
      gSaveBlock2Ptr->questData[completedIndex] &= ~(1 << completedBit);
      gSaveBlock2Ptr->questData[failedIndex] &= ~(1 << failedBit);
    }
    return 1;
  case FLAG_REMOVE_ACTIVE:
    gSaveBlock2Ptr->questData[index] &= ~mask;
    return 1;
  case FLAG_GET_REWARD:
    if (sSideQuests[quest].numSubquests > 0) {
      if (sSideQuests[quest].rewardItem != ITEM_NONE) {
        if (QuestMenu_GetSetQuestState(quest, FLAG_GET_COMPLETED))
          return FALSE;
        return gSaveBlock2Ptr->questData[index] & mask;
      } else {
        u8 i;
        for (i = 0; i < sSideQuests[quest].numSubquests; i++) {
          u16 subReward = sSideQuests[quest].subquests[i].rewardItem;
          if (subReward != ITEM_NONE) {
            if (QuestMenu_GetSetSubquestState(quest, FLAG_GET_COMPLETED, i) &&
                !QuestMenu_GetSetSubquestState(quest, FLAG_GET_REWARD, i)) {
              return TRUE;
            }
          }
        }
        return FALSE;
      }
    }
    return gSaveBlock2Ptr->questData[index] & mask;
  case FLAG_SET_REWARD:
    gSaveBlock2Ptr->questData[index] |= mask;
    gSaveBlock2Ptr->questData[quest * 6 / 8] |= (1 << (quest * 6 % 8));
    {
      u8 activeIndex = (quest * 6 + 1) / 8;
      u8 activeBit = (quest * 6 + 1) % 8;
      u8 completedIndex = (quest * 6 + 3) / 8;
      u8 completedBit = (quest * 6 + 3) % 8;
      u8 failedIndex = (quest * 6 + 4) / 8;
      u8 failedBit = (quest * 6 + 4) % 8;
      gSaveBlock2Ptr->questData[activeIndex] &= ~(1 << activeBit);
      gSaveBlock2Ptr->questData[completedIndex] &= ~(1 << completedBit);
      gSaveBlock2Ptr->questData[failedIndex] &= ~(1 << failedBit);
    }
    return 1;
  case FLAG_REMOVE_REWARD:
    gSaveBlock2Ptr->questData[index] &= ~mask;
    return 1;
  case FLAG_GET_COMPLETED:
    if (sSideQuests[quest].dexRegion > 0) {
      enum NationalDexOrder natNum;
      for (natNum = NATIONAL_DEX_START; natNum <= NATIONAL_DEX_COUNT;
           natNum++) {
        u8 dexRegion = 0;
        if ((s32)natNum <= 151)
          dexRegion = DEX_REGION_KANTO;
        else if (natNum <= 251)
          dexRegion = DEX_REGION_JOHTO;
        else if (natNum <= 386)
          dexRegion = DEX_REGION_HOENN;
        else if (natNum <= 493)
          dexRegion = DEX_REGION_SINNOH;
        else if (natNum <= 649)
          dexRegion = DEX_REGION_UNOVA;
        else if (natNum <= 721)
          dexRegion = DEX_REGION_KALOS;
        else if (natNum <= 809)
          dexRegion = DEX_REGION_ALOLA;
        else if (natNum <= 905)
          dexRegion = DEX_REGION_GALAR;
        else
          dexRegion = DEX_REGION_PALDEA;
        if (dexRegion != sSideQuests[quest].dexRegion)
          continue;
        if (GetSetPokedexFlag(natNum, FLAG_GET_CAUGHT) == FALSE)
          return FALSE;
      }
      return TRUE;
    }
    if (sSideQuests[quest].numSubquests > 0) {
      u8 i;
      for (i = 0; i < sSideQuests[quest].numSubquests; i++) {
        if (!QuestMenu_GetSetSubquestState(quest, FLAG_GET_COMPLETED, i))
          return FALSE;
        u16 subReward = sSideQuests[quest].subquests[i].rewardItem;
        if (subReward != ITEM_NONE &&
            !QuestMenu_GetSetSubquestState(quest, FLAG_GET_REWARD, i))
          return FALSE;
      }
      return TRUE;
    }
    return gSaveBlock2Ptr->questData[index] & mask;
  case FLAG_SET_COMPLETED:
    gSaveBlock2Ptr->questData[index] |= mask;
    gSaveBlock2Ptr->questData[quest * 6 / 8] |= (1 << (quest * 6 % 8));
    {
      u8 activeIndex = (quest * 6 + 1) / 8;
      u8 activeBit = (quest * 6 + 1) % 8;
      u8 rewardIndex = (quest * 6 + 2) / 8;
      u8 rewardBit = (quest * 6 + 2) % 8;
      u8 failedIndex = (quest * 6 + 4) / 8;
      u8 failedBit = (quest * 6 + 4) % 8;
      gSaveBlock2Ptr->questData[activeIndex] &= ~(1 << activeBit);
      gSaveBlock2Ptr->questData[rewardIndex] &= ~(1 << rewardBit);
      gSaveBlock2Ptr->questData[failedIndex] &= ~(1 << failedBit);
    }
    return 1;
  case FLAG_GET_FAILED:
    return gSaveBlock2Ptr->questData[index] & mask;
  case FLAG_SET_FAILED:
    gSaveBlock2Ptr->questData[index] |= mask;
    gSaveBlock2Ptr->questData[quest * 6 / 8] |= (1 << (quest * 6 % 8));
    {
      u8 activeIndex = (quest * 6 + 1) / 8;
      u8 activeBit = (quest * 6 + 1) % 8;
      u8 rewardIndex = (quest * 6 + 2) / 8;
      u8 rewardBit = (quest * 6 + 2) % 8;
      u8 completedIndex = (quest * 6 + 3) / 8;
      u8 completedBit = (quest * 6 + 3) % 8;
      gSaveBlock2Ptr->questData[activeIndex] &= ~(1 << activeBit);
      gSaveBlock2Ptr->questData[rewardIndex] &= ~(1 << rewardBit);
      gSaveBlock2Ptr->questData[completedIndex] &= ~(1 << completedBit);
    }
    return 1;
  case FLAG_GET_FAVORITE:
    return gSaveBlock2Ptr->questData[index] & mask;
  case FLAG_SET_FAVORITE:
    gSaveBlock2Ptr->questData[index] |= mask;
    return 1;
  case FLAG_REMOVE_FAVORITE:
    gSaveBlock2Ptr->questData[index] &= ~mask;
    return 1;
  }
  return -1; // failure
}

static u8 CountQuestsInCategory(void) {
  u8 count = 0;
  u8 i;
  for (i = 0; i < QUEST_COUNT; i++) {
    if (QuestMenu_GetSetQuestState(i, FLAG_GET_UNLOCKED)) {
      if (DoesQuestMatchCategory(i)) {
        count++;
      }
    }
  }
  return count;
}

u8 CountUnlockedQuests(void) {
  u8 q = 0, i = 0;

  for (i = 0; i < QUEST_COUNT; i++) {
    if (QuestMenu_GetSetQuestState(i, FLAG_GET_UNLOCKED)) {
      if (DoesQuestMatchCategory(i)) {
        q++;
      }
    }
  }
  return q;
}

u8 CountInactiveQuests(void) {
  u8 q = 0, i = 0;

  for (i = 0; i < QUEST_COUNT; i++) {
    if (QuestMenu_GetSetQuestState(i, FLAG_GET_INACTIVE)) {
      if (DoesQuestMatchCategory(i)) {
        q++;
      }
    }
  }
  return q;
}

u8 CountActiveQuests(void) {
  u8 q = 0, i = 0;

  for (i = 0; i < QUEST_COUNT; i++) {
    if (QuestMenu_GetSetQuestState(i, FLAG_GET_ACTIVE)) {
      if (DoesQuestMatchCategory(i)) {
        q++;
      }
    }
  }
  return q;
}

u8 CountRewardQuests(void) {
  u8 q = 0, i = 0;

  for (i = 0; i < QUEST_COUNT; i++) {
    if (QuestMenu_GetSetQuestState(i, FLAG_GET_REWARD)) {
      if (DoesQuestMatchCategory(i)) {
        q++;
      }
    }
  }
  return q;
}

u8 CountCompletedQuests(void) {
  u8 q = 0, i = 0;

  u8 parentQuest = sStateDataPtr->parentQuest;

  if (IsSubquestMode()) {
    for (i = 0; i < sSideQuests[parentQuest].numSubquests; i++) {
      if (QuestMenu_GetSetSubquestState(parentQuest, FLAG_GET_COMPLETED, i)) {
        q++;
      }
    }
  } else {
    for (i = 0; i < QUEST_COUNT; i++) {
      if (QuestMenu_GetSetQuestState(i, FLAG_GET_COMPLETED)) {
        if (DoesQuestMatchCategory(i)) {
          q++;
        }
      }
    }
  }

  return q;
}

u8 CountFailedQuests(void) {
  u8 q = 0, i = 0;

  for (i = 0; i < QUEST_COUNT; i++) {
    if (QuestMenu_GetSetQuestState(i, FLAG_GET_FAILED)) {
      if (DoesQuestMatchCategory(i)) {
        q++;
      }
    }
  }
  return q;
}

u8 CountFavoriteQuests(void) {
  u8 q = 0, i = 0, x = 0;
  u8 mode = sStateDataPtr->filterMode % 10;

  for (i = 0; i < QUEST_COUNT; i++) {
    if (QuestMenu_GetSetQuestState(i, FLAG_GET_FAVORITE)) {
      if (DoesQuestMatchCategory(i)) {
        if (QuestMenu_GetSetQuestState(i, mode)) {
          x++;
        }
        q++;
      }
    }
  }

  if (IsNotFilteredMode()) {
    return q;
  } else {
    return x;
  }
}

void PopulateEmptyRow(u8 countQuest) {
  questNamePointer = StringCopy(questNameArray[countQuest], sText_Empty);
}
void PrependQuestNumber(u8 countQuest) {
  questNamePointer = ConvertIntToDecimalStringN(
      questNameArray[countQuest], countQuest + 1, STR_CONV_MODE_LEFT_ALIGN, 2);
  questNamePointer = StringAppend(questNamePointer, sText_DotSpace);
}

void SetFavoriteQuest(u8 countQuest) {
  questNamePointer = StringAppend(questNameArray[countQuest], sText_ColorGreen);
}

void PopulateQuestName(u8 countQuest) {
  if (IsQuestHidden(countQuest)) {
    questNamePointer = StringAppend(questNameArray[countQuest], sText_HiddenQuest);
    AddSubQuestButton(countQuest);
  } else if (QuestMenu_GetSetQuestState(countQuest, FLAG_GET_UNLOCKED)) {
    questNamePointer =
        StringAppend(questNameArray[countQuest], sSideQuests[countQuest].name);
    AddSubQuestButton(countQuest);
  } else {
    questNamePointer = StringAppend(questNameArray[countQuest], sText_Unk);
  }
}

void PopulateSubquestName(u8 parentQuest, u8 countQuest) {
  if (IsSubquestCompletedState(countQuest)) {
    const u8 *subName = sSideQuests[parentQuest].subquests[countQuest].name;
    if (subName != NULL) {
      questNamePointer = StringAppend(questNamePointer, subName);
    } else {
      questNamePointer = StringAppend(questNamePointer, sText_Unk);
    }
  } else {
    questNamePointer = StringAppend(questNamePointer, sText_Unk);
  }
}

void PopulateListRowNameAndId(u8 row, u8 countQuest) {
  sListMenuItems[row].name = questNameArray[countQuest];
  sListMenuItems[row].id = countQuest;
}

static bool8 DoesQuestHaveChildrenAndNotInactive(u16 itemId) {
  // For quests with subquests, FLAG_GET_INACTIVE uses raw bit logic and doesn't
  // account for the subquest-derived active state. Use FLAG_GET_UNLOCKED
  // instead, since any unlocked quest with subquests should be enterable.
  if (sSideQuests[itemId].numSubquests != 0 &&
      QuestMenu_GetSetQuestState(itemId, FLAG_GET_UNLOCKED)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

void AddSubQuestButton(u8 countQuest) {
  if (DoesQuestHaveChildrenAndNotInactive(countQuest)) {
    questNamePointer =
        StringAppend(questNameArray[countQuest], sText_SubQuestButton);
  }
}
static void QuestMenu_AddTextPrinterParameterized(u8 windowId, u8 fontId,
                                                  const u8 *str, u8 x, u8 y,
                                                  u8 letterSpacing,
                                                  u8 lineSpacing, u8 speed,
                                                  u8 colorIdx) {
  AddTextPrinterParameterized4(
      windowId, fontId, x, y, letterSpacing, lineSpacing,
      sQuestMenuWindowFontColors[colorIdx], speed, str);
}

static void MoveCursorFunc(s32 questId, bool8 onInit, struct ListMenu *list) {
  PlayCursorSound(onInit);

  if (sStateDataPtr->moveModeOrigPos == 0xFF) {
    QuestMenu_DestroySprite(sStateDataPtr->spriteIconSlot ^ 1);
    sStateDataPtr->spriteIconSlot ^= 1;

    if (questId == LIST_CANCEL) {
      PrintDetailsForCancel();
      QuestMenu_UpdateBottomRightReward(LIST_CANCEL);
    } else {
      GenerateAndPrintQuestDetails(questId);
      DetermineSpriteType(questId);
      QuestMenu_UpdateBottomRightReward(questId);
    }
  }
}

static void PlayCursorSound(bool8 firstRun) {
  if (firstRun == FALSE) {
    PlaySE(SE_SELECT);
  }
}

static void PrintDetailsForCancel() {
  FillWindowPixelBuffer(1, 0);

  QuestMenu_AddTextPrinterParameterized(1, 2, sText_Empty, 2, 3, 2, 0, 0, 0);
  QuestMenu_AddTextPrinterParameterized(1, 2, sText_Empty, 40, 19, 5, 0, 0, 0);
  CopyWindowToVram(1, COPYWIN_GFX);

  QuestMenu_CreateSprite(-1, sStateDataPtr->spriteIconSlot, ITEM);
}

static void PrintDetailsForHidden(void) {
  FillWindowPixelBuffer(1, 0);

  QuestMenu_AddTextPrinterParameterized(1, 2, sText_Empty, 2, 3, 2, 0, 0, 0);
  QuestMenu_AddTextPrinterParameterized(1, 2, sText_Empty, 40, 19, 5, 0, 0, 0);
  CopyWindowToVram(1, COPYWIN_GFX);
}

void GenerateAndPrintQuestDetails(s32 questId) {
  if (!IsSubquestMode()) {
    if (!QuestMenu_GetSetQuestState(questId, FLAG_GET_UNLOCKED)) {
      PrintDetailsForHidden();
      return;
    }
  } else {
    if (!IsSubquestCompletedState(questId)) {
      PrintDetailsForHidden();
      return;
    }
  }
  GenerateQuestLocation(questId);
  PrintQuestLocation(questId);
  GenerateQuestFlavorText(questId);
  PrintQuestFlavorText(questId);
}
void GenerateQuestLocation(s32 questId) {
  if (!IsSubquestMode()) {
    if (IsQuestHidden(questId)) {
      StringCopy(gStringVar2, sText_HiddenLocation);
    } else if (IsQuestInactiveState(questId) && !IsQuestRewardState(questId) &&
        !IsQuestCompletedState(questId)) {
      StringCopy(gStringVar2, sText_Empty);
    } else {
      StringCopy(gStringVar2, GetQuestLocation(questId));
    }
  } else {
    const u8 *subMap = sSideQuests[sStateDataPtr->parentQuest].subquests[questId].map;
    if (subMap != NULL) {
      StringCopy(gStringVar2, subMap);
    } else {
      StringCopy(gStringVar2, sText_Empty);
    }
  }
}
void PrintQuestLocation(s32 questId) {
  const u8 *title;
  u32 locationX;

  FillWindowPixelBuffer(1, 0);

  if (!IsSubquestMode()) {
    if (IsQuestHidden(questId)) {
      title = sText_HiddenTitle;
    } else {
      title = sSideQuests[questId].title;
      if (title == NULL) {
        title = sSideQuests[questId].name;
      }
    }
  } else {
    title = sSideQuests[sStateDataPtr->parentQuest].subquests[questId].title;
    if (title == NULL) {
      title = sSideQuests[sStateDataPtr->parentQuest].subquests[questId].name;
    }
  }
  
  if (title == NULL) {
    title = sText_Empty;
  }
  
  QuestMenu_AddTextPrinterParameterized(1, 2, title, 2, 3, 2, 0, 0, 4);

  locationX = 234 - GetStringWidth(2, gStringVar2, 0);
  QuestMenu_AddTextPrinterParameterized(1, 2, gStringVar2, locationX, 3, 2, 0, 0, 4);
}
void GenerateQuestFlavorText(s32 questId) {
  // Undiscovered quest - show hidden message
  if (!IsQuestDiscovered(questId) && IsQuestHidden(questId)) {
    StringCopy(gStringVar1, sText_HiddenDesc);
    StringExpandPlaceholders(gStringVar3, gStringVar1);
    return;
  }
  if (IsSubquestMode() == FALSE) {
    if (IsQuestInactiveState(questId) == TRUE) {
      StringCopy(gStringVar1, sText_StartForMore);
    }
    if (IsQuestActiveState(questId) == TRUE) {
      UpdateQuestFlavorText(questId);
    }
    if (IsQuestRewardState(questId) == TRUE) {
      if (sSideQuests[questId].rewarddesc != NULL) {
        StringCopy(gStringVar1, sSideQuests[questId].rewarddesc);
      } else {
        StringCopy(gStringVar1, sText_ReturnRecieveReward);
      }
    }
    if (IsQuestCompletedState(questId) == TRUE) {
      if (sSideQuests[questId].donedesc != NULL) {
        StringCopy(gStringVar1, sSideQuests[questId].donedesc);
      } else {
        StringCopy(gStringVar1, sText_Empty);
      }
    }
    if (IsQuestFailedState(questId) == TRUE) {
      if (sSideQuests[questId].faileddesc != NULL) {
        StringCopy(gStringVar1, sSideQuests[questId].faileddesc);
      } else {
        StringCopy(gStringVar1, sText_Empty);
      }
    }
  } else {
    if (IsSubquestCompletedState(questId) == TRUE) {
      const u8 *subDesc = sSideQuests[sStateDataPtr->parentQuest].subquests[questId].desc;
      if (subDesc != NULL) {
        StringCopy(gStringVar1, subDesc);
      } else {
        StringCopy(gStringVar1, sText_Empty);
      }
    } else {
      StringCopy(gStringVar1, sText_Unk);
    }
  }

  // Strip "Claim your..." text if present
  {
    static const u8 sText_ClaimYour[] = _("Claim your");
    u16 i, j;
    u16 len = StringLength(sText_ClaimYour);
    u16 destLen = StringLength(gStringVar1);
    if (destLen >= len) {
      for (i = 0; i <= destLen - len; i++) {
        for (j = 0; j < len; j++) {
          if (gStringVar1[i + j] != sText_ClaimYour[j])
            break;
        }
        if (j == len) {
          // Found it! strip starting at i.
          // If the character before is a newline (0xFE) or a prompt (0xFA), strip that too.
          if (i > 0 && (gStringVar1[i - 1] == 0xFE || gStringVar1[i - 1] == 0xFA)) {
            gStringVar1[i - 1] = EOS;
          } else {
            gStringVar1[i] = EOS;
          }
          break;
        }
      }
    }
  }

  StringExpandPlaceholders(gStringVar3, gStringVar1);
}
void UpdateQuestFlavorText(s32 questId) {
  StringExpandPlaceholders(gStringVar1, GetQuestDesc(questId));
}
void PrintQuestFlavorText(s32 questId) {
  QuestMenu_AddTextPrinterParameterized(1, 2, gStringVar3, 40, 19, 5, 0, 0, 4);
}

static const u8 *GetQuestLocation(s32 questId) {
  if (questId >= QUEST_COUNT || sSideQuests[questId].name == NULL || IsQuestHidden(questId))
    return sText_HiddenLocation;

  u32 qvar = VarGet(sSideQuests[questId].questVariable);

  if (qvar >= MAX_QUEST_STATES || sSideQuests[questId].map[qvar] == NULL)
    qvar = 0;

  if (sSideQuests[questId].map[qvar] == NULL)
    return sText_Empty;

  return sSideQuests[questId].map[qvar];
}

static const u8 *GetQuestDesc(s32 questId) {
  if (questId >= QUEST_COUNT || sSideQuests[questId].name == NULL || IsQuestHidden(questId))
    return sText_HiddenDesc;

  u32 qvar = VarGet(sSideQuests[questId].questVariable);

  if (qvar >= MAX_QUEST_STATES || sSideQuests[questId].desc[qvar] == NULL)
    qvar = 0;

  if (sSideQuests[questId].desc[qvar] == NULL)
    return sText_Empty;

  return sSideQuests[questId].desc[qvar];
}

bool8 IsSubquestCompletedState(s32 questId) {
  if (QuestMenu_GetSetSubquestState(sStateDataPtr->parentQuest,
                                    FLAG_GET_COMPLETED, questId)) {
    return TRUE;
  } else {
    return FALSE;
  }
}
bool8 IsQuestRewardState(s32 questId) {
  if (QuestMenu_GetSetQuestState(questId, FLAG_GET_REWARD)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

bool8 IsQuestFailedState(s32 questId) {
  if (QuestMenu_GetSetQuestState(questId, FLAG_GET_FAILED)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

bool8 IsQuestInactiveState(s32 questId) {
  if (!QuestMenu_GetSetQuestState(questId, FLAG_GET_ACTIVE) &&
      !QuestMenu_GetSetQuestState(questId, FLAG_GET_FAILED)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

bool8 IsQuestActiveState(s32 questId) {
  if (QuestMenu_GetSetQuestState(questId, FLAG_GET_ACTIVE)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

bool8 IsQuestCompletedState(s32 questId) {
  if (QuestMenu_GetSetQuestState(questId, FLAG_GET_COMPLETED)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

bool8 UNUSED IsQuestUnlocked(s32 questId) {
  if (QuestMenu_GetSetQuestState(questId, FLAG_GET_UNLOCKED)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

void DetermineSpriteType(s32 questId) {
  u16 spriteId;
  u8 spriteType;

  if (IsSubquestMode() == FALSE) {
    if (!QuestMenu_GetSetQuestState(questId, FLAG_GET_UNLOCKED) || IsQuestHidden(questId)) {
      QuestMenu_CreateSprite(ITEM_NONE, sStateDataPtr->spriteIconSlot, ITEM);
    } else {
      spriteId = GetQuestSprite(questId);
      spriteType = GetQuestSpriteType(questId);
      QuestMenu_CreateSprite(spriteId, sStateDataPtr->spriteIconSlot, spriteType);
    }
  } else if (IsSubquestCompletedState(questId) == TRUE) {
    spriteId =
        sSideQuests[sStateDataPtr->parentQuest].subquests[questId].sprite;
    spriteType =
        sSideQuests[sStateDataPtr->parentQuest].subquests[questId].spritetype;
    QuestMenu_CreateSprite(spriteId, sStateDataPtr->spriteIconSlot, spriteType);
  } else {
    QuestMenu_CreateSprite(ITEM_NONE, sStateDataPtr->spriteIconSlot, ITEM);
  }
  QuestMenu_DestroySprite(sStateDataPtr->spriteIconSlot ^ 1);
  sStateDataPtr->spriteIconSlot ^= 1;
}

static const u32 sQuestBadgeGfx[] =
    INCBIN_U32("graphics/trainer_card/badges.4bpp.smol");
static const u16 sQuestBadgePal[] =
    INCBIN_U16("graphics/trainer_card/badges.gbapal");

static const struct OamData sOamData_QuestBadge = {.affineMode =
                                                       ST_OAM_AFFINE_OFF,
                                                   .shape = SPRITE_SHAPE(16x16),
                                                   .size = SPRITE_SIZE(16x16),
                                                   .priority = 1,
                                                   .paletteNum = 0};

static const union AnimCmd sAnim_QuestBadge[] = {ANIMCMD_FRAME(0, 0),
                                                 ANIMCMD_END};

static const union AnimCmd *const sAnims_QuestBadge[] = {sAnim_QuestBadge};

static const struct SpriteTemplate sQuestBadgeSpriteTemplate = {
    .tileTag = 102,
    .paletteTag = 102,
    .oam = &sOamData_QuestBadge,
    .anims = sAnims_QuestBadge,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy};

static void LoadBadgeSprite(u16 badgeId, u8 idx) {
  struct SpriteSheet spriteSheet;
  struct SpritePalette spritePalette;
  struct SpriteTemplate template;
  u8 *decompressed = Alloc(0x400); // 1024 bytes
  u8 badgeTiles[128];              // 4 tiles * 32 bytes = 128 bytes
  u8 spriteId;

  DecompressDataWithHeaderWram(sQuestBadgeGfx, decompressed);

  memcpy(badgeTiles, decompressed + 32 * (2 * badgeId), 32);
  memcpy(badgeTiles + 32, decompressed + 32 * (2 * badgeId + 1), 32);
  memcpy(badgeTiles + 64, decompressed + 32 * (2 * badgeId + 16), 32);
  memcpy(badgeTiles + 96, decompressed + 32 * (2 * badgeId + 17), 32);

  Free(decompressed);

  spriteSheet.data = badgeTiles;
  spriteSheet.size = 128;
  spriteSheet.tag = 102 + idx;
  LoadSpriteSheet(&spriteSheet);

  spritePalette.data = sQuestBadgePal;
  spritePalette.tag = 102 + idx;
  LoadSpritePalette(&spritePalette);

  CpuCopy16(&sQuestBadgeSpriteTemplate, &template,
            sizeof(struct SpriteTemplate));
  template.tileTag = 102 + idx;
  template.paletteTag = 102 + idx;
  spriteId = CreateSprite(&template, 0, 0, 0);

  if (spriteId != MAX_SPRITES) {
    sItemMenuIconSpriteIds[10 + idx] = spriteId;
    gSprites[spriteId].x2 = 20;
    gSprites[spriteId].y2 = 136;
    gSprites[spriteId].oam.objMode = ST_OAM_OBJ_BLEND;
  }
}

static const union AffineAnimCmd sAffineAnim_Reward_NoAnim[] = {
    AFFINEANIMCMD_FRAME(0, 0, 0, 60),
    AFFINEANIMCMD_END,
};

static const union AffineAnimCmd sAffineAnim_Reward_Anim[] = {
    AFFINEANIMCMD_FRAME(20, 20, 0, 5),    // Scale big
    AFFINEANIMCMD_FRAME(-10, -10, 0, 10), // Scale smol
    AFFINEANIMCMD_FRAME(0, 0, 1, 4),      // Begin rotating

    AFFINEANIMCMD_FRAME(0, 0, -1,
                        4), // Loop starts from here ; Rotate/Tilt left
    AFFINEANIMCMD_FRAME(0, 0, 0, 2),
    AFFINEANIMCMD_FRAME(0, 0, -1, 4),
    AFFINEANIMCMD_FRAME(0, 0, 0, 2),
    AFFINEANIMCMD_FRAME(0, 0, -1, 4),

    AFFINEANIMCMD_FRAME(0, 0, 1, 4), // Rotate/Tilt Right
    AFFINEANIMCMD_FRAME(0, 0, 0, 2),
    AFFINEANIMCMD_FRAME(0, 0, 1, 4),
    AFFINEANIMCMD_FRAME(0, 0, 0, 2),
    AFFINEANIMCMD_FRAME(0, 0, 1, 4),

    AFFINEANIMCMD_JUMP(3),
};

static const union AffineAnimCmd *const sAffineAnims_Reward[] = {
    sAffineAnim_Reward_NoAnim,
    sAffineAnim_Reward_Anim,
};

static const struct OamData sOamData_QuestReward = {
    .affineMode = ST_OAM_AFFINE_DOUBLE,
    .shape = SPRITE_SHAPE(32x32),
    .size = SPRITE_SIZE(32x32),
    .priority = 1,
    .paletteNum = 0};

static const struct SpriteTemplate sQuestRewardSpriteTemplate = {
    .tileTag = 102,
    .paletteTag = 102,
    .oam = &sOamData_QuestReward,
    .anims = sAnims_QuestBadge,
    .images = NULL,
    .affineAnims = sAffineAnims_Reward,
    .callback = SpriteCallbackDummy};

static void QuestMenu_CreateRewardSprite(u16 itemId, u8 idx) {
  struct SpriteTemplate template;
  struct SpriteSheet spriteSheet;
  struct SpritePalette spritePalette;
  u8 spriteId;
  u8 *decompressed;
  u8 *iconBuffer;

  FreeSpriteTilesByTag(102 + idx);
  FreeSpritePaletteByTag(102 + idx);

  decompressed = Alloc(0x200);
  if (decompressed == NULL)
    return;

  iconBuffer = AllocZeroed(0x200);
  if (iconBuffer == NULL) {
    Free(decompressed);
    return;
  }

  DecompressDataWithHeaderWram(GetItemIconPic(itemId), decompressed);
  if (GetDecompressedDataSize(GetItemIconPic(itemId)) >= 0x200)
    CpuCopy16(decompressed, iconBuffer, 0x200);
  else
    CopyItemIconPicTo4x4Buffer(decompressed, iconBuffer);

  spriteSheet.data = iconBuffer;
  spriteSheet.size = 0x200;
  spriteSheet.tag = 102 + idx;
  LoadSpriteSheet(&spriteSheet);

  spritePalette.data = GetItemIconPalette(itemId);
  spritePalette.tag = 102 + idx;
  LoadSpritePalette(&spritePalette);

  Free(decompressed);
  Free(iconBuffer);

  CpuCopy16(&sQuestRewardSpriteTemplate, &template,
            sizeof(struct SpriteTemplate));
  template.tileTag = 102 + idx;
  template.paletteTag = 102 + idx;
  spriteId = CreateSprite(&template, 0, 0, 0);

  if (spriteId != MAX_SPRITES) {
    sItemMenuIconSpriteIds[10 + idx] = spriteId;
    gSprites[spriteId].x2 = 24;
    gSprites[spriteId].y2 = 140;
    gSprites[spriteId].oam.objMode = ST_OAM_OBJ_BLEND;
    StartSpriteAffineAnim(&gSprites[spriteId], 1);
  }
}

static const struct OamData sOamData_BottomRight = {
    .affineMode = ST_OAM_AFFINE_OFF,
    .shape = SPRITE_SHAPE(32x32),
    .size = SPRITE_SIZE(32x32),
    .priority = 1,
    .paletteNum = 0};

static void QuestMenu_CreateBottomRightRewardSprite(u16 itemId) {
  struct SpriteTemplate template;
  struct SpriteSheet spriteSheet;
  struct SpritePalette spritePalette;
  u8 spriteId;
  u8 *decompressed;
  u8 *iconBuffer;

  if (sItemMenuIconSpriteIds[8] != 0xFF) {
    return;
  }

  FreeSpriteTilesByTag(104);
  FreeSpritePaletteByTag(104);

  decompressed = Alloc(0x200);
  if (decompressed == NULL)
    return;

  iconBuffer = AllocZeroed(0x200);
  if (iconBuffer == NULL) {
    Free(decompressed);
    return;
  }

  DecompressDataWithHeaderWram(GetItemIconPic(itemId), decompressed);
  if (GetDecompressedDataSize(GetItemIconPic(itemId)) >= 0x200)
    CpuCopy16(decompressed, iconBuffer, 0x200);
  else
    CopyItemIconPicTo4x4Buffer(decompressed, iconBuffer);

  spriteSheet.data = iconBuffer;
  spriteSheet.size = 0x200;
  spriteSheet.tag = 104;
  LoadSpriteSheet(&spriteSheet);

  spritePalette.data = GetItemIconPalette(itemId);
  spritePalette.tag = 104;
  LoadSpritePalette(&spritePalette);

  Free(decompressed);
  Free(iconBuffer);

  CpuCopy16(&sQuestRewardSpriteTemplate, &template,
            sizeof(struct SpriteTemplate));
  template.tileTag = 104;
  template.paletteTag = 104;
  template.oam = &sOamData_BottomRight;
  template.affineAnims = gDummySpriteAffineAnimTable;
  spriteId = CreateSprite(&template, 230, 152, 0);

  if (spriteId != MAX_SPRITES) {
    sItemMenuIconSpriteIds[8] = spriteId;
    gSprites[spriteId].oam.objMode = ST_OAM_OBJ_BLEND;
  }
}

static void QuestMenu_DestroyBottomRightSprite(void) {
  if (sItemMenuIconSpriteIds[8] != 0xFF) {
    DestroySprite(&gSprites[sItemMenuIconSpriteIds[8]]);
    sItemMenuIconSpriteIds[8] = 0xFF;
  }
  FreeSpriteTilesByTag(104);
  FreeSpritePaletteByTag(104);
}

static void QuestMenu_DrawBottomRightBox(bool8 draw) {
  u8 r, c;
  u16 *tilemap = (u16 *)sBg1TilemapBuffer;

  if (tilemap == NULL)
    return;

  for (r = 0; r < 4; r++) {
    for (c = 0; c < 4; c++) {
      u16 destCol = 26 + c;
      u16 destRow = 16 + r;
      if (draw) {
        tilemap[destRow * 32 + destCol] = tilemap[(14 + r) * 32 + c];
      } else {
        tilemap[destRow * 32 + destCol] = tilemap[destRow * 32 + 10 + c];
      }
    }
  }
  ScheduleBgCopyTilemapToVram(1);
}

static void QuestMenu_UpdateBottomRightReward(s32 questId) {
  bool8 hasReward = FALSE;
  u16 rewardItem = ITEM_NONE;

  if (questId != LIST_CANCEL) {
    if (IsSubquestMode()) {
      u8 parentQuest = sStateDataPtr->parentQuest;
      rewardItem = sSideQuests[parentQuest].subquests[questId].rewardItem;
      if (rewardItem != ITEM_NONE) {
        if (QuestMenu_GetSetSubquestState(parentQuest, FLAG_GET_COMPLETED,
                                          questId) &&
            QuestMenu_GetSetSubquestState(parentQuest, FLAG_GET_REWARD,
                                          questId)) {
          hasReward = TRUE;
        }
      }
    } else {
      rewardItem = sSideQuests[questId].rewardItem;
      if (rewardItem != ITEM_NONE) {
        if (QuestMenu_GetSetQuestState(questId, FLAG_GET_COMPLETED)) {
          hasReward = TRUE;
        }
      }
    }
  }

  if (hasReward) {
    QuestMenu_DrawBottomRightBox(TRUE);
    QuestMenu_CreateBottomRightRewardSprite(rewardItem);
  } else {
    QuestMenu_DrawBottomRightBox(FALSE);
    QuestMenu_DestroyBottomRightSprite();
  }
}

static void QuestMenu_CreateSprite(u16 itemId, u8 idx, u8 spriteType) {
  u8 *ptr = &sItemMenuIconSpriteIds[10];
  u8 spriteId = 0xFF;

  if (ptr[idx] == 0xFF) {
    FreeSpriteTilesByTag(102 + idx);
    FreeSpritePaletteByTag(102 + idx);

    switch (spriteType) {
    case OBJECT:
      spriteId =
          CreateObjectGraphicsSprite(itemId, SpriteCallbackDummy, 20, 132, 0);
      break;
    case ITEM:
      spriteId = AddItemIconSprite(102 + idx, 102 + idx, itemId);
      break;
    case PKMN:
      LoadMonIconPalettes();
      spriteId = CreateMonIcon(itemId, SpriteCallbackDummy, 20, 132, 0, 1);
      break;
    case BADGE:
      LoadBadgeSprite(itemId, idx);
      return;
    default:
      spriteId = SPRITE_NONE;
      break;
    }

    if (spriteId != SPRITE_NONE)
      gSprites[spriteId].oam.objMode = ST_OAM_OBJ_BLEND;

    if (spriteId != MAX_SPRITES) {
      ptr[idx] = spriteId;

      if (spriteType == ITEM) {
        gSprites[spriteId].x2 = 24;
        gSprites[spriteId].y2 = 140;
      }
    }
  }
}

void ResetSpriteState(void) {
  u16 i;

  for (i = 0; i < ARRAY_COUNT(sItemMenuIconSpriteIds); i++) {
    sItemMenuIconSpriteIds[i] = 0xFF;
  }
}

static void QuestMenu_DestroySprite(u8 idx) {
  u8 *ptr = &sItemMenuIconSpriteIds[10];

  if (ptr[idx] != 0xFF) {
    struct Sprite *sprite = &gSprites[ptr[idx]];
    if (!sprite->usingSheet) {
      FreeAndDestroyMonIconSprite(sprite);
    } else {
      DestroySprite(sprite);
    }
    ptr[idx] = 0xFF;
  }
  FreeSpriteTilesByTag(102 + idx);
  FreeSpritePaletteByTag(102 + idx);
}

static u32 GetQuestSprite(s32 questId) {
  u32 qvar = VarGet(sSideQuests[questId].questVariable);

  if (qvar >= MAX_QUEST_STATES || sSideQuests[questId].sprite[qvar] == 0)
    qvar = 0;

  return sSideQuests[questId].sprite[qvar];
}

static u32 GetQuestSpriteType(s32 questId) {
  u32 qvar = VarGet(sSideQuests[questId].questVariable);

  if (qvar >= MAX_QUEST_STATES || sSideQuests[questId].spritetype[qvar] == 0)
    qvar = 0;

  return sSideQuests[questId].spritetype[qvar];
}

static void GenerateStateAndPrint(u8 windowId, u32 questId, u8 y) {
  u8 colorIndex;

  if (questId != LIST_CANCEL) {
    if (IsSubquestMode()) {
      colorIndex = GenerateSubquestState(questId);
    } else {
      colorIndex = GenerateQuestState(questId);
    }

    PrintQuestState(windowId, y, colorIndex);
  }
}

u8 GenerateSubquestState(u8 questId) {
  u8 parentQuest = sStateDataPtr->parentQuest;

  if (QuestMenu_GetSetSubquestState(parentQuest, FLAG_GET_COMPLETED, questId)) {
    u16 rewardItem = sSideQuests[parentQuest].subquests[questId].rewardItem;
    if (rewardItem != ITEM_NONE &&
        !QuestMenu_GetSetSubquestState(parentQuest, FLAG_GET_REWARD, questId)) {
      StringCopy(gStringVar4, sText_Reward);
      return 1;
    }

    if (parentQuest == QUEST_GYM_CHALLENGES) {
      StringCopy(gStringVar4, sText_Beat);
    } else if (sSideQuests[parentQuest].subquests[questId].spritetype == PKMN) {
      StringCopy(gStringVar4, sText_Caught);
    } else {
      StringCopy(gStringVar4, sText_Beat);
    }
  } else {
    StringCopy(gStringVar4, sText_Empty);
  }

  return 2;
}

u8 GenerateQuestState(u8 questId) {
  if (IsQuestHidden(questId)) {
    StringCopy(gStringVar4, sText_Empty);
    return 0;
  }

  if (QuestMenu_GetSetQuestState(questId, FLAG_GET_COMPLETED)) {
    StringCopy(gStringVar4, sText_Complete);
    return 2;
  } else if (QuestMenu_GetSetQuestState(questId, FLAG_GET_REWARD)) {
    StringCopy(gStringVar4, sText_Reward);
    return 1;
  } else if (QuestMenu_GetSetQuestState(questId, FLAG_GET_ACTIVE)) {
    StringCopy(gStringVar4, sText_Active);
    return 3;
  } else if (QuestMenu_GetSetQuestState(questId, FLAG_GET_FAILED)) {
    StringCopy(gStringVar4, sText_Failed);
    return 5;
  } else {
    StringCopy(gStringVar4, sText_Empty);
  }

  return 0;
}

void PrintQuestState(u8 windowId, u8 y, u8 colorIndex) {
  QuestMenu_AddTextPrinterParameterized(windowId, 0, gStringVar4, 200, y, 0, 0,
                                        0xFF, colorIndex);
}

static void GenerateAndPrintHeader(void) {
  GenerateDenominatorNumQuests();
  GenerateNumeratorNumQuests();
  GenerateMenuContext();

  PrintNumQuests();
  PrintMenuContext();

  if (!IsSubquestMode()) {
    PrintTypeFilterButton();

    if (sStateDataPtr->isSelectorFocused) {
      if (sStateDataPtr->selectorMainOrSide == 0)
        QuestMenu_AddTextPrinterParameterized(2, 0, sText_MainSelectorFocused,
                                              28, 1, 0, 1, 0, 0);
      else
        QuestMenu_AddTextPrinterParameterized(2, 0, sText_SideSelectorFocused,
                                              28, 1, 0, 1, 0, 0);
    } else {
      if (sStateDataPtr->selectorMainOrSide == 0)
        QuestMenu_AddTextPrinterParameterized(2, 0, sText_MainSelector, 28, 1,
                                              0, 1, 0, 0);
      else
        QuestMenu_AddTextPrinterParameterized(2, 0, sText_SideSelector, 28, 1,
                                              0, 1, 0, 0);
    }
  }
}
static void GenerateDenominatorNumQuests(void) {
  ConvertIntToDecimalStringN(gStringVar2, CountQuestsInCategory(),
                             STR_CONV_MODE_LEFT_ALIGN, 6);
}

static void GenerateNumeratorNumQuests(void) {
  u8 mode = sStateDataPtr->filterMode % 10;
  u8 parentQuest = sStateDataPtr->parentQuest;

  switch (mode) {
  case SORT_DEFAULT:
    ConvertIntToDecimalStringN(gStringVar1, CountUnlockedQuests(),
                               STR_CONV_MODE_LEFT_ALIGN, 6);
    break;
  case SORT_INACTIVE:
    ConvertIntToDecimalStringN(gStringVar1, CountInactiveQuests(),
                               STR_CONV_MODE_LEFT_ALIGN, 6);
    break;
  case SORT_ACTIVE:
    ConvertIntToDecimalStringN(gStringVar1, CountActiveQuests(),
                               STR_CONV_MODE_LEFT_ALIGN, 6);
    break;
  case SORT_REWARD:
    ConvertIntToDecimalStringN(gStringVar1, CountRewardQuests(),
                               STR_CONV_MODE_LEFT_ALIGN, 6);
    break;
  case SORT_DONE:
    ConvertIntToDecimalStringN(gStringVar1, CountCompletedQuests(),
                               STR_CONV_MODE_LEFT_ALIGN, 6);
    break;
  case SORT_FAILED:
    ConvertIntToDecimalStringN(gStringVar1, CountFailedQuests(),
                               STR_CONV_MODE_LEFT_ALIGN, 6);
    break;
  }

  if (IsSubquestMode()) {
    ConvertIntToDecimalStringN(gStringVar2,
                               sSideQuests[parentQuest].numSubquests,
                               STR_CONV_MODE_LEFT_ALIGN, 6);
    ConvertIntToDecimalStringN(gStringVar1, CountCompletedQuests(),
                               STR_CONV_MODE_LEFT_ALIGN, 6);
  }
}

static void GenerateMenuContext(void) {
  u8 mode = sStateDataPtr->filterMode % 10;
  u8 parentQuest = sStateDataPtr->parentQuest;

  switch (mode) {
  case SORT_DEFAULT:
    questNamePointer =
        StringCopy(questNameArray[QUEST_ARRAY_COUNT], sText_AllHeader);
    break;
  case SORT_INACTIVE:
    questNamePointer =
        StringCopy(questNameArray[QUEST_ARRAY_COUNT], sText_InactiveHeader);
    break;
  case SORT_ACTIVE:
    questNamePointer =
        StringCopy(questNameArray[QUEST_ARRAY_COUNT], sText_ActiveHeader);
    break;
  case SORT_REWARD:
    questNamePointer =
        StringCopy(questNameArray[QUEST_ARRAY_COUNT], sText_RewardHeader);
    break;
  case SORT_DONE:
    questNamePointer =
        StringCopy(questNameArray[QUEST_ARRAY_COUNT], sText_CompletedHeader);
    break;
  case SORT_FAILED:
    questNamePointer =
        StringCopy(questNameArray[QUEST_ARRAY_COUNT], sText_FailedHeader);
    break;
  }

  if (IsAlphaMode()) {
    questNamePointer =
        StringAppend(questNameArray[QUEST_ARRAY_COUNT], sText_AZ);
  }
  if (IsSubquestMode()) {
    questNamePointer = StringCopy(questNameArray[QUEST_ARRAY_COUNT],
                                  sSideQuests[parentQuest].name);
  }
}

static void PrintNumQuests(void) {
  StringExpandPlaceholders(gStringVar4, sText_QuestNumberDisplay);
  QuestMenu_AddTextPrinterParameterized(2, 0, gStringVar4, 167, 1, 0, 1, 0, 0);
}
static void PrintMenuContext(void) {
  if (IsSubquestMode()) {
    QuestMenu_AddTextPrinterParameterized(2, 0, questNameArray[QUEST_ARRAY_COUNT],
                                          10, 1, 0, 1, 0, 0);
  } else {
    QuestMenu_AddTextPrinterParameterized(2, 0, questNameArray[QUEST_ARRAY_COUNT],
                                          90, 1, 0, 1, 0, 0);
  }
}
static void PrintTypeFilterButton(void) {
  QuestMenu_AddTextPrinterParameterized(2, 0, sText_LType, 6, 1, 0, 1, 0, 0);
  QuestMenu_AddTextPrinterParameterized(2, 0, sText_TypeR, 200, 1, 0, 1, 0, 0);
}

static void Task_Main(u8 taskId) {
  s16 *data = gTasks[taskId].data;

  if (!gPaletteFade.active) {
    if (sStateDataPtr->isSelectorFocused) {
      if (JOY_NEW(DPAD_LEFT) || JOY_NEW(DPAD_RIGHT)) {
        sStateDataPtr->selectorMainOrSide ^= 1;
        PlaySE(SE_SELECT);
        Task_QuestMenuCleanUp(taskId);
      } else if (JOY_NEW(DPAD_DOWN)) {
        sStateDataPtr->isSelectorFocused = FALSE;
        PlaySE(SE_SELECT);
        sListMenuState.scroll = 0;
        sListMenuState.row = 0;
        Task_QuestMenuCleanUp(taskId);
      } else if (JOY_NEW(B_BUTTON)) {
        if (IsSubquestMode()) {
          ReturnFromSubquestAndCleanUp(taskId);
        } else {
          TurnOffQuestMenu(taskId);
        }
      }
    } else {
      u16 prevScroll = sListMenuState.scroll;
      u16 prevRow = sListMenuState.row;

      // Check UP-at-top BEFORE processing list input so the list never wraps
      // to the bottom before the selector focus kicks in.
      // Use JOY_REPEAT to catch both new presses AND held auto-repeat,
      // since ListMenu_ProcessInput uses repeat internally.
      if (!IsSubquestMode() && prevScroll == 0 && prevRow == 0 &&
          JOY_REPEAT(DPAD_UP)) {
        sStateDataPtr->isSelectorFocused = TRUE;
        PlaySE(SE_SELECT);
        Task_QuestMenuCleanUp(taskId);
        return;
      }

      s32 input = ListMenu_ProcessInput(data[0]);
      u8 selectedQuestId = sListMenuItems[GetCursorPosition()].id;

      ListMenuGetScrollAndRow(data[0], &sListMenuState.scroll,
                              &sListMenuState.row);

      switch (input) {
      case LIST_NOTHING_CHOSEN:
        if (JOY_NEW(R_BUTTON)) {
          ChangeModeAndCleanUp(taskId);
        }
        if (JOY_NEW(L_BUTTON)) {
          ChangeModeDecrementAndCleanUp(taskId);
        }
        if (JOY_NEW(START_BUTTON)) {
          ToggleAlphaModeAndCleanUp(taskId);
        }
        if (JOY_NEW(SELECT_BUTTON)) {
          ToggleFavoriteAndCleanUp(taskId, selectedQuestId);
        }
        break;

      case LIST_CANCEL:
        if (IsSubquestMode()) {
          ReturnFromSubquestAndCleanUp(taskId);
        } else {
          TurnOffQuestMenu(taskId);
        }
        break;

      default:
        if (!IsSubquestMode()) {
          if (sSideQuests[input].rewardItem != ITEM_NONE &&
              QuestMenu_GetSetQuestState(input, FLAG_GET_REWARD)) {
            ClaimQuestReward(taskId, input);
          } else if (DoesQuestHaveChildrenAndNotInactive(input)) {
            EnterSubquestModeAndCleanUp(taskId, data, input);
          } else if (QuestMenu_GetSetQuestState(input, FLAG_GET_REWARD)) {
            ClaimQuestReward(taskId, input);
          }
        } else {
          u8 parentQuest = sStateDataPtr->parentQuest;
          if (QuestMenu_GetSetSubquestState(parentQuest, FLAG_GET_COMPLETED,
                                            input)) {
            u16 rewardItem =
                sSideQuests[parentQuest].subquests[input].rewardItem;
            if (rewardItem != ITEM_NONE &&
                !QuestMenu_GetSetSubquestState(parentQuest, FLAG_GET_REWARD,
                                               input)) {
              ClaimSubquestReward(taskId, parentQuest, input);
            }
          }
        }
        break;
      }
    }
  }
}

void ManageFavorites(u8 selectedQuestId) {
  if (QuestMenu_GetSetQuestState(selectedQuestId, FLAG_GET_FAVORITE)) {
    QuestMenu_GetSetQuestState(selectedQuestId, FLAG_REMOVE_FAVORITE);
  } else {
    QuestMenu_GetSetQuestState(selectedQuestId, FLAG_SET_FAVORITE);
  }
}

static void Task_QuestMenuCleanUp(u8 taskId) {
  s16 *data = gTasks[taskId].data;

  QuestMenu_RemoveScrollIndicatorArrowPair();
  DestroyListMenuTask(data[0], &sListMenuState.scroll, &sListMenuState.row);
  ClearStdWindowAndFrameToTransparent(2, FALSE);

  InitItems();
  GenerateAndPrintHeader();
  PutWindowTilemap(2);
  DrawSelectorBox(sStateDataPtr->isSelectorFocused);
  ScheduleBgCopyTilemapToVram(0);
  AllocateResourcesForListMenu();
  BuildMenuTemplate();
  PlaceTopMenuScrollIndicatorArrows();

  if (sStateDataPtr->restoreCursor == TRUE) {
    RestoreSavedScrollAndRow(data);
  } else {
    ResetCursorToTop(data);
  }

  // Consume the flag so it doesn't leak into subsequent cleanup calls
  // (e.g., selector focus, Main/Side switching).
  sStateDataPtr->restoreCursor = FALSE;

  gTasks[taskId].func = Task_Main;
}

static void RestoreSavedScrollAndRow(s16 *data) {
  data[0] = ListMenuInit(&gMultiuseListMenuTemplate,
                         sListMenuState.storedScrollOffset,
                         sListMenuState.storedRowPosition);
}
static void ResetCursorToTop(s16 *data) {
  sListMenuState.row = 0;
  sListMenuState.scroll = 0;
  data[0] = ListMenuInit(&gMultiuseListMenuTemplate, sListMenuState.scroll,
                         sListMenuState.row);
}

static void QuestMenu_RemoveScrollIndicatorArrowPair(void) {
  if (sStateDataPtr->scrollIndicatorArrowPairId != 0xFF) {
    RemoveScrollIndicatorArrowPair(sStateDataPtr->scrollIndicatorArrowPairId);
    sStateDataPtr->scrollIndicatorArrowPairId = 0xFF;
  }
}

void EnterSubquestModeAndCleanUp(u8 taskId, s16 *data, s32 input) {
  if (DoesQuestHaveChildrenAndNotInactive(input)) {
    PrepareFadeOut(taskId);

    PlaySE(SE_SELECT);
    sStateDataPtr->parentQuest = input;
    sStateDataPtr->filterMode = ManageMode(SUB);
    SaveScrollAndRow(data);
    gTasks[taskId].func = Task_FadeOut;
  }
}
void ChangeModeAndCleanUp(u8 taskId) {
  if (!IsSubquestMode()) {
    PlaySE(SE_SELECT);
    sStateDataPtr->filterMode = ManageMode(INCREMENT);
    Task_QuestMenuCleanUp(taskId);
  }
}
void ChangeModeDecrementAndCleanUp(u8 taskId) {
  if (!IsSubquestMode()) {
    PlaySE(SE_SELECT);
    sStateDataPtr->filterMode = ManageMode(DECREMENT);
    Task_QuestMenuCleanUp(taskId);
  }
}
void ToggleAlphaModeAndCleanUp(u8 taskId) {
  if (!IsSubquestMode()) {
    PlaySE(SE_SELECT);
    sStateDataPtr->filterMode = ManageMode(ALPHA);
    Task_QuestMenuCleanUp(taskId);
  }
}
void ToggleFavoriteAndCleanUp(u8 taskId, u8 selectedQuestId) {
  if (!IsSubquestMode() && !CheckSelectedIsCancel(selectedQuestId)) {
    PlaySE(SE_SELECT);
    ManageFavorites(selectedQuestId);
    sStateDataPtr->restoreCursor = FALSE;
    Task_QuestMenuCleanUp(taskId);
  }
}
bool8 CheckSelectedIsCancel(u8 selectedQuestId) {
  if (selectedQuestId == (0xFF - 1)) {
    return TRUE;
  } else {
    return FALSE;
  }
}
void ReturnFromSubquestAndCleanUp(u8 taskId) {
  QuestMenu_DestroyBottomRightSprite();
  QuestMenu_DrawBottomRightBox(FALSE);
  PrepareFadeOut(taskId);

  PlaySE(SE_SELECT);
  sStateDataPtr->filterMode = ManageMode(SUB);
  gTasks[taskId].func = Task_FadeOut;
}

static void SetGpuRegBaseForFade() {
  // Sets the GPU registers to prepare for a hardware fade
  SetGpuReg(REG_OFFSET_BLDCNT,
            BLDCNT_TGT1_OBJ | BLDCNT_TGT1_BG0 | BLDCNT_TGT2_BG1 |
                BLDCNT_EFFECT_BLEND); // Blend Sprites and BG0 into BG1
  SetGpuReg(REG_OFFSET_BLDY, 0);
}

#define MAX_FADE_INTENSITY 16
#define MIN_FADE_INTENSITY 0

void InitFadeVariables(u8 taskId, u8 blendWeight, u8 frameDelay,
                       u8 frameTimerBase, u8 delta) {
  gTasks[taskId].data[1] = blendWeight;
  gTasks[taskId].data[2] = frameDelay;
  gTasks[taskId].data[3] = gTasks[taskId].data[frameTimerBase];
  gTasks[taskId].data[4] = delta;
}

static void PrepareFadeOut(u8 taskId) {
  SetGpuRegBaseForFade();
  SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(MAX_FADE_INTENSITY, 0));
  InitFadeVariables(taskId, MAX_FADE_INTENSITY, 0, 2, 2);
}

static bool8 HandleFadeOut(u8 taskId) {
  if (gTasks[taskId].data[3]-- != 0) {
    return FALSE;
  }

  // Set the timer, decrease the fade weight by the delta, increase the delta by
  // the timer
  gTasks[taskId].data[3] = gTasks[taskId].data[2];
  gTasks[taskId].data[1] -= gTasks[taskId].data[4];
  gTasks[taskId].data[2] += gTasks[taskId].data[3];

  // When blend weight runs out, set final blend and quit
  if (gTasks[taskId].data[1] <= 0) {
    SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(0, gTasks[taskId].data[1]));
    return TRUE;
  }
  // Set intermediate blend state
  SetGpuReg(REG_OFFSET_BLDALPHA,
            BLDALPHA_BLEND(gTasks[taskId].data[1],
                           MAX_FADE_INTENSITY - gTasks[taskId].data[1]));
  return FALSE;
}

static void PrepareFadeIn(u8 taskId) {
  SetGpuRegBaseForFade();
  SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(0, MAX_FADE_INTENSITY));
  InitFadeVariables(taskId, MIN_FADE_INTENSITY, 0, 1, 2);
}

static bool8 HandleFadeIn(u8 taskId) {
  // Set the timer, ncrease the fade weight by the delta,
  gTasks[taskId].data[3] = gTasks[taskId].data[2];
  gTasks[taskId].data[1] += gTasks[taskId].data[4];

  // When blend weight reaches max, set final blend and quit
  if (gTasks[taskId].data[1] >= MAX_FADE_INTENSITY) {
    SetGpuReg(REG_OFFSET_BLDALPHA,
              BLDALPHA_BLEND(MAX_FADE_INTENSITY, MIN_FADE_INTENSITY));
    return TRUE;
  }
  // Set intermediate blend state
  SetGpuReg(REG_OFFSET_BLDALPHA,
            BLDALPHA_BLEND(gTasks[taskId].data[1],
                           MAX_FADE_INTENSITY - gTasks[taskId].data[1]));
  return FALSE;
}

static void Task_FadeOut(u8 taskId) {
  if (HandleFadeOut(taskId)) {
    PrepareFadeIn(taskId);
    Task_QuestMenuCleanUp(taskId);
    gTasks[taskId].func = Task_FadeIn;
  }
}

static void Task_FadeIn(u8 taskId) {
  if (HandleFadeIn(taskId)) {
    gTasks[taskId].func = Task_Main;
  }
}

static void Task_QuestMenuWaitFadeAndBail(u8 taskId) {
  if (!gPaletteFade.active) {
    SetMainCallback2(sListMenuState.savedCallback);
    FreeResources();
    DestroyTask(taskId);
  }
}

static void FadeAndBail(void) {
  BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB_BLACK);
  CreateTask(Task_QuestMenuWaitFadeAndBail, 0);
  SetVBlankCallback(VBlankCB);
  SetMainCallback2(MainCB);
}

#define try_free(ptr)                                                          \
  ({                                                                           \
    void **ptr__ = (void **)&(ptr);                                            \
    if (*ptr__ != NULL)                                                        \
      Free(*ptr__);                                                            \
  })

static void FreeResources(void) {
  int i;

  QuestMenu_DestroyBottomRightSprite();
  try_free(sStateDataPtr);
  try_free(sBg1TilemapBuffer);
  try_free(sListMenuItems);

  for (i = QUEST_ARRAY_COUNT; i > -1; i--) {
    try_free(questNameArray[i]);
  }

  try_free(questNameArray);
  FreeAllWindowBuffers();
}

void TurnOffQuestMenu(u8 taskId) {
  SetInitializedFlag(0);
  gTasks[taskId].func = Task_QuestMenuTurnOff1;
}
static void Task_QuestMenuTurnOff1(u8 taskId) {
  BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB_BLACK);
  gTasks[taskId].func = Task_QuestMenuTurnOff2;
}

static void Task_QuestMenuTurnOff2(u8 taskId) {
  s16 *data = gTasks[taskId].data;

  if (!gPaletteFade.active) {
    DestroyListMenuTask(data[0], &sListMenuState.scroll, &sListMenuState.row);
    if (sStateDataPtr->savedCallback != NULL) {
      SetMainCallback2(sStateDataPtr->savedCallback);
    } else {
      SetMainCallback2(sListMenuState.savedCallback);
    }

    QuestMenu_RemoveScrollIndicatorArrowPair();
    FreeResources();
    DestroyTask(taskId);
  }
}

void Task_QuestMenu_OpenFromStartMenu(u8 taskId) {
  s16 *data = gTasks[taskId].data;
  if (!gPaletteFade.active) {
    CleanupOverworldWindowsAndTilemaps();
    QuestMenu_Init(tItemPcParam, CB2_ReturnToFieldWithOpenMenu);
    DestroyTask(taskId);
  }
}

void CB2_OpenQuestMenu(void) {
  QuestMenu_Init(0, CB2_ReturnToFieldWithOpenMenu);
}

void QuestMenu_CopyQuestName(u8 *dst, u8 questId) {
  if (questId >= QUEST_COUNT || sSideQuests[questId].name == NULL) {
    StringCopy(dst, sText_Empty);
    return;
  }
  StringCopy(dst, GetDisplayedQuestName(questId));
}

void QuestMenu_CopySubquestName(u8 *dst, u8 parentId, u8 childId) {
  StringCopy(dst, sSideQuests[parentId].subquests[childId].name);
}

void QuestMenu_ResetMenuSaveData(void) {
  memset(&gSaveBlock2Ptr->questData, 0, sizeof(gSaveBlock2Ptr->questData));
  memset(&gSaveBlock2Ptr->subQuests, 0, sizeof(gSaveBlock2Ptr->subQuests));
}

static void Task_ClaimRewardMessage(u8 taskId) {
  if (JOY_NEW(A_BUTTON | B_BUTTON)) {
    PlaySE(SE_SELECT);
    Task_QuestMenuCleanUp(taskId);
  }
}

static void ClaimQuestReward(u8 taskId, u8 questId) {
  u16 itemId = sSideQuests[questId].rewardItem;

  if (itemId != ITEM_NONE) {
    if (AddBagItem(itemId, 1)) {
      const u8 *pocketMsg;
      // Mark quest completed
      QuestMenu_GetSetQuestState(questId, FLAG_SET_COMPLETED);

      // Calculate new cursor position to prevent jumping to the top
      {
        u16 currentIndex = sListMenuState.scroll + sListMenuState.row;
        u16 new_nItems = sStateDataPtr->nItems - 1; // one item is removed from the active list
        u16 targetIndex = currentIndex;
        u16 newScroll = sListMenuState.scroll;
        u16 newRow = sListMenuState.row;

        if (targetIndex >= new_nItems) {
          if (new_nItems > 0) {
            targetIndex = new_nItems - 1;
          } else {
            targetIndex = 0;
          }
        }

        if (newScroll + newRow >= new_nItems) {
          if (targetIndex >= newScroll) {
            newRow = targetIndex - newScroll;
          } else {
            newScroll = targetIndex;
            newRow = 0;
          }
        }

        sListMenuState.storedScrollOffset = newScroll;
        sListMenuState.storedRowPosition = newRow;
        sStateDataPtr->restoreCursor = TRUE;
      }

      // Copy item name to gStringVar1, appending move name if it is a TM/HM
      if (IsItemTMHM(itemId)) {
        u16 moveId = GetItemTMHMMoveId(itemId);
        u8 *ptr = CopyItemName(itemId, gStringVar1);
        ptr = StringCopy(ptr, COMPOUND_STRING(" ("));
        ptr = StringCopy(ptr, gMovesInfo[moveId].name);
        ptr = StringCopy(ptr, COMPOUND_STRING(")"));
      } else {
        CopyItemName(itemId, gStringVar1);
      }

      switch (GetItemPocket(itemId)) {
      case POCKET_KEY_ITEMS:
        pocketMsg = COMPOUND_STRING("Obtained the {STR_VAR_1}!\nIt was placed "
                                    "in the Key Items pocket.");
        break;
      case POCKET_POKE_BALLS:
        pocketMsg = COMPOUND_STRING("Obtained the {STR_VAR_1}!\nIt was placed "
                                    "in the Poké Balls pocket.");
        break;
      case POCKET_TM_HM:
        pocketMsg = COMPOUND_STRING("Obtained the {STR_VAR_1}!\nIt was placed "
                                    "in the TMs & HMs pocket.");
        break;
      case POCKET_BERRIES:
        pocketMsg = COMPOUND_STRING(
            "Obtained the {STR_VAR_1}!\nIt was placed in the Berries pocket.");
        break;
      default:
        pocketMsg = COMPOUND_STRING(
            "Obtained the {STR_VAR_1}!\nIt was placed in the Items pocket.");
        break;
      }

      // Show success message
      StringExpandPlaceholders(gStringVar4, pocketMsg);

      // Clear and print to the footer window (window 1)
      FillWindowPixelBuffer(1, 0);
      
      // 1. Item Name in the Title
      QuestMenu_AddTextPrinterParameterized(1, 2, GetItemName(itemId), 2, 3, 2, 0, 0, 4);

      // 2. Item Description in the Description box
      QuestMenu_AddTextPrinterParameterized(1, 2, GetItemDescription(itemId), 40, 19, 2, 3, 0, 4);

      // 3. Obtained Message at the bottom of the window
      QuestMenu_AddTextPrinterParameterized(1, 2, gStringVar4, 40, 55, 2, 3, 0, 4);

      // Destroy old quest sprite, create reward item sprite and animate it
      {
        u8 activeSlot = sStateDataPtr->spriteIconSlot ^ 1;
        QuestMenu_DestroySprite(activeSlot);
        QuestMenu_CreateRewardSprite(itemId, activeSlot);
      }

      PlayFanfare(MUS_OBTAIN_TMHM);

      // Set task to wait for button press
      gTasks[taskId].func = Task_ClaimRewardMessage;
    } else {
      // Bag is full!
      FillWindowPixelBuffer(1, 0);
      QuestMenu_AddTextPrinterParameterized(
          1, 2, COMPOUND_STRING("Too bad!\nThe Bag is full..."), 2, 3, 2, 4, 0,
          4);

      PlaySE(SE_FAILURE);

      gTasks[taskId].func = Task_ClaimRewardMessage;
    }
  }
}

static void ClaimSubquestReward(u8 taskId, u8 parentQuest, u8 subQuestIndex) {
  u16 itemId = sSideQuests[parentQuest].subquests[subQuestIndex].rewardItem;

  if (itemId != ITEM_NONE) {
    if (AddBagItem(itemId, 1)) {
      const u8 *pocketMsg;
      QuestMenu_GetSetSubquestState(parentQuest, FLAG_SET_REWARD,
                                    subQuestIndex);

      // Keep cursor position for subquests
      sListMenuState.storedScrollOffset = sListMenuState.scroll;
      sListMenuState.storedRowPosition = sListMenuState.row;
      sStateDataPtr->restoreCursor = TRUE;

      if (IsItemTMHM(itemId)) {
        u16 moveId = GetItemTMHMMoveId(itemId);
        u8 *ptr = CopyItemName(itemId, gStringVar1);
        ptr = StringCopy(ptr, COMPOUND_STRING(" ("));
        ptr = StringCopy(ptr, gMovesInfo[moveId].name);
        ptr = StringCopy(ptr, COMPOUND_STRING(")"));
      } else {
        CopyItemName(itemId, gStringVar1);
      }

      switch (GetItemPocket(itemId)) {
      case POCKET_KEY_ITEMS:
        pocketMsg = COMPOUND_STRING("Obtained the {STR_VAR_1}!\nIt was placed "
                                    "in the Key Items pocket.");
        break;
      case POCKET_POKE_BALLS:
        pocketMsg = COMPOUND_STRING("Obtained the {STR_VAR_1}!\nIt was placed "
                                    "in the Poké Balls pocket.");
        break;
      case POCKET_TM_HM:
        pocketMsg = COMPOUND_STRING("Obtained the {STR_VAR_1}!\nIt was placed "
                                    "in the TMs & HMs pocket.");
        break;
      case POCKET_BERRIES:
        pocketMsg = COMPOUND_STRING(
            "Obtained the {STR_VAR_1}!\nIt was placed in the Berries pocket.");
        break;
      default:
        pocketMsg = COMPOUND_STRING(
            "Obtained the {STR_VAR_1}!\nIt was placed in the Items pocket.");
        break;
      }

      StringExpandPlaceholders(gStringVar4, pocketMsg);

      FillWindowPixelBuffer(1, 0);
      
      // 1. Item Name in the Title
      QuestMenu_AddTextPrinterParameterized(1, 2, GetItemName(itemId), 2, 3, 2, 0, 0, 4);

      // 2. Item Description in the Description box
      QuestMenu_AddTextPrinterParameterized(1, 2, GetItemDescription(itemId), 40, 19, 2, 3, 0, 4);

      // 3. Obtained Message at the bottom of the window
      QuestMenu_AddTextPrinterParameterized(1, 2, gStringVar4, 40, 55, 2, 3, 0, 4);

      {
        u8 activeSlot = sStateDataPtr->spriteIconSlot ^ 1;
        QuestMenu_DestroySprite(activeSlot);
        QuestMenu_CreateRewardSprite(itemId, activeSlot);
      }

      PlayFanfare(MUS_OBTAIN_TMHM);

      gTasks[taskId].func = Task_ClaimRewardMessage;
    } else {
      FillWindowPixelBuffer(1, 0);
      QuestMenu_AddTextPrinterParameterized(
          1, 2, COMPOUND_STRING("Too bad!\nThe Bag is full..."), 2, 3, 2, 4, 0,
          4);

      PlaySE(SE_FAILURE);

      gTasks[taskId].func = Task_ClaimRewardMessage;
    }
  }
}

static void QuestMenu_UnlockAndActivateQuest(u8 questId) {
  if (questId >= QUEST_COUNT || sSideQuests[questId].name == NULL)
    return;
  if (!QuestMenu_GetSetQuestState(questId, FLAG_GET_UNLOCKED)) {
    QuestMenu_GetSetQuestState(questId, FLAG_SET_UNLOCKED);
    QuestMenu_GetSetQuestState(questId, FLAG_SET_ACTIVE);
  }
}

void QuestMenu_SetSubquestCompleted(u16 subQuestId) {
  u8 index = subQuestId / 8;
  u8 bit = subQuestId % 8;
  u8 mask = 1 << bit;
  gSaveBlock2Ptr->subQuests[index] |= mask;

  // Find parent quest
  u8 parentId;
  for (parentId = 0; parentId < QUEST_COUNT; parentId++) {
    if (sSideQuests[parentId].name == NULL)
      continue;
    if (sSideQuests[parentId].numSubquests > 0) {
      u8 j;
      for (j = 0; j < sSideQuests[parentId].numSubquests; j++) {
        if (sSideQuests[parentId].subquests[j].id == subQuestId) {
          // Found the parent! Ensure it's unlocked and active
          QuestMenu_UnlockAndActivateQuest(parentId);

          // Check if all subquests are completed
          bool8 allCompleted = TRUE;
          u8 k;
          for (k = 0; k < sSideQuests[parentId].numSubquests; k++) {
            u8 subId = sSideQuests[parentId].subquests[k].id;
            u8 subIndex = subId / 8;
            u8 subBit = subId % 8;
            if (!(gSaveBlock2Ptr->subQuests[subIndex] & (1 << subBit))) {
              allCompleted = FALSE;
              break;
            }
          }

          if (allCompleted) {
            if (sSideQuests[parentId].rewardItem != 0) {
              QuestMenu_GetSetQuestState(parentId, FLAG_SET_REWARD);
            } else {
              QuestMenu_GetSetQuestState(parentId, FLAG_SET_COMPLETED);
            }
          }
          return;
        }
      }
    }
  }
}

bool8 QuestMenu_IsSubquestCompleted(u16 subQuestId) {
  u8 index = subQuestId / 8;
  u8 bit = subQuestId % 8;
  u8 mask = 1 << bit;
  return (gSaveBlock2Ptr->subQuests[index] & mask) != 0;
}

#if 0
void HandleQuestIconForSingleObjectEvent(struct ObjectEvent *objectEvent, u32 objectEventId)
{
    u32 localId = objectEvent->localId;
    u32 mapNum = objectEvent->mapNum;
    u32 mapGroup = objectEvent->mapGroup;
	u32 questId;

    const struct ObjectEventTemplate *obj = GetObjectEventTemplateByLocalIdAndMap(localId, mapNum, mapGroup);

	questId = obj->questId;

	// Never attempt to put a quest icon on the player
	if (objectEvent->movementType == MOVEMENT_TYPE_PLAYER)
    	return;

    if (obj == NULL)
        return;
	
	if (obj->trainerType != TRAINER_TYPE_QUEST_GIVER)
        return;

	// Remove icon if quest is completed
	if (QuestMenu_GetSetQuestState(questId, FLAG_GET_COMPLETED))
	{
		RemoveQuestIconFieldEffect(objectEvent);
		return;
	}

	// Already has icon? Do nothing
	if (ObjectEventAlreadyHasQuest(objectEvent->hasQuestIcon))
        return;

	// Add icon to NPCs who have quests
	if (!objectEvent->hasQuestIcon && !FieldEffectActiveListContains(FLDEFF_QUEST_ICON))
		SpawnQuestIconForObject(objectEvent, objectEventId);
}

static void RemoveQuestIconFieldEffect(struct ObjectEvent *objectEvent)
{
	objectEvent->hasQuestIcon = FALSE;
	
	if (FieldEffectActiveListContains(FLDEFF_QUEST_ICON))
	{
		u8 spriteId = objectEvent->spriteId;
		struct Sprite *sprite = &gSprites[spriteId];
		FieldEffectStop(sprite, FLDEFF_QUEST_ICON);
	}
}

static bool32 ObjectEventAlreadyHasQuest(bool32 hasQuestIcon)
{
    if (!FieldEffectActiveListContains(FLDEFF_QUEST_ICON))
        return FALSE;

    return (hasQuestIcon);
}


static void SpawnQuestIconForObject(struct ObjectEvent *objectEvent, u32 objectEventId)
{
	SetQuestIconOnObject(objectEvent);
	StartFieldEffectForObjectEvent(FLDEFF_QUEST_ICON, objectEvent);
}

void ResetQuestIconOnObject(struct ObjectEvent *objectEvent)
{
	objectEvent->hasQuestIcon = FALSE;
}

static void SetQuestIconOnObject(struct ObjectEvent *objectEvent)
{
	objectEvent->hasQuestIcon = TRUE;
}

void RefreshQuestIcons(void)
{
	u8 i;
	for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
	{
		if (gObjectEvents[i].active)
			HandleQuestIconForSingleObjectEvent(&gObjectEvents[i], i);
	}
}
#endif