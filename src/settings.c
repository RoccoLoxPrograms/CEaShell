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

#include <graphx.h>
#include <keypadc.h>
#include <time.h>

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

    gfx_BlitScreen();
    gfx_SwapDraw();

    shapes_RoundRectangleFill(6, 8, 28, 304, 204);
    ui_DrawUISprite(shellPrefs->fgColor, UI_RARROW, 290, 210);
    gfx_BlitBuffer();

    struct menu_t *menuContext = malloc(sizeof(struct menu_t));

    menuContext->totalOptions = 8;
    menuContext->optionSelected = 0;
    menuContext->totalHeight = 180;
    menuContext->options = malloc(sizeof(char *) * 8);
    menuContext->details = malloc(sizeof(char *) * 8);
    menuContext->types = malloc(sizeof(uint8_t) * 8);
    menuContext->values = malloc(sizeof(uint8_t) * 8);

    #ifdef FR
    menuContext->options[0] = "Icon Hook";
    menuContext->options[1] = "[on] Shortcuts";
    menuContext->options[2] = "Edit Archived Programs";
    menuContext->options[3] = "Edit Locked Programs";
    menuContext->options[4] = "Hide Programming Options";
    menuContext->options[5] = "Disable Busy Indicator";
    menuContext->options[6] = "Lowercase";
    menuContext->options[7] = "About CEaShell";

    menuContext->details[0] = "Show program icons and descriptions in the TI-OS programs menu.";
    menuContext->details[1] =
        "Enable keyboard shortcuts.\n"
        "[on] + [prgm]: Launch CEaShell.\n"
        "[on] + [sto]/[ln]: Enable / Disable OS dark mode.\n"
        "[on] + [stat]: Trigger APD.\n"
        "[on] + [graph]: Jump to lbl in programs.";
    menuContext->details[2] = "Allow the editing of archived programs in TI-OS.";
    menuContext->details[3] = "Allow the editing of locked TI-BASIC programs in CEaShell.";
    menuContext->details[4] = "On Python models, skip the TI-BASIC / Python menu and directly open the TI-BASIC program list when pressing the [prgm] key.";
    menuContext->details[5] = "Disable busy indicator in TI-BASIC programs.";
    menuContext->details[6] = "Enable lowercase in TI-OS by pressing [alpha] twice.";
    menuContext->details[7] = "Learn about the people who played a role in creating CEaShell!";
    #else
    menuContext->options[0] = "Icon Hook";
    menuContext->options[1] = "[on] Shortcuts";
    menuContext->options[2] = "Edit Archived Programs";
    menuContext->options[3] = "Edit Locked Programs";
    menuContext->options[4] = "Hide Programming Options";
    menuContext->options[5] = "Disable Busy Indicator";
    menuContext->options[6] = "Lowercase";
    menuContext->options[7] = "About CEaShell";

    menuContext->details[0] = "Show file icons and descriptions in the TI-OS programs and apps menus.";
    menuContext->details[1] =
        "Enable keyboard shortcuts.\n"
        "[on] + [prgm]: Launch CEaShell.\n"
        "[on] + [sto]/[ln]: Enable / Disable OS dark mode.\n"
        "[on] + [stat]: Trigger APD.\n"
        "[on] + [graph]: Jump to lbl in programs.";
    menuContext->details[2] = "Allow the editing of archived programs in TI-OS.";
    menuContext->details[3] = "Allow the editing of locked TI-BASIC programs in CEaShell.";
    menuContext->details[4] = "On Python models, skip the TI-BASIC / Python menu and directly open the TI-BASIC program list when pressing the [prgm] key.";
    menuContext->details[5] = "Disable busy indicator in TI-BASIC programs.";
    menuContext->details[6] = "Enable lowercase in TI-OS by pressing [alpha] twice.";
    menuContext->details[7] = "Learn about the people who played a role in creating CEaShell!";
    #endif

    menuContext->types[0] = MENU_TYPE_BOOL;
    menuContext->types[1] = MENU_TYPE_BOOL;
    menuContext->types[2] = MENU_TYPE_BOOL;
    menuContext->types[3] = MENU_TYPE_BOOL;
    menuContext->types[4] = MENU_TYPE_BOOL;
    menuContext->types[5] = MENU_TYPE_BOOL;
    menuContext->types[6] = MENU_TYPE_BOOL;
    menuContext->types[7] = MENU_TYPE_MENU;

    menuContext->values[0] = (shellPrefs->getCSCHooks == ICON_HOOK || shellPrefs->getCSCHooks == BOTH);
    menuContext->values[1] = (shellPrefs->getCSCHooks == ON_SHORTS_HOOK || shellPrefs->getCSCHooks == BOTH);
    menuContext->values[2] = shellPrefs->editArchivedProgs;
    menuContext->values[3] = shellPrefs->editLockedProgs;
    menuContext->values[4] = shellPrefs->hidePrgmOptions;
    menuContext->values[5] = shellPrefs->hideBusyIndicator;
    menuContext->values[6] = shellPrefs->osLowercase;
    menuContext->values[7] = '\0';

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
                    startY = 26;
                    optionY = 193;
                    menuContext->optionSelected = 7;
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
                        shellPrefs->getCSCHooks ^= ICON_HOOK;
                        menuContext->values[0] = (shellPrefs->getCSCHooks == ICON_HOOK || shellPrefs->getCSCHooks == BOTH);
                        break;
                    case 1:
                        shellPrefs->getCSCHooks ^= ON_SHORTS_HOOK;
                        menuContext->values[1] = (shellPrefs->getCSCHooks == ON_SHORTS_HOOK || shellPrefs->getCSCHooks == BOTH);
                        break;
                    case 2:
                        shellPrefs->editArchivedProgs = 3 * !shellPrefs->editArchivedProgs;
                        menuContext->values[2] = shellPrefs->editArchivedProgs;
                        break;
                    case 3:
                        shellPrefs->editLockedProgs = !shellPrefs->editLockedProgs;
                        menuContext->values[3] = shellPrefs->editLockedProgs;
                        break;
                    case 4:
                        shellPrefs->hidePrgmOptions = 2 * !shellPrefs->hidePrgmOptions;
                        menuContext->values[4] = shellPrefs->hidePrgmOptions;
                        break;
                    case 5:
                        shellPrefs->hideBusyIndicator = !shellPrefs->hideBusyIndicator;
                        menuContext->values[5] = shellPrefs->hideBusyIndicator;
                        break;
                    case 6:
                        shellPrefs->osLowercase = !shellPrefs->osLowercase;
                        menuContext->values[6] = shellPrefs->osLowercase;
                        break;
                    case 7:
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

