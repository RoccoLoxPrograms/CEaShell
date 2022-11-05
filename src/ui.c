/**
 * --------------------------------------
 * 
 * CEaShell Source Code - ui.c
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#include "ui.h"
#include "shapes.h"
#include "utility.h"
#include "asm/fileOps.h"
#include "gfx/gfx.h"

#include <graphx.h>
#include <string.h>
#include <fileioc.h>
#include <keypadc.h>
#include <sys/rtc.h>
#include <sys/timers.h>
#include <sys/power.h>
#include <ti/getcsc.h>

void ui_DrawUISprite(const uint8_t color, const uint8_t spriteNo, const int x, const uint8_t y) {   // Takes care of drawing the sprite in white or black, depending on the theme
    bool colorAlt = !(color > 131 && color % 8 > 3);
    const gfx_sprite_t *uiIcons[22] = {battery, charging, paint, info, settings, lArrow, rArrow, dArrow, check, cursorAlpha, cursorNumber, batteryAlt, chargingAlt, paintAlt, infoAlt, settingsAlt, lArrowAlt, rArrowAlt, dArrowAlt, checkAlt, cursorAlphaAlt, cursorNumberAlt};
    gfx_TransparentSprite_NoClip(uiIcons[spriteNo + colorAlt * 11], x, y);
    gfx_SetTextFGColor(colorAlt * 255);
}

void ui_DrawFile(const bool selected, const bool drawName, const bool drawHidden, const bool hidden, uint8_t *colors, char *fileName, const uint8_t fileType, const uint8_t osFileType, const int x, const uint8_t y) {  // Draws a file, with the icon if it exists
    const bool colorAlt = (colors[1] > 131 && colors[1] % 8 > 3);
    gfx_sprite_t *icon = gfx_MallocSprite(16, 16);  // Malloc the sprite ahead of time
    gfx_sprite_t *tileSprite = gfx_MallocSprite(16, 16);
    if (hidden) {
        shapes_GetTransparentRect(tileSprite, colors[(selected)], x, y);
    }
    if (fileType != DIR_TYPE) {
        fileName[0] -= 64 * hidden;
    }

    if (selected) {
        shapes_RoundRectangleFill(colors[1], 6, 68, 78, x - 2, y - 2);
    }
    if (fileType == DIR_TYPE) {
        shapes_RoundRectangleFill(colors[2], 4, 64, 64, x, y);
        gfx_SetColor(255 * colorAlt);
        gfx_FillRectangle_NoClip(28, 52, 36, 23);
        shapes_Folder(255 * !colorAlt, colors[2], x + 11, y + 16);
    } else if (fileType != ICE_SRC_TYPE && fileType != BASIC_TYPE && getIconASM(fileName, osFileType, fileType, icon)) {
        gfx_sprite_t *corner1 = gfx_MallocSprite(4, 4); // Round the corners of the icon to match with the file icons
        shapes_GetRoundCorners(corner1, colors[(selected)], 4, x, y);
        gfx_ScaledSprite_NoClip(icon, x, y, 4, 4);
        shapes_DrawRoundCorners(corner1, 64, 64, x, y);
        free(corner1);
    } else if (fileType == BASIC_TYPE && getIconDCS(fileName, osFileType, icon)) { // Possible optimizations with this bit and the above if statement?
        gfx_sprite_t *corner1 = gfx_MallocSprite(4, 4);
        shapes_GetRoundCorners(corner1, colors[(selected)], 4, x, y);
        gfx_ScaledSprite_NoClip(icon, x, y, 4, 4);
        shapes_DrawRoundCorners(corner1, 64, 64, x, y);
        free(corner1);
    } else {
        shapes_RoundRectangleFill(colors[2], 4, 64, 64, x, y);  // If there isn't an icon we'll draw our own default file icon
        gfx_SetColor(255 * colorAlt);
        gfx_FillRectangle(x + 19, y + 14, 29, 39);
        shapes_FileIcon(255 * !colorAlt, colors[2], x + 16, y + 11);
        gfx_SetColor(255 * !colorAlt);
        gfx_FillRectangle(x + 19, y + 36, 28, 9);
        gfx_SetTextScale(1, 1);
        char *fileTypeString = util_FileTypeToString(fileType, true);
        gfx_SetTextFGColor(255 * colorAlt);
        gfx_SetTextScale(1, 1);
        uint8_t typeLength = gfx_GetStringWidth(fileTypeString);
        if (typeLength) {
            gfx_PrintStringXY(fileTypeString, x + (64 - typeLength) / 2, y + 37);
        } else {
            gfx_PrintStringXY("?", x + 29, y + 37);
        }
        gfx_SetTextFGColor(255 * !colorAlt);
    }
    
    free(icon);    // We do not need this anymore
    if (fileType != DIR_TYPE) {
        fileName[0] += 64 * hidden; // Make the hidden name viewable (If it is hidden)
    }
    gfx_SetTextScale(1, 1);
    if (drawName) { // In some places we do not draw the name
        uint8_t stringLength = gfx_GetStringWidth(fileName);
        if (stringLength) {
            gfx_PrintStringXY(fileName, x + (64 - stringLength) / 2, y + 67);
        }
    }

    if (drawHidden && hidden) {   // Hidden effect
        shapes_DrawTransparentRect(tileSprite, x, y);
    }
    free(tileSprite);
}

void ui_CheckBox(const uint8_t color, const uint8_t bgColor, const bool isChecked, const int x, const uint8_t y) {    // Draws a simple checkbox
    bool colorAlt = !(bgColor > 131 && bgColor % 8 > 3);
    if (colorAlt) {
        gfx_SetColor(148);
    } else {
        gfx_SetColor(181);
    }
    gfx_FillRectangle_NoClip(x, y, 7, 7);
    gfx_SetColor(bgColor);
    gfx_SetPixel(x, y);
    gfx_SetPixel(x, y + 6);
    gfx_SetPixel(x + 6, y + 6);
    gfx_SetPixel(x + 6, y);
    if (isChecked) {
        ui_DrawUISprite(color, UI_CHECK, x + 1, y - 1);
    }
}

void ui_Clock(const bool is24Hour) {    // Displays time in either 24-Hour or AM/PM
    uint8_t time[3];
    bool isAfterNoon = boot_IsAfterNoon();

    boot_GetTime(&time[0], &time[1], &time[2]);
    time[2] -= 12 * (!is24Hour && isAfterNoon);  // Add 12 to the hour if it's PM and 24-Hour Time, so that 1 PM becomes 13, etc.
    if (!time[2] && !is24Hour) {
        time[2] = 12;
    }
    gfx_SetTextScale(1, 1);
    gfx_SetTextXY(15, 12);
    gfx_PrintUInt(time[2], 1);
    gfx_PrintString(":");
    gfx_PrintUInt(time[1], 2);
    if (!is24Hour) {    // This draws the AM/PM if 24-Hour Time is not enabled
        if (isAfterNoon) {
            gfx_PrintString(" PM");
        } else {
            gfx_PrintString(" AM");
        }
    }
}

void ui_Battery(const uint8_t color, const uint8_t batteryStatus, const bool isCharging) {
    ui_DrawUISprite(color, UI_BATTERY, 286, 10);

    gfx_SetColor((255 * !(color > 131 && color % 8 > 3)) * (batteryStatus > 1) + 160 * (batteryStatus < 2));
    gfx_FillRectangle_NoClip(291 + (12 - batteryStatus * 3), 12, batteryStatus * 3, 6);

    if (isCharging) {
        ui_DrawUISprite(color, UI_CHARGING, 292, 12);  // Charging sprite instead of battery percentage
    }
}

void ui_StatusBar(const uint8_t color, const bool is24Hour, const uint8_t batteryStatus, const char *menuName, const unsigned int fileCount, const bool showFileCount) {  // Draws a 308 pixel long bar with rounded edges at 6, 6
    gfx_SetColor(color);
    shapes_RoundRectangleFill(color, 8, 308, 18, 6, 6);

    ui_Clock(is24Hour);
    ui_Battery(color, batteryStatus, boot_BatteryCharging());

    gfx_SetTextScale(2, 2);
    const int x = 160 - gfx_GetStringWidth(menuName) / 2;
    gfx_PrintStringXY(menuName, x, 8);
    if (showFileCount) {
        gfx_SetTextScale(1, 1);
        gfx_SetTextXY(250, 12);
        gfx_PrintInt(fileCount, 4);
    }
}

void ui_DescriptionWrap(const char *description, const uint8_t charPerLine, const int x, const uint8_t y) {
    gfx_SetTextScale(1, 1); // Description
    char lineOne[30] = "\0";
    char lineTwo[30] = "\0";
    if (strlen(description) > charPerLine) { // Wraps text that is longer than 24, and cuts off anything longer than 48
        int8_t cut = util_SpaceSearch(description, charPerLine); // If there is a space it will end the line there
        int8_t descLen = strlen(description);
        strncpy(lineOne, description, cut);
        if (descLen - cut > charPerLine - 1) {
            strncpy(lineTwo, description + cut, charPerLine - 3);
        } else {
            strncpy(lineTwo, description + cut, charPerLine - 1);
        }
        gfx_PrintStringXY(lineOne, x, y);
        gfx_PrintStringXY(lineTwo, x, y + 11);
        if (descLen - cut > charPerLine - 1) {
            gfx_PrintString("...");
        }
    } else {
        gfx_PrintStringXY(description, x, y + 5);
    }
}

void ui_BottomBar(const uint8_t color) {
    shapes_RoundRectangleFill(color, 6, 34, 34, 8, 197);    // Background and sprite
    shapes_RoundRectangleFill(color, 15, 220, 32, 50, 198);
    shapes_RoundRectangleFill(color, 6, 34, 34, 278, 197);
    ui_DrawUISprite(color, UI_PAINT, 14, 203);
    ui_DrawUISprite(color, UI_INFO, 56, 204);
    ui_DrawUISprite(color, UI_SETTINGS, 284, 203);
}

bool ui_DeleteConf(uint8_t *colors, const int x, const uint8_t y) {
    gfx_SetDrawBuffer();
    bool retVal = false;
    while (kb_AnyKey());
    shapes_RoundRectangleFill(colors[0], 8, 208, 20, x, y);
    gfx_PrintStringXY("Are you sure?", x + 28, y + 6);
    gfx_BlitBuffer();
    gfx_SetColor(colors[2]);
    shapes_PixelIndentRectangle(colors[2], colors[0], x + 159, y + 4, 29, 11);
    gfx_PrintStringXY("No", x + 134, y + 6);
    gfx_PrintStringXY("Yes", x + 162, y + 6);
    gfx_SetDrawScreen();
    shapes_PixelIndentRectangle(colors[2], colors[0], x + 127, y + 4, 29, 11);
    gfx_PrintStringXY("No", x + 134, y + 6);
    gfx_PrintStringXY("Yes", x + 162, y + 6);
    gfx_SetDrawBuffer();
    while (!kb_IsDown(kb_Key2nd) && !kb_IsDown(kb_KeyEnter) && !kb_IsDown(kb_KeyClear) && !kb_IsDown(kb_Key1) && !kb_IsDown(kb_KeyLog) && !kb_IsDown(kb_KeyZoom) && !kb_IsDown(kb_KeyGraph)) {
        kb_Scan();
        if (kb_IsDown(kb_KeyLeft) || kb_IsDown(kb_KeyRight)) {
            gfx_SwapDraw();
            retVal = !retVal;
            while (kb_AnyKey());
        }
        if (kb_IsDown(kb_Key1) || kb_IsDown(kb_KeyZoom)) {
            retVal = true;
        } else if (kb_IsDown(kb_KeyLog) || kb_IsDown(kb_KeyGraph)) {
            retVal = false;
        }
    }
    if (kb_IsDown(kb_KeyClear)) {
        return false;
    }
    shapes_RoundRectangleFill(colors[1], 8, 208, 20, x, y);
    return retVal;
}

bool ui_RenameBox(uint8_t *colors, char *newName) {
    gfx_BlitScreen();
    const char *charAlpha = "\0\0\0\0\0\0\0\0\0\0\0WRMH\0\0\0[VQLG\0\0\0ZUPKFC\0\0YTOJEBX\0XSNIDA\0\0\0\0\0\0\0\0";
    const char *charNums = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0""369\0\0\0\0\0""258\0\0\0\0""0147\0\0\0X\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
    uint8_t length = strlen(newName);
    gfx_SetColor(colors[0]);
    gfx_FillRectangle_NoClip(129, 182, 62, 11);
    gfx_PrintStringXY("Rename", 136, 184);
    gfx_SetTextScale(2, 2);
    uint8_t xOffset = gfx_GetStringWidth(newName) / 2;
    gfx_SetColor(colors[2]);
    gfx_FillRectangle_NoClip(60, 52, 129, 14);
    gfx_PrintStringXY(newName, 121 - xOffset, 52);
    gfx_BlitBuffer();
    while (kb_AnyKey());
    uint8_t key = os_GetCSC();
    bool redraw;
    bool alphaPressed;  // Number mode?
    bool cursor = false;
    timer_Set(1, 0);
    while (key != sk_Clear && key != sk_Window && key != sk_Zoom && key != sk_Trace) {
        key = os_GetCSC();
        if (key == sk_Clear) {
            continue;
        }
        if (key) {
            if ((key == sk_Left || key == sk_Del) && length) {
                length -= 1;
                newName[length] = '\0';
                redraw = true;
            }
            if (!alphaPressed) {
                if (charAlpha[key] && length < 8) {
                    newName[length++] = charAlpha[key];
                    redraw = true;
                }
            } else {
                if (charNums[key] && length < 8 && length) {
                    newName[length++] = charNums[key];
                    redraw = true;
                }
            }
            if (redraw) {
                redraw = false;
                shapes_RoundRectangleFill(colors[2], 8, 138, 30, 56, 44);
                xOffset = gfx_GetStringWidth(newName) / 2;
                gfx_PrintStringXY(newName, 121 - xOffset, 52);
                gfx_BlitBuffer();
            }
            if (key == sk_Enter || key == sk_2nd) {
                if (length) {
                    break;
                }
            }
            if (key == sk_Alpha) {
                alphaPressed = !alphaPressed;
            }
        } else {
            if (cursor) {
                ui_DrawUISprite(colors[1], UI_CURSOR_A + alphaPressed, gfx_GetTextX() + 1, gfx_GetTextY());
            } else {
                gfx_SetColor(colors[2]);
                gfx_FillRectangle_NoClip(gfx_GetTextX() + 1, gfx_GetTextY(), 7, 14);
            }
            gfx_BlitBuffer();
            while (timer_Get(1) < 5000);
            timer_Set(1, 0);
            cursor = !cursor;
        }
    }
    gfx_SetTextScale(1, 1);
    if (key == sk_Enter || key == sk_2nd) {
        while (kb_AnyKey());
        return true;
    } else {
        return false;
    }
}

void ui_DrawAllFiles(uint8_t *colors, const uint8_t fileSelected, const uint8_t fileCount, const unsigned int fileStartLoc, const bool appvars, const bool displayCEaShell, const bool showHiddenProg) {
    int x = 14;
    uint8_t y = 30;
    unsigned int filesSearched = 0;

    uint8_t fileType;
    uint8_t shellFileType;
    bool hidden;
    char *fileName;
    void *vatPtr = NULL;
    if (fileStartLoc == 0) {
        if (appvars) {
            fileName = "Programs";
        } else {
            fileName = "Appvars";
        }
        ui_DrawFile((fileSelected == filesSearched), true, false, false, colors, fileName, DIR_TYPE, 0, 14, 30);
        y = 116;
        if (fileSelected == filesSearched) {
            gfx_PrintStringXY(fileName, 82, 210);
            gfx_PrintString(" folder");
            gfx_SetTextScale(2, 2);
            gfx_SetColor(colors[1]);
            uint8_t textX = 160 - gfx_GetStringWidth(fileName) / 2;
            gfx_PrintStringXY(fileName, textX, 8);
        }
    }
    filesSearched++;    // We have "Checked" for the folder
    while ((fileName = ti_DetectAny(&vatPtr, NULL, &fileType))) {
        if (*fileName == '!' || *fileName =='#') {  // We skip these two OS files
            continue;
        }
        if (!displayCEaShell && !strcmp(fileName, "CEASHELL")) {
            continue;
        }
        if (!showHiddenProg && fileName[0] < 65) {
            continue;
        }
        if ((fileType == OS_TYPE_PRGM || fileType == OS_TYPE_PROT_PRGM) && getPrgmType(fileName, fileType) == HIDDEN_TYPE) {
            continue;
        }
        hidden = (fileName[0] < 65);
        if (!appvars && (fileType == OS_TYPE_PRGM || fileType == OS_TYPE_PROT_PRGM)) {
            if (fileStartLoc <= filesSearched) {
                shellFileType = getPrgmType(fileName, fileType);
                fileName[0] += 64 * hidden;
                ui_DrawFile((fileSelected == filesSearched), true, true, hidden, colors, fileName, shellFileType, fileType, x, y);
                if (y == 30) {
                    y = 116;
                } else {
                    x += 76;
                    y = 30;
                }
                if (fileSelected == filesSearched) {    // Draws the name of the selected program
                    char *description = malloc(52);
                    fileName[0] -= 64 * hidden;
                    if (shellFileType != BASIC_TYPE && shellFileType != ICE_SRC_TYPE && getDescASM(fileName, fileType, shellFileType, description)) {
                        ui_DescriptionWrap(description, 24, 82, 205);
                    } else if (shellFileType == BASIC_TYPE && getDescBASIC(fileName, fileType, description)) {
                        ui_DescriptionWrap(description, 24, 82, 205);
                    } else {
                        util_PrintFreeRamRom();
                    }
                    free(description);
                    fileName[0] += 64 * hidden;
                    gfx_SetTextScale(2, 2);
                    gfx_SetColor(colors[1]);
                    uint8_t textX = 160 - gfx_GetStringWidth(fileName) / 2;
                    gfx_PrintStringXY(fileName, textX, 8);
                }
            }
            filesSearched++;
        } else if (appvars && fileType == OS_TYPE_APPVAR) {
            if (fileStartLoc <= filesSearched) {
                ui_DrawFile((fileSelected == filesSearched), true, true, hidden, colors, fileName, APPVAR_TYPE, OS_TYPE_APPVAR, x, y);
                if (y == 30) {
                    y = 116;
                } else {
                    x += 76;
                    y = 30;
                }
            }
            if (fileSelected == filesSearched) {
                util_PrintFreeRamRom();
                gfx_SetTextScale(2, 2);
                gfx_SetColor(colors[1]);
                uint8_t textX = 160 - gfx_GetStringWidth(fileName) / 2;
                gfx_PrintStringXY(fileName, textX, 8);
            }
            filesSearched++;
        }

        if ((x > 242) || filesSearched > fileCount) {
            break;
        }
    }
}

void ui_MiniCursor(uint8_t color, int x, uint8_t y) {
    gfx_SetColor(color);
    gfx_Line_NoClip(x, y + 1, x, y + 4);
    gfx_Line_NoClip(x + 1, y, x + 4, y);
    gfx_Line_NoClip(x + 1, y + 5, x + 4, y + 5);
    gfx_Line_NoClip(x + 5, y + 1, x + 5, y + 4);
    gfx_SetPixel(x + 1, y + 1);
    gfx_SetPixel(x + 4, y + 4);
    gfx_SetPixel(x + 1, y + 4);
    gfx_SetPixel(x + 4, y + 1);
}

void ui_DrawMenuItem(const char *lineOne, const int x, const uint8_t y, const bool status) {
    gfx_PrintStringXY(lineOne, x, y);
    gfx_PrintStringXY("<", x + 92, y);
    gfx_PrintStringXY(">", x + 122, y);
    if (status) {
        gfx_PrintStringXY("On", x + 102, y);
    } else {
        gfx_PrintStringXY("Off", x + 99, y);
    }
}

void ui_DrawDoubleMenuItem(const char *lineOne, const char *lineTwo, const int x, const uint8_t y, const bool status) {
    gfx_PrintStringXY(lineOne, x, y);
    gfx_PrintStringXY(lineTwo, x, y + 12);
    gfx_PrintStringXY("<", x + 92, y + 12);
    gfx_PrintStringXY(">", x + 122, y + 12);
    if (status) {
        gfx_PrintStringXY("On", x + 102, y + 12);
    } else {
        gfx_PrintStringXY("Off", x + 99, y + 12);
    }
}

void ui_AboutScreen(uint8_t *colors) {
    const char *specialThanks = "Special Thanks To: Code/Coding Help: MateoConLechuga, calc84maniac, commandblockguy, jacobly, Zeroko, and the CEdev Discord."
    " Inspiration/Feature Ideas: KermMartian, Adriweb, epsilon5, NoahK, DJ Omnimaga. Beta Testing: Nanobot567, ChuckyHecker, darkwater4213, Oxiti8."
    "And a big thank you to the members of the discord for your support and ideas!";
    unsigned int startDisplay = 0;
    shapes_RoundRectangleFill(colors[0], 8, 290, 155, 15, 46);
    gfx_SetTextScale(2, 2);
    gfx_PrintStringXY("CEaShell", 21, 52);
    gfx_SetTextScale(1, 1);
    gfx_SetTextXY(21, 72);
    gfx_PrintChar('v');
    gfx_PrintString(VERSION_NO);
    gfx_PrintStringXY("CEaShell (pronounced \"seashell\"), is a", 21, 89);
    gfx_PrintStringXY("shell created by RoccoLox Programs and", 21, 102);
    gfx_PrintStringXY("TIny_Hacker. CEaShell aims to provide an", 21, 115);
    gfx_PrintStringXY("easy to use, modern interface for the", 21, 128);
    gfx_PrintStringXY("TI-84 + CE calculator.", 21, 141);
    gfx_PrintStringXY("(C) Copyright 2022", 21, 175);
    gfx_PrintStringXY("RoccoLox Programs, TIny_Hacker", 21, 188);
    gfx_SetClipRegion(21, 52, 299, 196);
    gfx_SetTextConfig(1);
    gfx_PrintStringXY(&specialThanks[startDisplay], 21, 158);
    gfx_BlitBuffer();
    timer_Set(1, 0);
    while (!kb_IsDown(kb_KeyClear) && !kb_IsDown(kb_KeyAlpha) && !kb_IsDown(kb_KeyGraph) && !(timer_Get(1) > 9000)) {
        kb_Scan();
    }
    while (!kb_IsDown(kb_KeyClear) && !kb_IsDown(kb_KeyAlpha) && !kb_IsDown(kb_KeyGraph)) {
        kb_Scan();
        if (timer_Get(1) > 2500) {
            gfx_SetColor(colors[0]);
            gfx_FillRectangle_NoClip(21, 158, 278, 8);
            gfx_PrintStringXY(&specialThanks[startDisplay], 21, 158);
            startDisplay++;
            gfx_BlitBuffer();
            timer_Set(1, 0);
        }
        if (startDisplay > 343) {
            startDisplay = 0; // restart
        }
    }
    gfx_SetClipRegion(0, 0, 320, 240);
    gfx_SetTextConfig(0);
}

void ui_NewUser(void) {
    gfx_FillScreen(246);
    shapes_RoundRectangleFill(237, 6, 272, 44, 24, 22);
    gfx_SetTextScale(2, 2);
    gfx_PrintStringXY("Welcome to CEaShell", 29, 27);
    gfx_SetTextScale(1, 1);
    gfx_PrintStringXY("v"VERSION_NO, 29, 44);
    gfx_PrintStringXY("A new shell for the TI-84 Plus CE.", 29, 54);
    shapes_RoundRectangleFill(237, 6, 272, 143, 24, 71);
    shapes_RoundRectangleFill(236, 7, 86, 22, 206, 188);
    gfx_PrintStringXY("Alpha to", 211, 191);
    gfx_PrintStringXY("Continue", 211, 199);
    gfx_TransparentSprite_NoClip(rArrow, 273, 191);
    gfx_PrintStringXY("We try our best to make CEaShell as", 29, 76);
    gfx_PrintStringXY("user-friendly as possible. Here's a", 29, 88);
    gfx_PrintStringXY("quick guide to navigating the features", 29, 100);
    gfx_PrintStringXY("and menus.", 29, 112);
    gfx_PrintStringXY("Use the function keys to open the menu", 29, 130);
    gfx_PrintStringXY("buttons on the bottom row. You can open", 29, 142);
    gfx_PrintStringXY("the description button for more info", 29, 154);
    gfx_PrintStringXY("on a file using the function keys or", 29, 166);
    gfx_PrintStringXY("[alpha].", 29, 180);
    gfx_BlitBuffer();
    while (!kb_IsDown(kb_KeyClear) && !kb_IsDown(kb_KeyAlpha) && !kb_IsDown(kb_Key2nd) && !kb_IsDown(kb_KeyEnter) && !kb_IsDown(kb_KeyRight)) {
        kb_Scan();
    }
    if (kb_IsDown(kb_KeyClear)) {
        return;
    }
    gfx_SetColor(237);
    gfx_FillRectangle_NoClip(29, 76, 266, 112);
    gfx_PrintStringXY("Navigate through menus and files", 29, 76);
    gfx_PrintStringXY("using the arrow keys.", 29, 90);
    gfx_PrintStringXY("Run programs using [enter] or [2nd].", 29, 108);
    gfx_PrintStringXY("Toggle options in the settings", 29, 126);
    gfx_PrintStringXY("using the arrow keys, and check", 29, 140);
    gfx_PrintStringXY("boxes with [2nd] or [enter].", 29, 154);
    gfx_PrintStringXY("Thanks for using CEaShell!", 29, 172);
    gfx_BlitBuffer();
    while(kb_AnyKey());
    while (!kb_IsDown(kb_KeyClear) && !kb_IsDown(kb_KeyAlpha) && !kb_IsDown(kb_Key2nd) && !kb_IsDown(kb_KeyEnter) && !kb_IsDown(kb_KeyRight)) {
        kb_Scan();
    }
}

uint8_t ui_CopyNewMenu(uint8_t *colors, char *name) {
    bool returnVal = false;
    while (kb_AnyKey());
    shapes_RoundRectangleFill(colors[0], 9, 208, 20, 56, 204);
    gfx_BlitBuffer();
    shapes_PixelIndentRectangle(colors[2], colors[0], 166, 208, 85, 11);
    gfx_PrintStringXY("Copy File", 77, 210);
    gfx_PrintStringXY("New File", 179, 210);
    gfx_SetDrawScreen();
    shapes_PixelIndentRectangle(colors[2], colors[0], 68, 208, 85, 11);
    gfx_PrintStringXY("Copy File", 77, 210);
    gfx_PrintStringXY("New File", 179, 210);
    gfx_SetDrawBuffer();
    while(!kb_IsDown(kb_Key2nd) && !kb_IsDown(kb_KeyEnter)) {
        kb_Scan();
        if (kb_IsDown(kb_KeyClear)) {
            return 2;
        }
        if (kb_IsDown(kb_KeyMode)) {
            return 3;
        }
        if (kb_IsDown(kb_KeyLeft) || kb_IsDown(kb_KeyRight)) {
            gfx_SwapDraw();
            returnVal = !returnVal;
            while(kb_AnyKey());
        }
    }
    while (kb_AnyKey());
    gfx_SetColor(colors[0]);
    gfx_FillRectangle_NoClip(68, 207, 183, 14);
    gfx_PrintStringXY("Name:", 73, 211);
    gfx_SetTextScale(2, 2);
    gfx_SetTextXY(111, 207);
    gfx_BlitBuffer();
    const char *charAlpha = "\0\0\0\0\0\0\0\0\0\0\0WRMH\0\0\0[VQLG\0\0\0ZUPKFC\0\0YTOJEBX\0XSNIDA\0\0\0\0\0\0\0\0";
    const char *charNums = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0""369\0\0\0\0\0""258\0\0\0\0""0147\0\0\0X\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
    asm("ei");
    uint8_t length = 0;
    uint8_t key = os_GetCSC();
    bool redraw;
    bool alphaPressed;  // Number mode?
    bool cursor = false;
    timer_Set(1, 0);
    while (key != sk_Clear && key != sk_Mode) {
        key = os_GetCSC();
        if (key == sk_Clear || key == sk_Mode) {
            continue;
        }
        if (key) {
            if ((key == sk_Left || key == sk_Del) && length) {
                length -= 1;
                name[length] = '\0';
                redraw = true;
            }
            if (!alphaPressed) {
                if (charAlpha[key] && length < 8) {
                    name[length++] = charAlpha[key];
                    redraw = true;
                }
            } else {
                if (charNums[key] && length < 8 && length) {
                    name[length++] = charNums[key];
                    redraw = true;
                }
            }
            if (redraw) {
                redraw = false;
                gfx_SetColor(colors[0]);
                gfx_FillRectangle_NoClip(111, 207, 136, 14);
                gfx_PrintStringXY(name, 111, 207);
                gfx_BlitBuffer();
            }
            if (key == sk_Enter || key == sk_2nd) {
                if (length) {
                    break;
                }
            }
            if (key == sk_Alpha) {
                alphaPressed = !alphaPressed;
            }
        } else {
            if (cursor) {
                ui_DrawUISprite(colors[1], UI_CURSOR_A + alphaPressed, gfx_GetTextX() + 1, gfx_GetTextY());
            } else {
                gfx_SetColor(colors[0]);
                gfx_FillRectangle_NoClip(gfx_GetTextX() + 1, gfx_GetTextY(), 7, 14);
            }
            gfx_BlitBuffer();
            while (timer_Get(1) < 5000);
            timer_Set(1, 0);
            cursor = !cursor;
        }
    }
    asm("di");
    gfx_SetTextScale(1, 1);
    if (key == sk_Enter || key == sk_2nd) {
        while (kb_AnyKey());
        return returnVal;
    } else {
        return 2;
    }
    return returnVal;
}
