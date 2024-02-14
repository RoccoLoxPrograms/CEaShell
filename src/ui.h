/**
 * --------------------------------------
 * 
 * CEaShell Source Code - ui.h
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2024
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

/**
 * @brief Handles drawing dark vs. light sprites, depending on the color theme.
 * 
 * @param color Foreground color of the current color theme (shellPrefs->fgColor).
 * @param spriteNo Number representing the CEaShell icon to display.
 * @param x X coordinate to draw the sprite at.
 * @param y Y coordinate to draw the sprite at.
 */
void ui_DrawUISprite(const uint8_t color, const uint8_t spriteNo, const int x, const uint8_t y);

/**
 * @brief Draws CEaShell's status bar.
 * 
 * @param shellPrefs Shell preferences struct.
 * @param shellContext Shell context struct.
 * @param title Main title to display in the status bar.
 */
void ui_DrawStatusBar(struct preferences_t *shellPrefs, struct context_t *shellContext, char *title);

/**
 * @brief Draws the buttons and info bar at the bottom of the main UI.
 * 
 * @param shellPrefs Shell preferences struct.
 * @param description Description text to display in the info bar.
 */
void ui_DrawBottomBar(struct preferences_t *shellPrefs, char *description);

/**
 * @brief Draws a vertical or horizontal scrollbar.
 * 
 * @param shellPrefs Shell preferences struct.
 * @param x X coordinate of the start of the scrollbar window.
 * @param y Y coordinate of the start of the scrollbar window.
 * @param maxLength Length of the scrollbar window.
 * @param total Total items in the current context.
 * @param start Start item in the current context.
 * @param countPerPage Number of items per page in the current context.
 * @param horizontal True if the scrollbar should be horizontal, false for vertical.
 */
void ui_ScrollBar(struct preferences_t *shellPrefs, unsigned int x, uint8_t y, unsigned int maxLength, unsigned int total, unsigned int start, unsigned int countPerPage, bool horizontal);

/**
 * @brief Draws a checked or unchecked check box.
 * 
 * @param isChecked Current state of the check box.
 * @param x X coordinate to begin drawing the box at.
 * @param y Y coordinate to begin drawing the box at.
 */
void ui_CheckBox(const bool isChecked, const int x, const uint8_t y);

/**
 * @brief Draws the files displayed in the main file manager context.
 * 
 * @param shellPrefs Shell preferences struct.
 * @param shellContext Shell context struct.
 */
void ui_DrawFiles(struct preferences_t *shellPrefs, struct context_t *shellContext);

/**
 * @brief Displays a string with word-wrapping.
 * 
 * @param string String to display.
 * @param x X location to display the string at.
 * @param y Y location to display the string at.
 * @param charsPerLine Maximum number of characters on a line.
 * @param maxLines Maximum number of lines to display.
 * @return int Updated Y value
 */
int ui_PrintStringWrap(const char *string, unsigned int x, int y, unsigned int charsPerLine, uint8_t maxLines);

/**
 * @brief Print a string at double the scale (minimizes relocations to save space).
 * 
 * @param string String to print.
 * @param centerX X location to center the string at.
 * @param y Y location to display the string at.
 */
void ui_CenterStringBig(const char *string, unsigned int centerX, uint8_t y);

/**
 * @brief Gets an 8-character long string input (Used for file names).
 * 
 * @param shellPrefs Shell preferences struct.
 * @param shellContext Shell context struct.
 * @param x X coordinate to begin drawing input box at.
 * @param y Y coordinate to begin drawing input box at.
 * @param search Whether or not the input is used in a search.
 * @return char* User input.
 */
char *ui_StringInput(struct preferences_t *shellPrefs, struct context_t *shellContext, unsigned int x, uint8_t y, bool search);

/**
 * @brief Draws a frame of a transition.
 * 
 * @param radius Radius of the rounded corners.
 * @param x X coordinate to draw the rectangle at.
 * @param y Y coordinate to draw the rectangle at.
 * @param width Width of the rectangle.
 * @param height Height of the rectangle.
 */
void ui_TransitionDrawFrame(const uint8_t radius, const int x, const uint8_t y, const int width, const uint8_t height);

#ifdef __cplusplus
}
#endif

#endif
