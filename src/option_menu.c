#include "global.h"
#include "bg.h"
#include "field_fadetransition.h"
#include "gba/m4a_internal.h"
#include "gpu_regs.h"
#include "help_system.h"
#include "list_menu.h"
#include "malloc.h"
#include "menu.h"
#include "menu_helpers.h"
#include "overworld.h"
#include "palette.h"
#include "scanline_effect.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "text_window.h"

// Settings indices (corresponding to options saved in saveblock)
enum {
  SETTING_TEXTSPEED = 0,
  SETTING_BATTLESCENE,
  SETTING_BATTLESTYLE,
  SETTING_SOUND,
  SETTING_BUTTONMODE,
  SETTING_FRAMETYPE,
  SETTING_SHOP_UI,
  SETTING_PARTY_MENU,
  SETTING_MAIN_MENU,
  SETTING_START_MENU,
  SETTING_GAME_MODE,
  SETTING_SEASONS,
  SETTING_TUTORIALS,
  SETTING_QUESTS,
  SETTING_COUNT
};

// Menu pages
enum { PAGE_MAIN = 0, PAGE_UI, PAGE_COUNT };

// Main page items
enum {
  MAIN_MENUITEM_TEXTSPEED = 0,
  MAIN_MENUITEM_BATTLESCENE,
  MAIN_MENUITEM_BATTLESTYLE,
  MAIN_MENUITEM_SOUND,
  MAIN_MENUITEM_BUTTONMODE,
  MAIN_MENUITEM_UI_SUBMENU,
  MAIN_MENUITEM_GAME_MODE,
  MAIN_MENUITEM_SEASONS,
  MAIN_MENUITEM_TUTORIALS,
  MAIN_MENUITEM_QUESTS,
  MAIN_MENUITEM_CANCEL,
  MAIN_MENUITEM_COUNT
};

// UI page items
enum {
  UI_MENUITEM_SHOP_UI = 0,
  UI_MENUITEM_PARTY_MENU,
  UI_MENUITEM_MAIN_MENU,
  UI_MENUITEM_START_MENU,
  UI_MENUITEM_FRAMETYPE,
  UI_MENUITEM_BACK,
  UI_MENUITEM_COUNT
};

// Window Ids
enum { WIN_TEXT_OPTION, WIN_OPTIONS };

#define MAX_VISIBLE_ITEMS 7

// RAM symbols
struct OptionMenu {
  /*0x00*/ u16 option[SETTING_COUNT];
  /*0x18*/ u16 cursorPos;
  /*0x1A*/ u8 loadState;
  /*0x1B*/ u8 state;
  /*0x1C*/ u8 loadPaletteState;
  /*0x1D*/ u16 scrollOffset;
  /*0x1F*/ u8 arrowTaskId;
  /*0x20*/ u8 page;
};

static EWRAM_DATA struct OptionMenu *sOptionMenuPtr = NULL;

// Function Declarataions
static void MainCB2(void);
static void VBlankCB(void);
static void OptionMenu_InitCallbacks(void);
static void OptionMenu_SetVBlankCallback(void);
static void CB2_OptionMenu(void);
static void SetOptionMenuTask(void);
static void InitOptionMenuBg(void);
static void OptionMenu_PickSwitchCancel(void);
static void OptionMenu_ResetSpriteData(void);
static bool8 LoadOptionMenuPalette(void);
static void Task_OptionMenu(u8 taskId);
static u8 OptionMenu_ProcessInput(void);
static void BufferOptionMenuString(u8 selection);
static void CloseAndSaveOptionMenu(u8 taskId);
static void PrintOptionMenuHeader(void);
static void DrawOptionMenuBg(void);
static void LoadOptionMenuItemNames(void);
static void UpdateSettingSelectionDisplay(u16 selection);
static u8 GetPageMenuItemCount(void);
static s8 GetSettingIndex(u8 page, u8 item);
static void TransitionToPage(u8 page, u8 cursor);

static const u8 sText_PickSwitchCancel[] =
    _("{DPAD_UPDOWN}Pick {DPAD_LEFTRIGHT}Switch {A_BUTTON}{B_BUTTON}Cancel");

// Data Definitions
static const struct WindowTemplate sOptionMenuWinTemplates[] = {
    {.bg = 1,
     .tilemapLeft = 2,
     .tilemapTop = 3,
     .width = 26,
     .height = 2,
     .paletteNum = 1,
     .baseBlock = 2},
    {.bg = 0,
     .tilemapLeft = 2,
     .tilemapTop = 7,
     .width = 26,
     .height = 12,
     .paletteNum = 1,
     .baseBlock = 0x36},
    {.bg = 2,
     .tilemapLeft = 0,
     .tilemapTop = 0,
     .width = 30,
     .height = 2,
     .paletteNum = 15,
     .baseBlock = 0x16e},
    DUMMY_WIN_TEMPLATE};