void menu_AboutScreen(struct preferences_t *shellPrefs, struct context_t *shellContext) {
    #ifdef FR
    const char *specialThanks = "Special Thanks To: Code/Coding Help: MateoConLechuga, calc84maniac, commandblockguy, jacobly, Zeroko, and the CEdev Discord."
    " French translation: Shadow. Inspiration/Feature Ideas: KermMartian, Adriweb, epsilon5, NoahK,"
    " DJ Omnimaga. Beta Testing: Nanobot567, ChuckyHecker, darkwater4213, Oxiti8, LogicalJoe, Calculatordream."
    " And a big thank you to the members of our Discord for your support and ideas!";
    #else
    const char *specialThanks = "Special Thanks To: Code/Coding Help: MateoConLechuga, calc84maniac, commandblockguy, jacobly, Zeroko, and the CEdev Discord."
    " French translation: Shadow. Inspiration/Feature Ideas: KermMartian, Adriweb, epsilon5, NoahK,"
    " DJ Omnimaga. Beta Testing: Nanobot567, ChuckyHecker, darkwater4213, Oxiti8, LogicalJoe, Calculatordream."
    " And a big thank you to the members of our Discord for your support and ideas!";
    #endif
    unsigned int startDisplay = 0;
    gfx_SetColor(shellPrefs->bgColor);
    shapes_PixelIndentRectangle(14, 34, 292, 170);
    ui_CenterStringBig("CEaShell", 75, 38);
    gfx_PrintStringXY("v"VERSION_NO, 18, 58);
    #ifdef FR
    ui_PrintStringWrap(
        "CEaShell (pronounced \"seashell\"), "
        "is a shell created by RoccoLox Programs and TIny_Hacker. "
        "CEaShell aims to provide an easy to use, modern interface "
        "for the TI-84+ CE and TI-83 PCE calculators.", 18, 75, 40, 6);
    #else
    ui_PrintStringWrap(
        "CEaShell (pronounced \"seashell\"), "
        "is a shell created by RoccoLox Programs and TIny_Hacker. "
        "CEaShell aims to provide an easy to use, modern interface "
        "for the TI-84+ CE and TI-83 PCE calculators.", 18, 75, 40, 6);
    #endif
    gfx_PrintStringXY("(C) Copyright 2022 - 2024", 18, 179);
    gfx_PrintStringXY("RoccoLox Programs, TIny_Hacker", 18, 192);
    gfx_SetClipRegion(18, 38, 302, 202); // Helps cut the text off in the scrolling animation
    gfx_SetTextConfig(1);
    gfx_PrintStringXY(&specialThanks[startDisplay], 18, 151);
    gfx_BlitBuffer();

    clock_t clockOffset = clock();
    clock_t textRate = clock();

    while (!kb_IsDown(kb_KeyClear) && !kb_IsDown(kb_KeyAlpha) && !(clock() - clockOffset > 9000)) {
        kb_Scan();
    }

    while (!kb_IsDown(kb_KeyClear) && !kb_IsDown(kb_KeyAlpha)) {
        kb_Scan();

        if ((clock() - clockOffset >= ONE_MINUTE * shellPrefs->apdTimer && shellPrefs->apdTimer) || kb_On) {
            util_PowerOff(shellPrefs, shellContext);
        }

        if (clock() - textRate > CLOCKS_PER_SEC / 16) {
            gfx_SetColor(shellPrefs->bgColor);
            gfx_FillRectangle_NoClip(18, 151, 284, 8);
            gfx_PrintStringXY(&specialThanks[startDisplay], 18, 151);
            startDisplay++;
            gfx_BlitBuffer();
            textRate = clock();
        }

        if (startDisplay > 384) {
            startDisplay = 0; // restart
        }

        if (kb_AnyKey()) {
            clockOffset = clock();
        }
    }

    gfx_SetClipRegion(0, 0, 320, 240);
    gfx_SetTextConfig(0);
    gfx_SetColor(shellPrefs->fgColor);
    gfx_FillRectangle_NoClip(14, 34, 292, 170);

    while (kb_AnyKey());
}
