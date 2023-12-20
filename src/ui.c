/**
 * --------------------------------------
 * 
 * CEaShell Source Code - ui.c
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2023
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#include "defines.h"
#include "menu.h"
#include "shapes.h"
#include "ui.h"
#include "utility.h"

#include "asm/fileOps.h"
#include "asm/utils.h"
#include "asm/spi.h"
#include "gfx/gfx.h"

#include <fileioc.h>
#include <graphx.h>
#include <keypadc.h>
#include <string.h>
#include <time.h>

#include <sys/power.h>
#include <sys/rtc.h>

void ui_DrawUISprite(const uint8_t color, const uint8_t spriteNo, const int x, const uint8_t y) {
    bool colorAlt = !(color > 131 && color % 8 > 3);

    // Dark sprites first, then light sprites
    static const gfx_sprite_t *uiIcons[24] = {
    battery, charging, paint, info, settings, lArrow, rArrow, dArrow, check, cursorNumber, cursorUpper, cursorLower,
    batteryAlt, chargingAlt, paintAlt, infoAlt, settingsAlt, lArrowAlt, rArrowAlt, dArrowAlt, checkAlt, cursorNumberAlt, cursorUpperAlt, cursorLowerAlt};

    gfx_TransparentSprite(uiIcons[spriteNo + colorAlt * 12], x, y);
}

void ui_DrawStatusBar(struct preferences_t *shellPrefs, struct context_t *shellContext, char *title) {
    gfx_SetColor(shellPrefs->fgColor);
    gfx_FillRectangle(0, 0, 320, 20);

    if (title[0] < 'A') {
        title[0] += 64;
    }

    // Draw status bar main title
    if (title != NULL) {
        ui_CenterStringBig(title, 160, 3);
    }

    // Draw clock
    uint8_t time[3];
    bool isAfterNoon = boot_IsAfterNoon();

    boot_GetTime(&time[0], &time[1], &time[2]);
    time[2] -= 12 * (!(shellPrefs->timeFormat) && isAfterNoon); // Add 12 to the hour if it's PM and 24-Hour Time, so that 1 PM becomes 13, etc.

    if (!time[2] && !(shellPrefs->timeFormat)) {
        time[2] = 12;
    }

    gfx_SetTextXY(9, 7);
    gfx_PrintUInt(time[2], 1);
    gfx_PrintString(":");
    gfx_PrintUInt(time[1], 2);

    if (!(shellPrefs->timeFormat)) {
        if (isAfterNoon) {
            gfx_PrintString(" PM");
        } else {
            gfx_PrintString(" AM");
        }
    }

    // Draw file count if the user has it enabled
    if (shellPrefs->showFileCount) {
        gfx_SetTextXY(256, 7);
        gfx_PrintUInt(*(&(shellContext->programCount) + shellContext->directory), 4); // Print the current number of files in the active directory.
    }

    // Draw battery
    ui_DrawUISprite(shellPrefs->fgColor, UI_BATTERY, 292, 5);
    gfx_SetColor((255 * !(shellPrefs->fgColor > 131 && shellPrefs->fgColor % 8 > 3)) * (shellContext->batteryLevel > 1) + 160 * (shellContext->batteryLevel < 2));
    gfx_FillRectangle_NoClip(297 + (12 - shellContext->batteryLevel * 3), 7, shellContext->batteryLevel * 3, 6);

    if (boot_BatteryCharging()) {
        ui_DrawUISprite(shellPrefs->fgColor, UI_CHARGING, 298, 7);  // Charging sprite instead of battery percentage
    }
}

void ui_DrawBottomBar(struct preferences_t *shellPrefs, char *description) {
    gfx_SetColor(shellPrefs->fgColor);

    // Draw corner buttons and info bar
    shapes_RoundRectangleFill(6, 8, 198, 34, 34);
    shapes_RoundRectangleFill(15, 50, 199, 220, 32);
    shapes_RoundRectangleFill(6, 278, 198, 34, 34);

    ui_DrawUISprite(shellPrefs->fgColor, UI_PAINT, 14, 204);
    ui_DrawUISprite(shellPrefs->fgColor, UI_INFO, 56, 205);
    ui_DrawUISprite(shellPrefs->fgColor, UI_SETTINGS, 284, 204);

    if (description != NULL) {
        if (menu_CalculateLines(description, 24, 2) < 2) {
            gfx_PrintStringXY(description, 82, 210);
        } else {
            ui_PrintStringWrap(description, 82, 205, 24, 2);
        }
    } else {
        os_ArcChk();

        #ifdef FR
        gfx_PrintStringXY("RAM Libre: ", 82, 205);
        #else
        gfx_PrintStringXY("RAM Free: ", 82, 205);
        #endif

        gfx_PrintUInt(asm_utils_getFreeRAM(), 6);

        #ifdef FR
        gfx_PrintStringXY("ROM Libre: ", 82, 216);
        #else
        gfx_PrintStringXY("ROM Free: ", 82, 216);
        #endif

        gfx_PrintInt(os_TempFreeArc, 7);
    }
}

void ui_ScrollBar(struct preferences_t *shellPrefs, unsigned int x, uint8_t y, unsigned int maxLength, unsigned int total, unsigned int start, unsigned int countPerPage, bool horizontal) {
    if (total) {
        unsigned int scrollBarLength = (float)maxLength / ((float)total / (float)countPerPage) + 1;

        if (scrollBarLength > maxLength) {
            scrollBarLength = maxLength;
        } else if (scrollBarLength < 4) {
            scrollBarLength = 4;
        }

        uint8_t scrollOffset = (float)maxLength / (float)total * (float)start;

        gfx_SetColor(shellPrefs->bgColor);

        unsigned int width;
        uint8_t height;

        if (horizontal) {
            width = maxLength;
            height = 4;
        } else {
            width = 4;
            height = maxLength;
        }

        gfx_FillRectangle_NoClip(x, y, width, height);
        gfx_SetColor(shellPrefs->hlColor);

        if (horizontal) {
            width = scrollBarLength;
            x += scrollOffset;
        } else {
            height = scrollBarLength;
            y += scrollOffset;
        }

        shapes_PixelIndentRectangle(x, y, width, height);
    }
}

void ui_CheckBox(const bool isChecked, const int x, const uint8_t y) {
    uint8_t bgColor = gfx_GetPixel(x, y);

    if (!(bgColor > 131 && bgColor % 8 > 3)) {
        gfx_SetColor(148);
    } else {
        gfx_SetColor(181);
    }

    shapes_PixelIndentRectangle(x, y, 7, 7);

    if (isChecked) {
        ui_DrawUISprite(bgColor, UI_CHECK, x + 1, y - 1);
    }
}

void ui_File(unsigned int x, uint8_t y, unsigned int file, struct file_t *fileInfo, struct preferences_t *shellPrefs, struct context_t *shellContext) {
    uint8_t scale = shellPrefs->uiScale;

    util_GetFileInfo(file, fileInfo, shellPrefs, shellContext);
    free(fileInfo->description); // Don't use this here
    fileInfo->description = NULL;

    if (fileInfo->hidden) {
        fileInfo->name[0] += 64;
        gfx_SetTextFGColor(shellPrefs->hiddenTextColor);
    }

    unsigned int textX = x + 19;
    unsigned int textY = y + 4;
    uint8_t background = gfx_GetPixel(x, y);

    if (scale > 1) {
        textX = (x + ((scale * 16) / 2)) - (gfx_GetStringWidth(fileInfo->name) / 2);
        textY = y + (scale * 16) + 3;
    }

    gfx_PrintStringXY(fileInfo->name, textX, textY);
    free(fileInfo->name);
    fileInfo->name = NULL;
    gfx_SetColor(shellPrefs->hlColor);
    gfx_SetTextFGColor(shellPrefs->textColor);

    if (fileInfo->icon != NULL) {
        gfx_ScaledSprite_NoClip(fileInfo->icon, x, y, scale, scale);
    } else {
        gfx_FillRectangle_NoClip(x, y, 16 * scale, 16 * scale);

        if (fileInfo->shellType == DIR_TYPE) {
            shapes_Folder(scale, x + 3 * scale, y + 4 * scale);
        } else {
            shapes_FileIcon(scale, fileInfo->shellType, x + 4 * scale, y + 3 * scale);
        }
    }

    free(fileInfo->icon);
    fileInfo->icon = NULL;

    // Round corners
    if (scale == 1) {
        gfx_SetColor(background);
        gfx_SetPixel(x, y);
        gfx_SetPixel(x + 15, y);
        gfx_SetPixel(x, y + 15);
        gfx_SetPixel(x + 15, y + 15);
    } else {
        gfx_sprite_t *corner1 = gfx_MallocSprite(4, 4);
        shapes_GetRoundCorners(corner1, background, 4, x, y);
        shapes_DrawRoundCorners(corner1, 16 * scale, 16 * scale, x, y);
        free(corner1);
    }
}

void ui_DrawFiles(struct preferences_t *shellPrefs, struct context_t *shellContext) {
    struct file_t *fileInfo = malloc(sizeof(struct file_t));
    uint8_t scale = shellPrefs->uiScale; // Might be faster this way?
    unsigned int fileDrawing = shellContext->fileStartLoc;
    unsigned int fileCount = *(&(shellContext->programCount) + shellContext->directory); // This should save us from doing math a lot

    unsigned int iconX = 20;

    switch (scale) {
        case 3:
            iconX = 23;
            break;
        case 2:
            iconX = 36;
            break;
        case 1:
            iconX = 22;
            break;
        default:
            break;
    }

    uint8_t iconY;
    uint8_t rows = 2;

    if (scale < 4) {
        rows = 6 / scale;
    }

    uint8_t columns = 4 - (scale == 1);

    /*
     * Icons drawn in this order:
     *
     * +-+-+-+-+
     * |1|3|5|7|
     * +-+-+-+-+
     * |2|4|6|8|
     * +-+-+-+-+
     * 
    **/

    for (uint8_t i = 0; i < columns; i++) {
        iconY = 30; // Reset Y value at the start of each column

        if (scale == 3) {
            iconY = 39;
        }

        for (uint8_t j = 0; j < rows; j++) {
            if (fileDrawing >= fileCount) { // Only draw files that actually exist on the calculator. Do this first in case there are no files in the active directory.
                free(fileInfo);
                fileInfo = NULL;
                return;
            }

            if (fileDrawing == shellContext->fileSelected) {
                gfx_SetColor(shellPrefs->fgColor);

                if (scale > 1) {
                    shapes_RoundRectangleFill(6, iconX - (2 + 8 * (4 - scale)), iconY - 2, 68, 78 - (16 * (4 - scale)));
                } else {
                    shapes_PixelIndentRectangle(iconX - 2, iconY - 2, 87, 20);
                }
            }

            ui_File(iconX, iconY, fileDrawing, fileInfo, shellPrefs, shellContext);

            switch (scale) {
                case 4:
                    iconY += 82;
                    break;
                case 3:
                    iconY += 75;
                    break;
                case 2:
                    iconY += 52;
                    break;
                case 1:
                    iconY += 27;
                    break;
                default:
                    break;
            }

            fileDrawing++; // Move on to the next file
        }

        iconX += 72 + 5 * (scale & 1) + 10 * (scale == 1); // If scale is 2 or 4, add 72. If 3, add 77, and if 1, add 87
    }

    free(fileInfo);
    fileInfo = NULL;
}