static const struct BgTemplate sOptionMenuBgTemplates[] = {
    {.bg = 1,
     .charBaseIndex = 1,
     .mapBaseIndex = 30,
     .screenSize = 0,
     .paletteMode = 0,
     .priority = 0,
     .baseTile = 0},
    {.bg = 0,
     .charBaseIndex = 1,
     .mapBaseIndex = 31,
     .screenSize = 0,
     .paletteMode = 0,
     .priority = 1,
     .baseTile = 0},
    {.bg = 2,
     .charBaseIndex = 1,
     .mapBaseIndex = 29,
     .screenSize = 0,
     .paletteMode = 0,
     .priority = 2,
     .baseTile = 0},
};

static const u16 sOptionMenuPalette[] =
    INCBIN_U16("graphics/misc/option_menu.gbapal");

static const u16 sOptionSettingsCounts[SETTING_COUNT] = {
    [SETTING_TEXTSPEED] = 3,   [SETTING_BATTLESCENE] = 2,
    [SETTING_BATTLESTYLE] = 2, [SETTING_SOUND] = 2,
    [SETTING_BUTTONMODE] = 3,  [SETTING_FRAMETYPE] = 10,
    [SETTING_SHOP_UI] = 2,     [SETTING_PARTY_MENU] = 2,
    [SETTING_MAIN_MENU] = 2,   [SETTING_START_MENU] = 2,
    [SETTING_GAME_MODE] = 2,   [SETTING_SEASONS] = 2,
    [SETTING_TUTORIALS] = 2,   [SETTING_QUESTS] = 2,
};

static const u8 *const sMainPageItemsNames[MAIN_MENUITEM_COUNT] = {
    [MAIN_MENUITEM_TEXTSPEED] = COMPOUND_STRING("Text Speed"),
    [MAIN_MENUITEM_BATTLESCENE] = COMPOUND_STRING("Battle Scene"),
    [MAIN_MENUITEM_BATTLESTYLE] = COMPOUND_STRING("Battle Style"),
    [MAIN_MENUITEM_SOUND] = COMPOUND_STRING("Sound"),
    [MAIN_MENUITEM_BUTTONMODE] = COMPOUND_STRING("Button Mode"),
    [MAIN_MENUITEM_UI_SUBMENU] = COMPOUND_STRING("UI"),
    [MAIN_MENUITEM_GAME_MODE] = COMPOUND_STRING("Game Mode"),
    [MAIN_MENUITEM_SEASONS] = COMPOUND_STRING("Seasons"),
    [MAIN_MENUITEM_TUTORIALS] = COMPOUND_STRING("Tutorials"),
    [MAIN_MENUITEM_QUESTS] = COMPOUND_STRING("Quests"),
    [MAIN_MENUITEM_CANCEL] = gText_Cancel,
};

static const u8 *const sUIPageItemsNames[UI_MENUITEM_COUNT] = {
    [UI_MENUITEM_SHOP_UI] = COMPOUND_STRING("Shop UI"),
    [UI_MENUITEM_PARTY_MENU] = COMPOUND_STRING("Party Menu"),
    [UI_MENUITEM_MAIN_MENU] = COMPOUND_STRING("Main Menu"),
    [UI_MENUITEM_START_MENU] = COMPOUND_STRING("Start Menu"),
    [UI_MENUITEM_FRAMETYPE] = COMPOUND_STRING("Frame"),
    [UI_MENUITEM_BACK] = COMPOUND_STRING("Back"),
};

static const u8 *const sMainMenuOptions[] = {
    COMPOUND_STRING("Modern"),
    COMPOUND_STRING("Classic"),
};

static const u8 *const sStartMenuOptions[] = {
    COMPOUND_STRING("Modern"),
    COMPOUND_STRING("Classic"),
};

static const u8 *const sTextSpeedOptions[] = {
    COMPOUND_STRING("Slow"),
    COMPOUND_STRING("Medium"),
    COMPOUND_STRING("Fast"),
};

static const u8 *const sBattleSceneOptions[] = {
    COMPOUND_STRING("On"),
    COMPOUND_STRING("Off"),
};

static const u8 *const sBattleStyleOptions[] = {
    COMPOUND_STRING("Shift"),
    COMPOUND_STRING("Set"),
};

static const u8 *const sSoundOptions[] = {
    COMPOUND_STRING("Mono"),
    COMPOUND_STRING("Stereo"),
};

static const u8 *const sButtonTypeOptions[] = {
    COMPOUND_STRING("Help"),
    COMPOUND_STRING("LR"),
    COMPOUND_STRING("L=A"),
};

static const u8 *const sShopUIOptions[] = {
    COMPOUND_STRING("Classic"),
    COMPOUND_STRING("Modern"),
};

static const u8 *const sPartyMenuOptions[] = {
    COMPOUND_STRING("Classic"),
    COMPOUND_STRING("DS Style"),
};

static const u8 *const sGameModeOptions[] = {
    COMPOUND_STRING("Vanilla"),
    COMPOUND_STRING("Vanilla+"),
};

static const u8 *const sQuestsOptions[] = {
    COMPOUND_STRING("Off"),
    COMPOUND_STRING("On"),
};

