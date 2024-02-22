/**
 * --------------------------------------
 * 
 * CEaShell Source Code - menu.c
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2024
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#include "defines.h"
#include "shapes.h"
#include "ui.h"
#include "utility.h"

#include "asm/apps.h"
#include "asm/fileOps.h"
#include "asm/utils.h"
#include "gfx/gfx.h"

#include <fileioc.h>
#include <keypadc.h>
#include <string.h>

uint8_t menu_DrawValueString(unsigned int xEdge, int y, uint8_t type, uint8_t value) {
    uint8_t width = 0;

    switch (type) {
        case MENU_TYPE_MENU:
            return 0;
        case MENU_TYPE_BOOL:
            width = 10;

            if (xEdge) {
                ui_CheckBox(value, xEdge - 13, y);
            }

            return width;
        case MENU_TYPE_APD:
            width = 35;

            if (xEdge) {
                if (value) {
                    gfx_SetTextXY(xEdge - 24, y);
                    gfx_PrintUInt(value, 0);
                } else {
                    #ifdef FR
                    gfx_PrintStringXY("Off", xEdge - 32, y);
                    #else
                    gfx_PrintStringXY("Off", xEdge - 32, y);
                    #endif
                }
            }

            break;
        case MENU_TYPE_TIME:
            width = 62;

            if (xEdge) {
                #ifdef FR
                if (value) {
                    gfx_PrintStringXY("24 Heure", xEdge - 60, y);
                } else {
                    gfx_PrintStringXY("AM/PM", xEdge - 55, y);
                }
                #else
                if (value) {
                    gfx_PrintStringXY("24 Hour", xEdge - 59, y);
                } else {
                    gfx_PrintStringXY("AM/PM", xEdge - 54, y);
                }
                #endif
            }

            break;
        case MENU_TYPE_SPEED:
            width = 56;

            if (xEdge) {
                switch (value) {
                    case TRANSITION_OFF:
                        #ifdef FR
                        gfx_PrintStringXY("D}s", xEdge - 42, y);
                        #else
                        gfx_PrintStringXY("Off", xEdge - 42, y);
                        #endif
                        break;
                    case TRANSITION_SLOW:
                        #ifdef FR
                        gfx_PrintStringXY("Lente", xEdge - 45, y);
                        #else
                        gfx_PrintStringXY("Slow", xEdge - 45, y);
                        #endif
                        break;
                    case TRANSITION_MED:
                        #ifdef FR
                        gfx_PrintStringXY("Normal", xEdge - 53, y);
                        #else
                        gfx_PrintStringXY("Normal", xEdge - 53, y);
                        #endif
                        break;
                    case TRANSITION_FAST:
                        #ifdef FR
                        gfx_PrintStringXY("Rapide", xEdge - 46, y);
                        #else
                        gfx_PrintStringXY("Fast", xEdge - 46, y);
                        #endif
                        break;
                    default:
                        break;
                }
            }

            break;
        case MENU_TYPE_SCALE:
            width = 19;

            if (xEdge) {
                gfx_SetTextXY(xEdge - 16, y);
                gfx_PrintUInt(value, 0);
            }

            break;
        default:
            break;
    }

    if (xEdge) {
        gfx_SetTextXY(xEdge - 8, y);
        gfx_PrintChar('>');
        gfx_SetTextXY(xEdge - 3 - width, y);
        gfx_PrintChar('<');
    }

    return width;
}

uint8_t menu_CalculateLines(char *string, unsigned int charsPerLine, uint8_t maxLines) {
    unsigned int offset = 0;
    uint8_t lines = 1;
    unsigned int end = 0;

    while (lines <= maxLines) {
        if (strlen(string + offset) <= charsPerLine || lines == maxLines) {
            end = charsPerLine;
        } else {
            end = util_SpaceSearch(string + offset, charsPerLine);
        }

        for (unsigned int byte = 0; byte < end; byte++) {
            if ((string + offset)[byte] == '\0') {
                return lines;
            }
        }

        offset += end;

        if (*(string + offset) == '\0') {
            return lines;
        }

        lines++;
    }

    return lines;
}

void menu_Draw(struct preferences_t *shellPrefs, unsigned int x, uint8_t clipY, int startY, uint8_t width, uint8_t height, struct menu_t *menuContext) {
    uint8_t optionLines;
    uint8_t charsPerLine;

    gfx_SetColor(shellPrefs->bgColor);
    shapes_PixelIndentRectangle(x - 1, clipY - 1, width + 2, height + 2);

    gfx_SetClipRegion(x, clipY, x + width, clipY + height);
    gfx_SetTextConfig(gfx_text_clip);

    ui_ScrollBar(shellPrefs, x + 1, clipY + 1, height - 2, menuContext->totalHeight, abs(startY - clipY - 3), height, false);

    for (int i = 0; i < menuContext->totalOptions; i++) {
        charsPerLine = (width - menu_DrawValueString(0, 0, menuContext->types[i], 0) - 3) / 8;
        optionLines = menu_CalculateLines(menuContext->options[i], charsPerLine, 3);

        if (!(startY > clipY + height)) {
            if (i == menuContext->optionSelected) {
                gfx_SetColor(shellPrefs->hlColor);
                shapes_PixelIndentRectangle(x + 7, startY - 2, width - 8, optionLines * 11 + (optionLines - 1) * 1);
            }

            startY = ui_PrintStringWrap(menuContext->options[i], x + 9, startY, charsPerLine, 3) + 17;
            menu_DrawValueString(x + width, startY - 17, menuContext->types[i], menuContext->values[i]);
        }

    }

    gfx_SetTextConfig(gfx_text_noclip);
    gfx_SetClipRegion(0, 0, 320, 240);

    gfx_SetColor(shellPrefs->fgColor);
    gfx_FillRectangle_NoClip(x + width + 7, clipY - 1, width + 2, height + 2);
    gfx_SetColor(shellPrefs->bgColor);
    shapes_PixelIndentRectangle(x + width + 7, clipY - 1, width + 2, 20 + menu_CalculateLines(menuContext->details[menuContext->optionSelected], 18, 12) * 12);
    #ifdef FR
    gfx_PrintStringXY("A propos :", x + width + 11, clipY + 3);
    #else
    gfx_PrintStringXY("About:", x + width + 11, clipY + 3);
    #endif
    ui_PrintStringWrap(menuContext->details[menuContext->optionSelected], x + width + 11, clipY + 20, 18, 12);
}

bool menu_YesNo(struct preferences_t *shellPrefs, struct context_t *shellContext, unsigned int x, unsigned int optionWidth, char *option1, char *option2) {
    bool retVal = false;

    gfx_SetColor(shellPrefs->hlColor);
    gfx_BlitBuffer();
    gfx_SetDrawScreen();
    shapes_PixelIndentRectangle(x, 209, optionWidth, 11);
    unsigned int optionX = x + 1 + optionWidth / 2;
    gfx_PrintStringXY(option1, optionX - gfx_GetStringWidth(option1) / 2, 211);
    gfx_PrintStringXY(option2, optionX + 3 + optionWidth - gfx_GetStringWidth(option2) / 2, 211);
    gfx_SetDrawBuffer();
    shapes_PixelIndentRectangle(x + 3 + optionWidth, 209, optionWidth, 11);
    gfx_PrintStringXY(option1, optionX - gfx_GetStringWidth(option1) / 2, 211);
    gfx_PrintStringXY(option2, optionX + 3 + optionWidth - gfx_GetStringWidth(option2) / 2, 211);

    bool keyPressed = false;
    clock_t clockOffset = clock();

    while (kb_AnyKey());

    while (!kb_IsDown(kb_KeyClear) && !kb_IsDown(kb_KeyEnter) && !kb_IsDown(kb_Key2nd) && !kb_IsDown(kb_KeyMode)) {
        kb_Scan();
        util_UpdateKeyTimer(shellPrefs, shellContext, &clockOffset, &keyPressed);

        if ((kb_IsDown(kb_KeyLeft) || kb_IsDown(kb_KeyRight)) && (!keyPressed || clock() - clockOffset > CLOCKS_PER_SEC / 32)) {
            gfx_SwapDraw();
            retVal = !retVal;

            if (!keyPressed) {
                while (clock() - clockOffset < CLOCKS_PER_SEC / 3 && (kb_Data[7] || kb_Data[1] || kb_IsDown(kb_KeyEnter))) {
                    kb_Scan();
                }
            }

            keyPressed = true;
            clockOffset = clock();
        }
    }

    return retVal;
}

bool menu_DeleteFile(struct preferences_t *shellPrefs, struct context_t *shellContext, struct file_t *fileInfo) {
    if (fileInfo->shellType == DIR_TYPE) {
        return false;
    }

    gfx_SetColor(shellPrefs->bgColor);
    shapes_RoundRectangleFill(9, 56, 205, 208, 20);
    #ifdef FR
    gfx_PrintStringXY("Are you sure?", 80, 211);
    bool delete = menu_YesNo(shellPrefs, shellContext, 179, 29, "Non", "Oui");
    #else
    gfx_PrintStringXY("Are you sure?", 80, 211);
    bool delete = menu_YesNo(shellPrefs, shellContext, 179, 29, "No", "Yes");
    #endif

    gfx_SetColor(shellPrefs->fgColor);
    gfx_FillRectangle_NoClip(56, 205, 208, 20);

    if ((kb_IsDown(kb_KeyEnter) || kb_IsDown(kb_Key2nd)) && delete) {
        if (fileInfo->shellType == APP_TYPE) {
            asm_apps_deleteApp(shellContext->appPtrs[shellContext->fileSelected - 1]);
        } else {
            ti_DeleteVar(fileInfo->name, fileInfo->type);
        }

        return true;
    }

    while (kb_AnyKey());
    return false;
}

bool menu_RenameFile(struct preferences_t *shellPrefs, struct context_t *shellContext, struct file_t *fileInfo) {
    gfx_SetColor(shellPrefs->bgColor);
    shapes_RoundRectangleFill(9, 56, 205, 208, 20);
    #ifdef FR
    gfx_PrintStringXY("New", 69, 207);
    gfx_PrintStringXY("Name:", 64, 216);
    #else
    gfx_PrintStringXY("New", 69, 207);
    gfx_PrintStringXY("Name:", 64, 216);
    #endif
    char *newName = ui_StringInput(shellPrefs, shellContext, 104, 208, false);

    gfx_SetColor(shellPrefs->fgColor);
    gfx_FillRectangle_NoClip(56, 205, 208, 20);

    if (newName != NULL) {
        ti_RenameVar(fileInfo->name, newName, fileInfo->type);
        free(newName);
        util_WritePrefs(shellPrefs, shellContext, true);
        return true;
    }

    free(newName);
    while (kb_AnyKey());
    return false;
}

void menu_CopyFile(struct preferences_t *shellPrefs, struct context_t *shellContext, struct file_t *fileInfo) {
    unsigned int *fileCount = &(shellContext->programCount) + shellContext->directory;

    if (fileInfo->shellType == DIR_TYPE) {
        return;
    }

    gfx_SetColor(shellPrefs->bgColor);
    shapes_RoundRectangleFill(9, 56, 205, 208, 20);
    #ifdef FR
    bool createNew = menu_YesNo(shellPrefs, shellContext, 80, 79, "Copy", "Create New");
    #else
    bool createNew = menu_YesNo(shellPrefs, shellContext, 80, 79, "Copy", "Create New");
    #endif

    if (kb_IsDown(kb_KeyClear) || kb_IsDown(kb_KeyMode)) {
        while (kb_AnyKey());
        return;
    }

    gfx_SetColor(shellPrefs->bgColor);
    shapes_RoundRectangleFill(9, 56, 205, 208, 20);
    #ifdef FR
    gfx_PrintStringXY("New", 69, 207);
    gfx_PrintStringXY("Name:", 64, 216);
    #else
    gfx_PrintStringXY("New", 69, 207);
    gfx_PrintStringXY("Name:", 64, 216);
    #endif
    char *newName = ui_StringInput(shellPrefs, shellContext, 104, 208, false);
    gfx_SetColor(shellPrefs->fgColor);
    gfx_FillRectangle_NoClip(56, 205, 208, 20);

    if (newName != NULL) {
        if (createNew) {
            uint8_t slot = ti_OpenVar(newName, "w+", OS_TYPE_PRGM + (OS_TYPE_APPVAR - OS_TYPE_PRGM) * (shellContext->directory == APPVARS_FOLDER));

            if (!slot || !asm_utils_checkEnoughRAM(16)) {
                while (kb_AnyKey());
                return;
            }

            if (shellContext->directory == APPVARS_FOLDER) { // Add Celtic header
                ti_Write(&rodata_celticAppVarHeader, 5, 1, slot);
            }

            ti_Close(slot);
            util_SearchToMain(shellPrefs, shellContext);
            while (kb_AnyKey());
            gfx_End();
            asm_editProgram_edit(newName, shellContext->directory == APPVARS_FOLDER, shellPrefs);
        } else {
            asm_fileOps_copyFile(fileInfo->name, newName, fileInfo->type);
            *fileCount += 1;
            util_WritePrefs(shellPrefs, shellContext, true);
        }
    }

    while (kb_AnyKey());
}

void menu_AboutScreen(struct preferences_t *shellPrefs, struct context_t *shellContext) {
    #ifdef FR
    const char *specialThanks = "Special Thanks To: Code/Coding Help: MateoConLechuga, calc84maniac, commandblockguy, jacobly, Zeroko, and the CEdev Discord."
    " French translation: Shadow. Inspiration/Feature Ideas: KermMartian, Adriweb, epsilon5, NoahK,"
    " Dream of Omnimaga. Beta Testing: Nanobot567, ChuckyHecker, darkwater4213, Oxiti8, LogicalJoe, Calculatordream."
    " And a big thank you to the members of our Discord for your support and ideas!";
    #else
    const char *specialThanks = "Special Thanks To: Code/Coding Help: MateoConLechuga, calc84maniac, commandblockguy, jacobly, Zeroko, and the CEdev Discord."
    " French translation: Shadow. Inspiration/Feature Ideas: KermMartian, Adriweb, epsilon5, NoahK,"
    " Dream of Omnimaga. Beta Testing: Nanobot567, ChuckyHecker, darkwater4213, Oxiti8, LogicalJoe, Calculatordream."
    " And a big thank you to the members of our Discord for your support and ideas!";
    #endif
    unsigned int startDisplay = 0;
    gfx_SetColor(shellPrefs->bgColor);
    shapes_PixelIndentRectangle(14, 34, 292, 170);
    ui_CenterStringBig(&rodata_appName, 75, 38);
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
        "aims to provide an easy to use, modern interface "
        "for the TI-84+ CE and TI-83 PCE calculators.", 18, 75, 42, 6);
    #endif
    gfx_PrintStringXY("(C) Copyright 2022 - 2024", 18, 179);
    gfx_PrintStringXY("RoccoLox Programs, TIny_Hacker", 18, 192);
    gfx_SetClipRegion(18, 38, 302, 202); // Helps cut the text off in the scrolling animation
    gfx_SetTextConfig(1);
    gfx_PrintStringXY(&specialThanks[startDisplay], 18, 144);
    gfx_BlitBuffer();

    clock_t clockOffset = clock();
    clock_t textRate = clock();

    while (!kb_IsDown(kb_KeyClear) && !kb_IsDown(kb_KeyAlpha) && !kb_IsDown(kb_KeyGraph) && !(clock() - clockOffset > 9000)) {
        kb_Scan();
    }

    while (!kb_IsDown(kb_KeyClear) && !kb_IsDown(kb_KeyAlpha) && !kb_IsDown(kb_KeyGraph)) {
        kb_Scan();

        if ((clock() - clockOffset >= ONE_MINUTE * shellPrefs->apdTimer && shellPrefs->apdTimer) || kb_On) {
            util_PowerOff(shellPrefs, shellContext);
        }

        if (clock() - textRate > CLOCKS_PER_SEC / 16) {
            gfx_SetColor(shellPrefs->bgColor);
            gfx_FillRectangle_NoClip(18, 144, 284, 8);
            gfx_PrintStringXY(&specialThanks[startDisplay], 18, 144);
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