int ui_PrintStringWrap(const char *string, unsigned int x, int y, unsigned int charsPerLine, uint8_t maxLines) {
    unsigned int offset = 0;
    uint8_t line = 1;
    unsigned int end = 0;

    while (line <= maxLines && y + 8 <= 240) {
        gfx_SetTextXY(x, y);

        if (strlen(string + offset) <= charsPerLine || line == maxLines) {
            end = charsPerLine;
        } else {
            end = util_SpaceSearch(string + offset, charsPerLine);
        }

        for (unsigned int byte = 0; byte < end; byte++) {
            if ((string + offset)[byte] == '\0') {
                return y;
            }

            if (line == maxLines && byte == end - 1 && (string + offset)[byte + 1] != '\0') {
                gfx_PrintString("...");
            } else if ((string + offset)[byte] != '\n'){
                gfx_PrintChar((string + offset)[byte]);
            }
        }

        offset += end;

        if (*(string + offset) == '\0') {
            return y;
        }

        y += 12;
        line++;
    }

    return y;
}

void ui_CenterStringBig(const char *string, unsigned int centerX, uint8_t y) {
    gfx_SetTextScale(2, 2);
    gfx_PrintStringXY(string, centerX - gfx_GetStringWidth(string) / 2, y);
    gfx_SetTextScale(1, 1);
}