static const u8 *const sSeasonsOptions[] = {
    COMPOUND_STRING("Disabled"),
    COMPOUND_STRING("Enabled"),
};

static const u8 *const sTutorialsOptions[] = {
    COMPOUND_STRING("On"),
    COMPOUND_STRING("Off"),
};

static const u8 sOptionMenuPickSwitchCancelTextColor[] = {
    TEXT_DYNAMIC_COLOR_6, TEXT_COLOR_WHITE, TEXT_COLOR_DARK_GRAY};
static const u8 sOptionMenuTextColor[] = {TEXT_COLOR_TRANSPARENT,
                                          TEXT_COLOR_LIGHT_RED, TEXT_COLOR_RED};

// Functions
static void MainCB2(void) {
  RunTasks();
  AnimateSprites();
  BuildOamBuffer();
  UpdatePaletteFade();
}

static void VBlankCB(void) {
  LoadOam();
  ProcessSpriteCopyRequests();
  TransferPlttBuffer();
}

void CB2_InitOptionMenu(void) {
  u8 i;

  if (gMain.savedCallback == NULL)
    gMain.savedCallback = CB2_ReturnToFieldWithOpenMenu;
  sOptionMenuPtr = AllocZeroed(sizeof(struct OptionMenu));
  sOptionMenuPtr->loadState = 0;
  sOptionMenuPtr->loadPaletteState = 0;
  sOptionMenuPtr->state = 0;
  sOptionMenuPtr->cursorPos = 0;
  sOptionMenuPtr->scrollOffset = 0;
  sOptionMenuPtr->arrowTaskId = TASK_NONE;
  sOptionMenuPtr->page = PAGE_MAIN;
  sOptionMenuPtr->option[SETTING_TEXTSPEED] = gSaveBlock2Ptr->optionsTextSpeed;
  sOptionMenuPtr->option[SETTING_BATTLESCENE] =
      gSaveBlock2Ptr->optionsBattleSceneOff;
  sOptionMenuPtr->option[SETTING_BATTLESTYLE] =
      gSaveBlock2Ptr->optionsBattleStyle;
  sOptionMenuPtr->option[SETTING_SOUND] = gSaveBlock2Ptr->optionsSound;
  sOptionMenuPtr->option[SETTING_BUTTONMODE] =
      gSaveBlock2Ptr->optionsButtonMode;
  sOptionMenuPtr->option[SETTING_FRAMETYPE] =
      gSaveBlock2Ptr->optionsWindowFrameType;
  sOptionMenuPtr->option[SETTING_SHOP_UI] = gSaveBlock2Ptr->optionsModernShopUI;
  sOptionMenuPtr->option[SETTING_PARTY_MENU] =
      gSaveBlock2Ptr->optionsDSPartyMenu;
  sOptionMenuPtr->option[SETTING_MAIN_MENU] =
      gSaveBlock2Ptr->optionsCustomMainMenu;
  sOptionMenuPtr->option[SETTING_START_MENU] =
      gSaveBlock2Ptr->optionsCustomStartMenu;
  sOptionMenuPtr->option[SETTING_GAME_MODE] =
      gSaveBlock2Ptr->optionsVanillaPlusMode;
  sOptionMenuPtr->option[SETTING_SEASONS] =
      gSaveBlock2Ptr->optionsSeasons;
  sOptionMenuPtr->option[SETTING_TUTORIALS] =
      gSaveBlock2Ptr->optionsSkipTutorials;
  sOptionMenuPtr->option[SETTING_QUESTS] = gSaveBlock2Ptr->optionsEnableQuests;

  for (i = 0; i < SETTING_COUNT; i++) {
    if (sOptionMenuPtr->option[i] >= sOptionSettingsCounts[i])
      sOptionMenuPtr->option[i] = 0;
  }
  SetHelpContext(HELPCONTEXT_OPTIONS);
  SetMainCallback2(CB2_OptionMenu);
}

static void OptionMenu_InitCallbacks(void) {
  SetVBlankCallback(NULL);
  SetHBlankCallback(NULL);
}

static void OptionMenu_SetVBlankCallback(void) { SetVBlankCallback(VBlankCB); }

