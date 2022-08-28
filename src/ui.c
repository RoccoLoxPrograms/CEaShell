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

void ui_DrawUISprite(uint8_t color, uint8_t spriteNo, int x, uint8_t y) {   // Takes care of drawing the sprite in white or black, depending on the theme
    bool colorAlt = !(color > 131 && color % 8 > 3);
    const gfx_sprite_t *uiIcons[22] = {battery, charging, paint, info, settings, lArrow, rArrow, dArrow, check, cursorAlpha, cursorNumber, batteryAlt, chargingAlt, paintAlt, infoAlt, settingsAlt, lArrowAlt, rArrowAlt, dArrowAlt, checkAlt, cursorAlphaAlt, cursorNumberAlt};
    gfx_TransparentSprite_NoClip(uiIcons[spriteNo + colorAlt * 11], x, y);
    gfx_SetTextFGColor(colorAlt * 255);
}

void ui_DrawFile(bool selected, bool drawName, bool drawHidden, bool hidden, uint8_t *colors, char *fileName, uint8_t fileType, uint8_t osFileType, int x, uint8_t y) {  // Draws a file, with the icon if it exists
    bool colorAlt = (colors[1] > 131 && colors[1] % 8 > 3);
    gfx_sprite_t *icon = gfx_MallocSprite(16, 16);  // Malloc the sprite ahead of time
    gfx_sprite_t *tileSprite = gfx_MallocSprite(16, 16);
    if (hidden) {
        shapes_GetTransparentRect(tileSprite, colors[(selected)], x, y);
    }
    fileName[0] -= 64 * hidden;

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
        free (corner1);
    } else if (fileType == BASIC_TYPE && getIconDCS(fileName, osFileType, icon)) { // Possible optimizations with this bit and the above if statement?
        gfx_sprite_t *corner1 = gfx_MallocSprite(4, 4);
        shapes_GetRoundCorners(corner1, colors[(selected)], 4, x, y);
        gfx_ScaledSprite_NoClip(icon, x, y, 4, 4);
        shapes_DrawRoundCorners(corner1, 64, 64, x, y);
        free (corner1);
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
    
    free (icon);    // We do not need this anymore
    fileName[0] += 64 * hidden; // Make the hidden name viewable (If it is hidden)
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

void ui_CheckBox(uint8_t color, uint8_t bgColor, bool isChecked, int x, uint8_t y) {    // Draws a simple checkbox
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

void ui_Battery(uint8_t color, uint8_t batteryStatus, bool isCharging) {
    ui_DrawUISprite(color, UI_BATTERY, 286, 10);

    gfx_SetColor((255 * !(color > 131 && color % 8 > 3)) * (batteryStatus > 1) + 160 * (batteryStatus < 2));
    gfx_FillRectangle_NoClip(291 + (12 - batteryStatus * 3), 12, batteryStatus * 3, 6);

    if (isCharging) {
        ui_DrawUISprite(color, UI_CHARGING, 292, 12);  // Charging sprite instead of battery percentage
    }
}

void ui_Clock(bool is24Hour) {    // Displays time in either 24-Hour or AM/PM
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

void ui_StatusBar(uint8_t color, bool is24Hour, uint8_t batteryStatus, char *menuName) {  // Draws a 308 pixel long bar with rounded edges at 6, 6
    gfx_SetColor(color);
    shapes_RoundRectangleFill(color, 8, 308, 18, 6, 6);

    ui_Clock(is24Hour);
    ui_Battery(color, batteryStatus, boot_BatteryCharging());

    gfx_SetTextScale(2, 2);
    int x = 160 - gfx_GetStringWidth(menuName) / 2;
    gfx_PrintStringXY(menuName, x, 8);
}

void ui_DescriptionWrap(char *description, uint8_t charPerLine, int x, uint8_t y) {
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

void ui_BottomBar(uint8_t color) {
    shapes_RoundRectangleFill(color, 6, 34, 34, 8, 197);    // Background and sprite
    shapes_RoundRectangleFill(color, 15, 220, 32, 50, 198);
    shapes_RoundRectangleFill(color, 6, 34, 34, 278, 197);
    ui_DrawUISprite(color, UI_PAINT, 14, 203);
    ui_DrawUISprite(color, UI_INFO, 56, 204);
    ui_DrawUISprite(color, UI_SETTINGS, 284, 203);
}

bool ui_DeleteConf(uint8_t *colors, int x, uint8_t y) {
    bool retVal = true;
    while (kb_AnyKey());
    shapes_RoundRectangleFill(colors[0], 8, 208, 20, x, y);
    gfx_PrintStringXY("Are you sure?", x + 28, y + 6);
    gfx_BlitBuffer();
    gfx_SetColor(colors[2]);
    gfx_FillRectangle_NoClip(x + 160, y + 5, 27, 9);
    gfx_PrintStringXY("Yes    No", x + 130, y + 6);
    gfx_SetDrawScreen();
    gfx_FillRectangle_NoClip(x + 128, y + 5, 27, 9);
    gfx_PrintStringXY("Yes    No", x + 130, y + 6);
    gfx_SetColor(colors[0]);
    gfx_SetPixel(x + 128, y + 5);
    gfx_SetPixel(x + 128, y + 13);
    gfx_SetPixel(x + 154, y + 5);
    gfx_SetPixel(x + 154, y + 13);
    gfx_SetDrawBuffer();
    gfx_SetPixel(x + 160, y + 5);
    gfx_SetPixel(x + 160, y + 13);
    gfx_SetPixel(x + 186, y + 5);
    gfx_SetPixel(x + 186, y + 13);
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

void ui_DrawAllFiles(uint8_t *colors, uint8_t fileSelected, uint8_t fileCount, unsigned int fileStartLoc, bool appvars) {
    int x = 14;
    uint8_t y = 30;
    uint8_t filesSearched = 0;

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
                    free (description);
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