char *ui_StringInput(struct preferences_t *shellPrefs, struct context_t *shellContext, unsigned int x, uint8_t y) {
    bool cursorActive = true;
    uint8_t currentOffset = 0;
    uint8_t charCount = 0;
    unsigned int cursorX = x;
    static const char *modeChars = "1Aa";

    char *input = malloc(9);
    char inputChar = '\0';
    uint8_t inputMode = INPUT_UPPER;

    for (uint8_t i = 0; i <= 9; i++) {
        input[i] = '\0';
    }

    bool keyPressed = false;
    clock_t clockOffset = clock();

    gfx_SetColor(shellPrefs->fgColor);
    gfx_Rectangle_NoClip(245, 206, 2, 18);

    while (!kb_IsDown(kb_KeyClear)) {
        kb_Scan();
        util_UpdateKeyTimer(shellPrefs, shellContext, &clockOffset, &keyPressed);

        if (kb_AnyKey() && !kb_IsDown(kb_KeyClear) && (!keyPressed || clock() - clockOffset > CLOCKS_PER_SEC / 20)) {
            cursorActive = true;

            gfx_SetColor(shellPrefs->bgColor);
            gfx_FillRectangle_NoClip(x, y, charCount * 16 + 2, 16);
            gfx_SetTextScale(2, 2);

            if ((kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter)) && charCount != 0 && asm_utils_isNameValid(input, shellContext->directory == APPVARS_FOLDER)) {
                uint8_t slot = ti_OpenVar(input, "r", OS_TYPE_PRGM + (OS_TYPE_APPVAR - OS_TYPE_PRGM) * (shellContext->directory == APPVARS_FOLDER)); // Check if file exists
                ti_Close(slot);

                if (!slot) {
                    break;
                }
            } else if (kb_IsDown(kb_KeyLeft)) {
                if (currentOffset) {
                    currentOffset--;
                    cursorX -= gfx_GetCharWidth(input[currentOffset]);
                }
            } else if (kb_IsDown(kb_KeyRight)) {
                if (currentOffset < charCount) {
                    cursorX += gfx_GetCharWidth(input[currentOffset]);
                    currentOffset++;
                }
            } else if (kb_IsDown(kb_KeyUp)) {
                currentOffset = 0;
            } else if (kb_IsDown(kb_KeyDown)) {
                currentOffset = charCount;
                cursorX = x;
            } else if (kb_IsDown(kb_KeyMode)) { // Backspace
                if (currentOffset && charCount) {
                    charCount--;
                    currentOffset--;
                    cursorX -= gfx_GetCharWidth(input[currentOffset]);

                    for (uint8_t i = currentOffset + 1; input[i - 1] != '\0'; i++) {
                        input[i - 1] = input[i];
                    }
                }
            } else if (kb_IsDown(kb_KeyDel)) {
                if (currentOffset < charCount && charCount) {
                    for (uint8_t i = currentOffset; input[i] != '\0'; i++) {
                        input[i] = input[i + 1];
                    }

                    charCount--;
                }
            } else if (kb_IsDown(kb_KeyAlpha)) {
                if (inputMode == INPUT_LOWER) {
                    inputMode = 0;
                } else {
                    inputMode++;
                }

                while (kb_AnyKey());
            } else if (charCount < 8) {
                if (!keyPressed) {
                    inputChar = asm_utils_getCharFromKey(inputMode);
                }

                if (inputChar != '\0') {
                    for (uint8_t i = 8; i != currentOffset; i--) {
                        input[i] = input[i - 1];
                    }

                    input[currentOffset] = inputChar;
                    charCount++;
                    cursorX += gfx_GetCharWidth(input[currentOffset]);
                    currentOffset++;
                }
            }

            gfx_SetColor(shellPrefs->textColor);
            gfx_FillRectangle_NoClip(cursorX, y, 2, 14); // Draw new cursor
            gfx_PrintStringXY(input, x + 2, y);
            gfx_SetTextScale(1, 1);
            gfx_SetColor(shellPrefs->bgColor);
            gfx_FillRectangle_NoClip(251, 211, 7, 7);
            gfx_SetTextXY(251, 211);
            gfx_PrintChar(modeChars[inputMode]);

            gfx_BlitBuffer();

            util_WaitBeforeKeypress(&clockOffset, &keyPressed);
        }

        if (clock() - clockOffset > CLOCKS_PER_SEC / 4 && !keyPressed) {
            if (cursorActive) {
                gfx_SetColor(shellPrefs->textColor);
            } else {
                gfx_SetColor(shellPrefs->bgColor);
            }

            gfx_FillRectangle_NoClip(cursorX, y, 2, 14);

            cursorActive = !cursorActive;
            clockOffset = clock();

            gfx_BlitBuffer();
        }
    }

    if (kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter)) {
        return input;
    }

    free(input);
    return NULL;
}

void ui_TransitionDrawFrame(const uint8_t radius, const int x, const uint8_t y, const int width, const uint8_t height) {
    asm_spi_beginFrame();
    gfx_BlitBuffer();
    shapes_RoundRectangleFill(radius, x, y, width, height);
    asm_spi_endFrame();
}