static void CB2_OptionMenu(void) {
  u8 i, state;
  state = sOptionMenuPtr->state;
  switch (state) {
  case 0:
    OptionMenu_InitCallbacks();
    break;
  case 1:
    InitOptionMenuBg();
    break;
  case 2:
    OptionMenu_ResetSpriteData();
    break;
  case 3:
    if (LoadOptionMenuPalette() != TRUE)
      return;
    break;
  case 4:
    PrintOptionMenuHeader();
    break;
  case 5:
    DrawOptionMenuBg();
    break;
  case 6:
    LoadOptionMenuItemNames();
    break;
  case 7:
    for (i = 0; i < MAX_VISIBLE_ITEMS; i++) {
      if (sOptionMenuPtr->scrollOffset + i < GetPageMenuItemCount())
        BufferOptionMenuString(sOptionMenuPtr->scrollOffset + i);
    }
    break;
  case 8:
    UpdateSettingSelectionDisplay(sOptionMenuPtr->cursorPos);
    {
      struct ScrollArrowsTemplate template;
      template.firstX = 228;
      template.firstY = 48;
      template.secondX = 228;
      template.secondY = 152;
      template.fullyUpThreshold = 0;
      template.fullyDownThreshold =
          (GetPageMenuItemCount() > MAX_VISIBLE_ITEMS)
              ? (GetPageMenuItemCount() - MAX_VISIBLE_ITEMS)
              : 0;
      template.firstArrowType = SCROLL_ARROW_UP;
      template.secondArrowType = SCROLL_ARROW_DOWN;
      template.tileTag = 2000;
      template.palTag = 100;
      template.palNum = 0;

      sOptionMenuPtr->arrowTaskId = AddScrollIndicatorArrowPair(
          &template, &(sOptionMenuPtr->scrollOffset));
    }
    break;
  case 9:
    OptionMenu_PickSwitchCancel();
    break;
  default:
    SetOptionMenuTask();
    break;
  }
  sOptionMenuPtr->state++;
}

static void SetOptionMenuTask(void) {
  CreateTask(Task_OptionMenu, 0);
  SetMainCallback2(MainCB2);
}

static void InitOptionMenuBg(void) {
  void *dest = (void *)VRAM;
  DmaClearLarge16(3, dest, VRAM_SIZE, 0x1000);
  DmaClear32(3, (void *)OAM, OAM_SIZE);
  DmaClear16(3, (void *)PLTT, PLTT_SIZE);
  SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0);
  ResetBgsAndClearDma3BusyFlags(0);
  InitBgsFromTemplates(0, sOptionMenuBgTemplates,
                       ARRAY_COUNT(sOptionMenuBgTemplates));
  ChangeBgX(0, 0, 0);
  ChangeBgY(0, 0, 0);
  ChangeBgX(1, 0, 0);
  ChangeBgY(1, 0, 0);
  ChangeBgX(2, 0, 0);
  ChangeBgY(2, 0, 0);
  ChangeBgX(3, 0, 0);
  ChangeBgY(3, 0, 0);
  InitWindows(sOptionMenuWinTemplates);
  DeactivateAllTextPrinters();
  SetGpuReg(REG_OFFSET_BLDCNT,
            BLDCNT_TGT1_BG0 | BLDCNT_EFFECT_BLEND | BLDCNT_EFFECT_LIGHTEN);
  SetGpuReg(REG_OFFSET_BLDY, BLDCNT_TGT1_BG1);
  SetGpuReg(REG_OFFSET_WININ, WININ_WIN0_BG0);
  SetGpuReg(REG_OFFSET_WINOUT, WINOUT_WIN01_BG0 | WINOUT_WIN01_BG1 |
                                   WINOUT_WIN01_BG2 | WINOUT_WIN01_CLR);
  SetGpuReg(REG_OFFSET_DISPCNT,
            DISPCNT_OBJ_1D_MAP | DISPCNT_OBJ_ON | DISPCNT_WIN0_ON);
  ShowBg(0);
  ShowBg(1);
  ShowBg(2);
};

static void OptionMenu_PickSwitchCancel(void) {
  s32 x;
  x = 0xE4 - GetStringWidth(FONT_SMALL, sText_PickSwitchCancel, 0);
  FillWindowPixelBuffer(2, PIXEL_FILL(15));
  AddTextPrinterParameterized3(2, FONT_SMALL, x, 0,
                               sOptionMenuPickSwitchCancelTextColor, 0,
                               sText_PickSwitchCancel);
  PutWindowTilemap(2);
  CopyWindowToVram(2, COPYWIN_FULL);
}

static void OptionMenu_ResetSpriteData(void) {
  ResetSpriteData();
  ResetPaletteFade();
  FreeAllSpritePalettes();
  ResetTasks();
  ScanlineEffect_Stop();
}

static bool8 LoadOptionMenuPalette(void) {
  switch (sOptionMenuPtr->loadPaletteState) {
  case 0:
    LoadBgTiles(
        1,
        GetWindowFrameTilesPal(sOptionMenuPtr->option[SETTING_FRAMETYPE])
            ->tiles,
        0x120, 0x1AA);
    break;
  case 1:
    LoadPalette(
        GetWindowFrameTilesPal(sOptionMenuPtr->option[SETTING_FRAMETYPE])->pal,
        BG_PLTT_ID(2), PLTT_SIZE_4BPP);
    break;
  case 2:
    LoadPalette(sOptionMenuPalette, BG_PLTT_ID(1), sizeof(sOptionMenuPalette));
    LoadPalette(GetTextWindowPalette(2), BG_PLTT_ID(15), PLTT_SIZE_4BPP);
    break;
  case 3:
    LoadStdWindowGfxOnBg(1, 0x1B3, BG_PLTT_ID(3));
    break;
  default:
    return TRUE;
  }
  sOptionMenuPtr->loadPaletteState++;
  return FALSE;
}

