#include "ui.h"
#include "shapes.h"
#include "utility.h"
#include "gfx/gfx.h"

#include <graphx.h>
#include <string.h>
#include <fileioc.h>
#include <sys/rtc.h>
#include <sys/power.h>

void ui_DrawUISprite(uint8_t color, uint8_t spriteNo, int x, uint8_t y) {
    bool colorAlt = !(color > 131 && color % 8 > 3);
    const gfx_sprite_t *uiIcons[14] = {battery, charging, paint, info, settings, lArrow, rArrow, batteryAlt, chargingAlt, paintAlt, infoAlt, settingsAlt, lArrowAlt, rArrowAlt};
    gfx_TransparentSprite_NoClip(uiIcons[spriteNo + colorAlt * 7], x, y);
    gfx_SetTextFGColor(colorAlt * 255);
}

static void ui_DrawFile(bool selected, uint8_t *colors, char *fileName, char *fileType, int x, uint8_t y) {
    bool colorAlt = (colors[1] > 131 && colors[1] % 8 > 3);
    if (selected) {
        shapes_RoundRectangleFill(colors[1], 6, 68, 78, x - 2, y - 2);
    }
    shapes_RoundRectangleFill(colors[2], 4, 64, 64, x, y);
    gfx_SetColor(255 * colorAlt);
    gfx_FillRectangle(x + 19, y + 14, 29, 39);
    shapes_FileIcon(255 * !colorAlt, colors[2], x + 16, y + 11);
    gfx_SetColor(255 * !colorAlt);
    gfx_FillRectangle(x + 19, y + 36, 28, 9);
    gfx_SetTextFGColor(255 * colorAlt);
    gfx_SetTextScale(1, 1);
    uint8_t stringLength = gfx_GetStringWidth(fileType);
    if (stringLength) {
        gfx_PrintStringXY(fileType, x + (64 - stringLength) / 2, y + 37);
    } else {
        gfx_PrintStringXY("?", x + 29, y + 37);
    }
    gfx_SetTextFGColor(255 * !colorAlt);
    stringLength = gfx_GetStringWidth(fileName);
    if (stringLength) {
        gfx_PrintStringXY(fileName, x + (64 - stringLength) / 2, y + 67);
    } else {
        gfx_PrintStringXY("?", x + 29, y + 67);
    }
}

static void ui_Battery(uint8_t color, uint8_t batteryStatus, bool isCharging) {
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

void ui_StatusBar(uint8_t color, bool is24Hour, char *fileName) {  // Draws a 308 pixel long bar with rounded edges at 6, 6
    gfx_SetColor(color);
    shapes_RoundRectangleFill(color, 8, 308, 18, 6, 6);

    ui_Clock(is24Hour);
    ui_Battery(color, boot_GetBatteryStatus(), boot_BatteryCharging());   // For some reason the battery charging check isn't working


    gfx_SetTextScale(2, 2);
    int x = 160 - gfx_GetStringWidth(fileName) / 2;
    if (x == 160) { // If no filename was given
        gfx_PrintStringXY("FILENAME", 96, 8);
    } else {
        gfx_PrintStringXY(fileName, x, 8);
    }

}

void ui_BottomBar(uint8_t color, char *description) {
    shapes_RoundRectangleFill(color, 6, 34, 34, 8, 197);    // Background and sprite
    shapes_RoundRectangleFill(color, 15, 220, 32, 50, 198);
    shapes_RoundRectangleFill(color, 6, 34, 34, 278, 197);
    ui_DrawUISprite(color, UI_PAINT, 14, 203);
    ui_DrawUISprite(color, UI_INFO, 56, 204);
    ui_DrawUISprite(color, UI_SETTINGS, 284, 203);
    
    gfx_SetTextScale(1, 1); // Description
    char lineOne[25] = "\0";
    char lineTwo[25] = "\0";
    if (strlen(description) > 24) { // Wraps text that is longer than 24, and cuts off anything longer than 48
        int8_t cut = util_SpaceSearch(description); // If there is a space it will end the line there
        int8_t descLen = strlen(description);
        strncpy(lineOne, description, cut);
        if (descLen - cut > 24) {
            strncpy(lineTwo, description + cut, 22);
        } else {
            strncpy(lineTwo, description + cut, 24);
        }
        gfx_PrintStringXY(lineOne, 82, 205);
        gfx_PrintStringXY(lineTwo, 82, 216);
        if (descLen - cut > 24) {
            gfx_PrintString("...");
        }
    } else {
        gfx_PrintStringXY(description, 82, 211);
    }
}

void ui_DrawAllFiles(uint8_t *colors, uint8_t fileSelected, uint8_t fileCount, uint8_t fileStartLoc, bool appvars) {
    int x = 14;
    uint8_t y = 30;
    uint8_t filesDrawn = 0;
    uint8_t filesSearched = 0;

    uint8_t fileType;
    char *fileName;
    void *filePtr = NULL;
    while ((fileName = ti_DetectAny(&filePtr, NULL, &fileType))) {
        if (*fileName == '!' || *fileName =='#') {
            continue;
        }
        if (!appvars && (fileType == TI_PRGM_TYPE || fileType == TI_PPRGM_TYPE)) {
            if (fileStartLoc <= filesSearched) {
                fileName = util_FixHiddenName(fileName);
                if (fileType == TI_PRGM_TYPE) {
                    ui_DrawFile((fileSelected == filesSearched), colors, fileName, "BSC", x, y);    // We'll assume all unprotected programs are basic for now
                } else {
                    ui_DrawFile((fileSelected == filesSearched), colors, fileName, "PRG", x, y);    // More advanced type detection later
                }
                filesDrawn++;
                if (y == 30) {
                    y = 116;
                } else {
                    x += 76;
                    y = 30;
                }
            }
            filesSearched++;
        } else if (appvars && fileType == TI_APPVAR_TYPE) {
            if (fileStartLoc <= filesSearched) {
                fileName = util_FixHiddenName(fileName);
                ui_DrawFile((fileSelected == filesSearched), colors, fileName, "VAR", x, y);
                filesDrawn++;
                if (y == 30) {
                    y = 116;
                } else {
                    x += 76;
                    y = 30;
                }
            }
            filesSearched++;
        }

        if ((x > 242) || filesSearched > fileCount) {
            break;
        }
    }
}
