/**
 * --------------------------------------
 * 
 * CEaShell Source Code - ui.h
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#ifndef UI_H
#define UI_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UI_BATTERY	    0
#define UI_CHARGING	    1
#define UI_PAINT	    2
#define UI_INFO		    3
#define UI_SETTINGS	    4
#define UI_LARROW	    5
#define UI_RARROW	    6
#define UI_DARROW       7
#define UI_CHECK        8
#define UI_CURSOR_A     9
#define UI_CURSOR_1     10

void ui_DrawUISprite(const uint8_t color, const uint8_t spriteNo, const int x, const uint8_t y);    // Takes care of drawing the sprite in white or black, depending on the theme

void ui_DrawFile(const bool selected, const bool drawName, const bool drawHidden, const bool hidden, uint8_t *colors, char *fileName, const uint8_t fileType, const uint8_t osFileType, const int x, const uint8_t y);  // Draws a file, with the icon if it exists

void ui_CheckBox(const uint8_t color, const uint8_t bgColor, const bool isChecked, const int x, const uint8_t y);   // Draws a simple checkbox

void ui_Clock(const bool is24Hour); // Displays time in either 24-Hour or AM/PM

void ui_Battery(const uint8_t color, const uint8_t batteryStatus, const bool isCharging);

void ui_StatusBar(const uint8_t color, const bool is24Hour, const uint8_t batteryStatus, const char *menuName, const unsigned int fileCount, const bool showFileCount); // Draws a 308 pixel long bar with rounded edges at 6, 6

void ui_DescriptionWrap(const char *description, const uint8_t charPerLine, const int x, const uint8_t y);

void ui_BottomBar(const uint8_t color);

bool ui_DeleteConf(uint8_t *colors, const int x, const uint8_t y);

bool ui_RenameBox(uint8_t *colors, char *newName);

void ui_DrawAllFiles(uint8_t *colors, void **programPtrs, void **appvarPtrs, const uint8_t fileSelected, const uint8_t fileCount, const unsigned int fileStartLoc, const uint8_t directory, const bool displayCEaShell, const bool showHiddenProg, const bool showApps, const bool showAppvars);

void ui_MiniCursor(uint8_t color, int x, uint8_t y);

void ui_DrawMenuItem(const char *lineOne, const int x, const uint8_t y, const bool status);

void ui_DrawDoubleMenuItem(const char *lineOne, const char *lineTwo, const int x, const uint8_t y, const bool status);

void ui_AboutScreen(uint8_t *colors);

void ui_NewUser(void);

uint8_t ui_CopyNewMenu(uint8_t *colors, char *name);

#ifdef __cplusplus
}
#endif

#endif