static void Task_OptionMenu(u8 taskId) {
  u8 i;
  switch (sOptionMenuPtr->loadState) {
  case 0:
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0x10, 0, RGB_BLACK);
    OptionMenu_SetVBlankCallback();
    sOptionMenuPtr->loadState++;
    break;
  case 1:
    if (gPaletteFade.active)
      return;
    sOptionMenuPtr->loadState++;
    break;
  case 2:
    if (IsActiveOverworldLinkBusy() == TRUE)
      break;
    switch (OptionMenu_ProcessInput()) {
    case 0:
      break;
    case 1:
      sOptionMenuPtr->loadState++;
      break;
    case 2:
      LoadBgTiles(
          1,
          GetWindowFrameTilesPal(sOptionMenuPtr->option[SETTING_FRAMETYPE])
              ->tiles,
          0x120, 0x1AA);
      LoadPalette(
          GetWindowFrameTilesPal(sOptionMenuPtr->option[SETTING_FRAMETYPE])
              ->pal,
          BG_PLTT_ID(2), PLTT_SIZE_4BPP);
      BufferOptionMenuString(sOptionMenuPtr->cursorPos);
      break;
    case 3:
      UpdateSettingSelectionDisplay(sOptionMenuPtr->cursorPos);
      break;
    case 4:
      BufferOptionMenuString(sOptionMenuPtr->cursorPos);
      break;
    case 5:
      LoadOptionMenuItemNames();
      for (i = 0; i < MAX_VISIBLE_ITEMS; i++) {
        if (sOptionMenuPtr->scrollOffset + i < GetPageMenuItemCount())
          BufferOptionMenuString(sOptionMenuPtr->scrollOffset + i);
      }
      UpdateSettingSelectionDisplay(sOptionMenuPtr->cursorPos);
      break;
    }
    break;
  case 3:
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_BLACK);
    sOptionMenuPtr->loadState++;
    break;
  case 4:
    if (gPaletteFade.active)
      return;
    sOptionMenuPtr->loadState++;
    break;
  case 5:
    CloseAndSaveOptionMenu(taskId);
    break;
  }
}

static u8 OptionMenu_ProcessInput(void) {
  u16 current;
  u16 *curr;
  s8 settingIndex =
      GetSettingIndex(sOptionMenuPtr->page, sOptionMenuPtr->cursorPos);

  if (JOY_REPEAT(DPAD_RIGHT)) {
    if (settingIndex >= 0) {
      if (settingIndex == SETTING_GAME_MODE)
        return 0; // Read-only
      current = sOptionMenuPtr->option[settingIndex];
      if (current == (sOptionSettingsCounts[settingIndex] - 1))
        sOptionMenuPtr->option[settingIndex] = 0;
      else
        sOptionMenuPtr->option[settingIndex] = current + 1;
      if (settingIndex == SETTING_FRAMETYPE)
        return 2;
      else
        return 4;
    }
    return 0;
  } else if (JOY_REPEAT(DPAD_LEFT)) {
    if (settingIndex >= 0) {
      if (settingIndex == SETTING_GAME_MODE)
        return 0; // Read-only
      curr = &sOptionMenuPtr->option[settingIndex];
      if (*curr == 0)
        *curr = sOptionSettingsCounts[settingIndex] - 1;
      else
        --*curr;

      if (settingIndex == SETTING_FRAMETYPE)
        return 2;
      else
        return 4;
    }
    return 0;
  } else if (JOY_REPEAT(DPAD_UP)) {
    if (sOptionMenuPtr->cursorPos == 0) {
      sOptionMenuPtr->cursorPos = GetPageMenuItemCount() - 1;
      sOptionMenuPtr->scrollOffset =
          (GetPageMenuItemCount() > MAX_VISIBLE_ITEMS)
              ? GetPageMenuItemCount() - MAX_VISIBLE_ITEMS
              : 0;
      return 5;
    } else {
      sOptionMenuPtr->cursorPos = sOptionMenuPtr->cursorPos - 1;
      if (sOptionMenuPtr->cursorPos < sOptionMenuPtr->scrollOffset) {
        sOptionMenuPtr->scrollOffset = sOptionMenuPtr->cursorPos;
        return 5;
      }
    }
    return 3;
  } else if (JOY_REPEAT(DPAD_DOWN)) {
    if (sOptionMenuPtr->cursorPos == GetPageMenuItemCount() - 1) {
      sOptionMenuPtr->cursorPos = 0;
      sOptionMenuPtr->scrollOffset = 0;
      return 5;
    } else {
      sOptionMenuPtr->cursorPos = sOptionMenuPtr->cursorPos + 1;
      if (sOptionMenuPtr->cursorPos >=
          sOptionMenuPtr->scrollOffset + MAX_VISIBLE_ITEMS) {
        sOptionMenuPtr->scrollOffset =
            sOptionMenuPtr->cursorPos - MAX_VISIBLE_ITEMS + 1;
        return 5;
      }
    }
    return 3;
  } else if (JOY_NEW(A_BUTTON)) {
    if (sOptionMenuPtr->page == PAGE_MAIN) {
      if (sOptionMenuPtr->cursorPos == MAIN_MENUITEM_UI_SUBMENU) {
        TransitionToPage(PAGE_UI, 0);
        return 5;
      } else if (sOptionMenuPtr->cursorPos == MAIN_MENUITEM_CANCEL) {
        return 1;
      }
    } else {
      if (sOptionMenuPtr->cursorPos == UI_MENUITEM_BACK) {
        TransitionToPage(PAGE_MAIN, MAIN_MENUITEM_UI_SUBMENU);
        return 5;
      }
    }
    return 0;
  } else if (JOY_NEW(B_BUTTON)) {
    if (sOptionMenuPtr->page == PAGE_UI) {
      TransitionToPage(PAGE_MAIN, MAIN_MENUITEM_UI_SUBMENU);
      return 5;
    }
    return 1;
  } else {
    return 0;
  }
}

