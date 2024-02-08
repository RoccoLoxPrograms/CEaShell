/**
 * --------------------------------------
 * 
 * CEaShell Source Code - settings.c
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2024
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#include "defines.h"
#include "settings.h"
#include "menu.h"
#include "shapes.h"
#include "ui.h"
#include "utility.h"

#include <keypadc.h>

void settings_Open(struct preferences_t *shellPrefs, struct context_t *shellContext) {
    #ifdef FR
    ui_DrawStatusBar(shellPrefs, shellContext, "Param{tres");
    #else
    ui_DrawStatusBar(shellPrefs, shellContext, "Settings");
    #endif

    gfx_BlitBuffer();
    gfx_SetColor(shellPrefs->fgColor);

    if (shellPrefs->transitionSpeed) {
        for (int8_t frame = 3; frame < 16 / shellPrefs->transitionSpeed; frame++) {
            shapes_RoundRectangleFill(6, 312 - frame * 19 * shellPrefs->transitionSpeed, 232 - frame * 12 * shellPrefs->transitionSpeed, frame * (19 * shellPrefs->transitionSpeed), frame * (12 * shellPrefs->transitionSpeed));
            gfx_SwapDraw();
        }
    }

    shapes_RoundRectangleFill(6, 8, 28, 304, 204);
    ui_DrawUISprite(shellPrefs->fgColor, UI_RARROW, 290, 210);
    gfx_BlitBuffer();

    struct menu_t *menuContext = malloc(sizeof(struct menu_t));

    menuContext->totalOptions = 9;
    menuContext->optionSelected = 0;
    menuContext->totalHeight = 208;
    menuContext->options = malloc(sizeof(char *) * 9);
    menuContext->details = malloc(sizeof(char *) * 9);
    menuContext->types = malloc(sizeof(uint8_t) * 9);
    menuContext->values = malloc(sizeof(uint8_t) * 9);

    #ifdef FR
    menuContext->options[0] = "Icon Hook";
    menuContext->options[1] = "[on] Shortcuts";
    menuContext->options[2] = "Fast Alpha Scrolling";
    menuContext->options[3] = "Edit Archived Programs";
    menuContext->options[4] = "Edit Locked Programs";
    menuContext->options[5] = "Hide Programming Options";
    menuContext->options[6] = "Disable Busy Indicator";
    menuContext->options[7] = "Lowercase";
    menuContext->options[8] = "About CEaShell";

    menuContext->details[0] = "Show file icons and descriptions in the TI-OS programs and apps menus.";
    menuContext->details[1] =
        "Enable keyboard shortcuts.\n"
        "[on] + [prgm]: Launch CEaShell.\n"
        "[on] + [sto]/[ln]: Enable / Disable OS dark mode.\n"
        "[on] + [stat]: Trigger APD.\n"
        "[on] + [graph]: Jump to lbl in programs.";
    menuContext->details[2] = "Faster alpha scrolling in the TI-OS program editor.";
    menuContext->details[3] = "Allow the editing of archived programs in TI-OS.";
    menuContext->details[4] = "Allow the editing of locked TI-BASIC programs in CEaShell and when going to an error.";
    menuContext->details[5] = "On Python models, skip the TI-BASIC / Python menu and directly open the TI-BASIC program list when pressing the [prgm] key.";
    menuContext->details[6] = "Disable busy indicator in TI-BASIC programs.";
    menuContext->details[7] = "Enable lowercase in TI-OS by pressing [alpha] twice.";
    menuContext->details[8] = "Learn about the people who played a role in creating CEaShell!";
    #else
    menuContext->options[0] = "Icon Hook";
    menuContext->options[1] = "[on] Shortcuts";
    menuContext->options[2] = "Fast Alpha Scrolling";
    menuContext->options[3] = "Edit Archived Programs";
    menuContext->options[4] = "Edit Locked Programs";
    menuContext->options[5] = "Hide Programming Options";
    menuContext->options[6] = "Disable Busy Indicator";
    menuContext->options[7] = "Lowercase";
    menuContext->options[8] = "About CEaShell";

    menuContext->details[0] = "Show file icons and descriptions in the TI-OS programs and apps menus.";
    menuContext->details[1] =
        "Enable keyboard shortcuts.\n"
        "[on] + [prgm]: Launch CEaShell.\n"
        "[on] + [sto]/[ln]: Enable / Disable OS dark mode.\n"
        "[on] + [stat]: Trigger APD.\n"
        "[on] + [graph]: Jump to lbl in programs.";
    menuContext->details[2] = "Faster alpha scrolling in the TI-OS program editor.";
    menuContext->details[3] = "Allow the editing of archived programs in TI-OS.";
    menuContext->details[4] = "Allow the editing of locked TI-BASIC programs in CEaShell and when going to an error.";
    menuContext->details[5] = "On Python models, skip the TI-BASIC / Python menu and directly open the TI-BASIC program list when pressing the [prgm] key.";
    menuContext->details[6] = "Disable busy indicator in TI-BASIC programs.";
    menuContext->details[7] = "Enable lowercase in TI-OS by pressing [alpha] twice.";
    menuContext->details[8] = "Learn about the people who played a role in creating CEaShell!";
    #endif

    menuContext->types[0] = MENU_TYPE_BOOL;
    menuContext->types[1] = MENU_TYPE_BOOL;
    menuContext->types[2] = MENU_TYPE_BOOL;
    menuContext->types[3] = MENU_TYPE_BOOL;
    menuContext->types[4] = MENU_TYPE_BOOL;
    menuContext->types[5] = MENU_TYPE_BOOL;
    menuContext->types[6] = MENU_TYPE_BOOL;
    menuContext->types[7] = MENU_TYPE_BOOL;
    menuContext->types[8] = MENU_TYPE_MENU;

    menuContext->values[0] = bit(shellPrefs->getCSCHooks, ICON_HOOK);
    menuContext->values[1] = bit(shellPrefs->getCSCHooks, ON_SHORTS_HOOK);
    menuContext->values[2] = bit(shellPrefs->getCSCHooks, FAST_ALPHA_HOOK);
    menuContext->values[3] = shellPrefs->editArchivedProgs;
    menuContext->values[4] = shellPrefs->editLockedProgs;
    menuContext->values[5] = shellPrefs->hidePrgmOptions;
    menuContext->values[6] = shellPrefs->hideBusyIndicator;
    menuContext->values[7] = shellPrefs->osLowercase;
    menuContext->values[8] = '\0';

    int startY = 38;
    uint8_t optionY = 38;

    menu_Draw(shellPrefs, 15, 35, 38, 141, 168, menuContext);
    gfx_BlitBuffer();

    while(kb_AnyKey());

    bool keyPressed = false;
    clock_t clockOffset = clock(); // Keep track of an offset for timer stuff

    while (!kb_IsDown(kb_KeyClear) && !kb_IsDown(kb_KeyGraph)) {
        kb_Scan();
        util_UpdateKeyTimer(shellPrefs, shellContext, &clockOffset, &keyPressed);

        if ((kb_Data[7] || kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter)) && (!keyPressed || clock() - clockOffset > CLOCKS_PER_SEC / 32)) {
            if (kb_IsDown(kb_KeyUp)) {
                if (menuContext->optionSelected) {
                    uint8_t nextY = optionY - 5 - menu_CalculateLines(menuContext->options[menuContext->optionSelected - 1], (141 - menu_DrawValueString(0, 0, menuContext->types[menuContext->optionSelected - 1], 0) - 3) / 8, 3) * 12;

                    if (nextY < 38) {
                        startY += 38 - nextY;
                        optionY = 38;
                    } else {
                        optionY = nextY;
                    }

                    menuContext->optionSelected -= 1;
                } else {
                    startY = -2;
                    optionY = 193;
                    menuContext->optionSelected = 8;
                }
            } else if (kb_IsDown(kb_KeyDown)) {
                if (menuContext->optionSelected + 1 < menuContext->totalOptions) {
                    // Create variables to not call these functions so much
                    uint8_t nextY = optionY + 5 + menu_CalculateLines(menuContext->options[menuContext->optionSelected], (141 - menu_DrawValueString(0, 0, menuContext->types[menuContext->optionSelected], 0) - 3) / 8, 3) * 12;
                    uint8_t nextOptionHeight = menu_CalculateLines(menuContext->options[menuContext->optionSelected + 1], (141 - menu_DrawValueString(0, 0, menuContext->types[menuContext->optionSelected + 1], 0) - 3) / 8, 3) * 8;

                    if (nextOptionHeight > 8) {
                        nextOptionHeight += 4 * (nextOptionHeight / 8 - 1);
                    }

                    if (nextY + nextOptionHeight + 1 > 201) {
                        startY -= nextY + nextOptionHeight - 201;
                        optionY = 201 - nextOptionHeight;
                    } else {
                        optionY = nextY;
                    }

                    menuContext->optionSelected += 1;
                } else {
                    startY = 38;
                    optionY = 38;
                    menuContext->optionSelected = 0;
                }
            }

            if (kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter)) {
                switch (menuContext->optionSelected) {
                    case 0:
                        shellPrefs->getCSCHooks = toggle(shellPrefs->getCSCHooks, ICON_HOOK);
                        menuContext->values[0] = bit(shellPrefs->getCSCHooks, ICON_HOOK);
                        break;
                    case 1:
                        shellPrefs->getCSCHooks = toggle(shellPrefs->getCSCHooks, ON_SHORTS_HOOK);
                        menuContext->values[1] = bit(shellPrefs->getCSCHooks, ON_SHORTS_HOOK);
                        break;
                    case 2:
                        shellPrefs->getCSCHooks = toggle(shellPrefs->getCSCHooks, FAST_ALPHA_HOOK);
                        menuContext->values[2] = bit(shellPrefs->getCSCHooks, FAST_ALPHA_HOOK);
                        break;
                    case 3:
                        shellPrefs->editArchivedProgs = 3 * !shellPrefs->editArchivedProgs;
                        menuContext->values[3] = shellPrefs->editArchivedProgs;
                        break;
                    case 4:
                        shellPrefs->editLockedProgs = !shellPrefs->editLockedProgs;
                        menuContext->values[4] = shellPrefs->editLockedProgs;
                        break;
                    case 5:
                        shellPrefs->hidePrgmOptions = 2 * !shellPrefs->hidePrgmOptions;
                        menuContext->values[5] = shellPrefs->hidePrgmOptions;
                        break;
                    case 6:
                        shellPrefs->hideBusyIndicator = !shellPrefs->hideBusyIndicator;
                        menuContext->values[6] = shellPrefs->hideBusyIndicator;
                        break;
                    case 7:
                        shellPrefs->osLowercase = !shellPrefs->osLowercase;
                        menuContext->values[7] = shellPrefs->osLowercase;
                        break;
                    case 8:
                        menu_AboutScreen(shellPrefs, shellContext);
                        break;
                    default:
                        break;
                }
            }

            gfx_SetDrawBuffer();
            menu_Draw(shellPrefs, 15, 35, startY, 141, 168, menuContext);
            gfx_BlitBuffer();

            util_WaitBeforeKeypress(&clockOffset, &keyPressed);
        }
    }

    free(menuContext->options);
    free(menuContext->details);
    free(menuContext->types);
    free(menuContext->values);
    free(menuContext);
}