static void BufferOptionMenuString(u8 selection) {
  u8 str[20];
  u8 buf[12];
  u8 dst[3];
  u8 x, y;
  s8 settingIndex = GetSettingIndex(sOptionMenuPtr->page, selection);

  memcpy(dst, sOptionMenuTextColor, 3);
  if (settingIndex == SETTING_GAME_MODE) {
    dst[1] = TEXT_COLOR_LIGHT_GRAY;
    dst[2] = TEXT_COLOR_DARK_GRAY;
  }
  x = 0x82;
  y = ((GetFontAttribute(FONT_NORMAL, FONTATTR_MAX_LETTER_HEIGHT) - 1) *
       (selection - sOptionMenuPtr->scrollOffset)) +
      2;
  FillWindowPixelRect(
      1, 1, x, y, 0x46,
      GetFontAttribute(FONT_NORMAL, FONTATTR_MAX_LETTER_HEIGHT));

  if (settingIndex >= 0) {
    switch (settingIndex) {
    case SETTING_TEXTSPEED:
      AddTextPrinterParameterized3(
          1, FONT_NORMAL, x, y, dst, -1,
          sTextSpeedOptions[sOptionMenuPtr->option[settingIndex]]);
      break;
    case SETTING_BATTLESCENE:
      AddTextPrinterParameterized3(
          1, FONT_NORMAL, x, y, dst, -1,
          sBattleSceneOptions[sOptionMenuPtr->option[settingIndex]]);
      break;
    case SETTING_BATTLESTYLE:
      AddTextPrinterParameterized3(
          1, FONT_NORMAL, x, y, dst, -1,
          sBattleStyleOptions[sOptionMenuPtr->option[settingIndex]]);
      break;
    case SETTING_SOUND:
      AddTextPrinterParameterized3(
          1, FONT_NORMAL, x, y, dst, -1,
          sSoundOptions[sOptionMenuPtr->option[settingIndex]]);
      break;
    case SETTING_BUTTONMODE:
      AddTextPrinterParameterized3(
          1, FONT_NORMAL, x, y, dst, -1,
          sButtonTypeOptions[sOptionMenuPtr->option[settingIndex]]);
      break;
    case SETTING_SHOP_UI:
      AddTextPrinterParameterized3(
          1, FONT_NORMAL, x, y, dst, -1,
          sShopUIOptions[sOptionMenuPtr->option[settingIndex]]);
      break;
    case SETTING_PARTY_MENU:
      AddTextPrinterParameterized3(
          1, FONT_NORMAL, x, y, dst, -1,
          sPartyMenuOptions[sOptionMenuPtr->option[settingIndex]]);
      break;
    case SETTING_MAIN_MENU:
      AddTextPrinterParameterized3(
          1, FONT_NORMAL, x, y, dst, -1,
          sMainMenuOptions[sOptionMenuPtr->option[settingIndex]]);
      break;
    case SETTING_START_MENU:
      AddTextPrinterParameterized3(
          1, FONT_NORMAL, x, y, dst, -1,
          sStartMenuOptions[sOptionMenuPtr->option[settingIndex]]);
      break;
    case SETTING_GAME_MODE:
      AddTextPrinterParameterized3(
          1, FONT_NORMAL, x, y, dst, -1,
          sGameModeOptions[sOptionMenuPtr->option[settingIndex]]);
      break;
    case SETTING_SEASONS:
      AddTextPrinterParameterized3(
          1, FONT_NORMAL, x, y, dst, -1,
          sSeasonsOptions[sOptionMenuPtr->option[settingIndex]]);
      break;
    case SETTING_TUTORIALS:
      AddTextPrinterParameterized3(
          1, FONT_NORMAL, x, y, dst, -1,
          sTutorialsOptions[sOptionMenuPtr->option[settingIndex]]);
      break;
    case SETTING_QUESTS:
      AddTextPrinterParameterized3(
          1, FONT_NORMAL, x, y, dst, -1,
          sQuestsOptions[sOptionMenuPtr->option[settingIndex]]);
      break;
    case SETTING_FRAMETYPE:
      StringCopy(str, COMPOUND_STRING("Type "));
      ConvertIntToDecimalStringN(buf, sOptionMenuPtr->option[settingIndex] + 1,
                                 1, 2);
      StringAppendN(str, buf, 3);
      AddTextPrinterParameterized3(1, FONT_NORMAL, x, y, dst, -1, str);
      break;
    }
  } else {
    if (sOptionMenuPtr->page == PAGE_MAIN &&
        selection == MAIN_MENUITEM_UI_SUBMENU) {
      AddTextPrinterParameterized3(1, FONT_NORMAL, x, y, dst, -1,
                                   COMPOUND_STRING(">"));
    }
  }
  PutWindowTilemap(1);
  CopyWindowToVram(1, COPYWIN_FULL);
}

static void CloseAndSaveOptionMenu(u8 taskId) {
  gFieldCallback = FieldCB_DefaultWarpExit;
  SetMainCallback2(gMain.savedCallback);
  FreeAllWindowBuffers();
  gSaveBlock2Ptr->optionsTextSpeed = sOptionMenuPtr->option[SETTING_TEXTSPEED];
  gSaveBlock2Ptr->optionsBattleSceneOff =
      sOptionMenuPtr->option[SETTING_BATTLESCENE];
  gSaveBlock2Ptr->optionsBattleStyle =
      sOptionMenuPtr->option[SETTING_BATTLESTYLE];
  gSaveBlock2Ptr->optionsSound = sOptionMenuPtr->option[SETTING_SOUND];
  gSaveBlock2Ptr->optionsButtonMode =
      sOptionMenuPtr->option[SETTING_BUTTONMODE];
  gSaveBlock2Ptr->optionsWindowFrameType =
      sOptionMenuPtr->option[SETTING_FRAMETYPE];
  gSaveBlock2Ptr->optionsModernShopUI = sOptionMenuPtr->option[SETTING_SHOP_UI];
  gSaveBlock2Ptr->optionsDSPartyMenu =
      sOptionMenuPtr->option[SETTING_PARTY_MENU];
  gSaveBlock2Ptr->optionsCustomMainMenu =
      sOptionMenuPtr->option[SETTING_MAIN_MENU];
  gSaveBlock2Ptr->optionsCustomStartMenu =
      sOptionMenuPtr->option[SETTING_START_MENU];
  gSaveBlock2Ptr->optionsSeasons = sOptionMenuPtr->option[SETTING_SEASONS];
  gSaveBlock2Ptr->optionsSkipTutorials = sOptionMenuPtr->option[SETTING_TUTORIALS];
  gSaveBlock2Ptr->optionsEnableQuests = sOptionMenuPtr->option[SETTING_QUESTS];
  SetPokemonCryStereo(gSaveBlock2Ptr->optionsSound);
  if (sOptionMenuPtr->arrowTaskId != TASK_NONE) {
    RemoveScrollIndicatorArrowPair(sOptionMenuPtr->arrowTaskId);
  }
  FREE_AND_SET_NULL(sOptionMenuPtr);
  DestroyTask(taskId);
}

static void PrintOptionMenuHeader(void) {
  FillWindowPixelBuffer(0, PIXEL_FILL(1));
  AddTextPrinterParameterized(WIN_TEXT_OPTION, FONT_NORMAL, gText_Option, 8, 1,
                              TEXT_SKIP_DRAW, NULL);
  PutWindowTilemap(0);
  CopyWindowToVram(0, COPYWIN_FULL);
}

static void DrawOptionMenuBg(void) {
  u8 h;
  h = 2;

  FillBgTilemapBufferRect(1, 0x1B3, 1, 2, 1, 1, 3);
  FillBgTilemapBufferRect(1, 0x1B4, 2, 2, 0x1B, 1, 3);
  FillBgTilemapBufferRect(1, 0x1B5, 0x1C, 2, 1, 1, 3);
  FillBgTilemapBufferRect(1, 0x1B6, 1, 3, 1, h, 3);
  FillBgTilemapBufferRect(1, 0x1B8, 0x1C, 3, 1, h, 3);
  FillBgTilemapBufferRect(1, 0x1B9, 1, 5, 1, 1, 3);
  FillBgTilemapBufferRect(1, 0x1BA, 2, 5, 0x1B, 1, 3);
  FillBgTilemapBufferRect(1, 0x1BB, 0x1C, 5, 1, 1, 3);
  FillBgTilemapBufferRect(1, 0x1AA, 1, 6, 1, 1, h);
  FillBgTilemapBufferRect(1, 0x1AB, 2, 6, 0x1A, 1, h);
  FillBgTilemapBufferRect(1, 0x1AC, 0x1C, 6, 1, 1, h);
  FillBgTilemapBufferRect(1, 0x1AD, 1, 7, 1, 0x10, h);
  FillBgTilemapBufferRect(1, 0x1AF, 0x1C, 7, 1, 0x10, h);
  FillBgTilemapBufferRect(1, 0x1B0, 1, 0x13, 1, 1, h);
  FillBgTilemapBufferRect(1, 0x1B1, 2, 0x13, 0x1A, 1, h);
  FillBgTilemapBufferRect(1, 0x1B2, 0x1C, 0x13, 1, 1, h);
  CopyBgTilemapBufferToVram(1);
}

static void LoadOptionMenuItemNames(void) {
  u8 i;

  FillWindowPixelBuffer(1, PIXEL_FILL(1));
  for (i = 0; i < MAX_VISIBLE_ITEMS; i++) {
    u8 itemIndex = sOptionMenuPtr->scrollOffset + i;
    if (itemIndex >= GetPageMenuItemCount())
      break;
    AddTextPrinterParameterized(
        WIN_OPTIONS, FONT_NORMAL,
        (sOptionMenuPtr->page == PAGE_UI) ? sUIPageItemsNames[itemIndex]
                                          : sMainPageItemsNames[itemIndex],
        8,
        (u8)((i * (GetFontAttribute(FONT_NORMAL, FONTATTR_MAX_LETTER_HEIGHT))) +
             2) -
            i,
        TEXT_SKIP_DRAW, NULL);
  }
}

static void UpdateSettingSelectionDisplay(u16 selection) {
  u16 maxLetterHeight, y;

  maxLetterHeight = GetFontAttribute(FONT_NORMAL, FONTATTR_MAX_LETTER_HEIGHT);
  y = (selection - sOptionMenuPtr->scrollOffset) * (maxLetterHeight - 1) + 0x3A;
  SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE(y, y + maxLetterHeight));
  SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(0x10, 0xE0));
}

static u8 GetPageMenuItemCount(void) {
  if (sOptionMenuPtr->page == PAGE_UI)
    return UI_MENUITEM_COUNT;
  return MAIN_MENUITEM_COUNT;
}

static s8 GetSettingIndex(u8 page, u8 item) {
  if (page == PAGE_MAIN) {
    switch (item) {
    case MAIN_MENUITEM_TEXTSPEED:
      return SETTING_TEXTSPEED;
    case MAIN_MENUITEM_BATTLESCENE:
      return SETTING_BATTLESCENE;
    case MAIN_MENUITEM_BATTLESTYLE:
      return SETTING_BATTLESTYLE;
    case MAIN_MENUITEM_SOUND:
      return SETTING_SOUND;
    case MAIN_MENUITEM_BUTTONMODE:
      return SETTING_BUTTONMODE;
    case MAIN_MENUITEM_GAME_MODE:
      return SETTING_GAME_MODE;
    case MAIN_MENUITEM_SEASONS:
      return SETTING_SEASONS;
    case MAIN_MENUITEM_TUTORIALS:
      return SETTING_TUTORIALS;
    case MAIN_MENUITEM_QUESTS:
      return SETTING_QUESTS;
    }
  } else {
    switch (item) {
    case UI_MENUITEM_SHOP_UI:
      return SETTING_SHOP_UI;
    case UI_MENUITEM_PARTY_MENU:
      return SETTING_PARTY_MENU;
    case UI_MENUITEM_MAIN_MENU:
      return SETTING_MAIN_MENU;
    case UI_MENUITEM_START_MENU:
      return SETTING_START_MENU;
    case UI_MENUITEM_FRAMETYPE:
      return SETTING_FRAMETYPE;
    }
  }
  return -1;
}

static void TransitionToPage(u8 page, u8 cursor) {
  sOptionMenuPtr->page = page;
  sOptionMenuPtr->cursorPos = cursor;

  if (cursor < MAX_VISIBLE_ITEMS) {
    sOptionMenuPtr->scrollOffset = 0;
  } else {
    sOptionMenuPtr->scrollOffset = cursor - MAX_VISIBLE_ITEMS + 1;
  }

  if (sOptionMenuPtr->arrowTaskId != TASK_NONE) {
    RemoveScrollIndicatorArrowPair(sOptionMenuPtr->arrowTaskId);
    sOptionMenuPtr->arrowTaskId = TASK_NONE;
  }

  {
    struct ScrollArrowsTemplate template;
    template.firstX = 228;
    template.firstY = 48;
    template.secondX = 228;
    template.secondY = 152;
    template.fullyUpThreshold = 0;
    template.fullyDownThreshold =
        (GetPageMenuItemCount() > MAX_VISIBLE_ITEMS)
            ? (GetPageMenuItemCount() - MAX_VISIBLE_ITEMS)
            : 0;
    template.firstArrowType = SCROLL_ARROW_UP;
    template.secondArrowType = SCROLL_ARROW_DOWN;
    template.tileTag = 2000;
    template.palTag = 100;
    template.palNum = 0;

    sOptionMenuPtr->arrowTaskId =
        AddScrollIndicatorArrowPair(&template, &(sOptionMenuPtr->scrollOffset));
  }
}
