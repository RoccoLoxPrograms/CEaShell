/**
 * --------------------------------------
 * 
 * CEaShell Source Code - menu.h
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2024
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#ifndef MENU_H
#define MENU_H

#include <keypadc.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Draws a string on the screen or gets the pixel width of a given option value.
 * 
 * @param xEdge Right hand edge of the menu box, which text is aligned to. (If zero, no string is drawn)
 * @param y Y coordinate to begin drawing text at.
 * @param type Value info for the option.
 * @param value Value info for the option.
 * @return uint8_t Width of string.
 */
uint8_t menu_DrawValueString(unsigned int xEdge, int y, uint8_t type, uint8_t value);

/**
 * @brief Calculate the number of lines a string will take in a menu.
 * 
 * @param string String to calculate lines for.
 * @param charsPerLine Maximum number of characters per line.
 * @param maxLines Maximum number of lines possible.
 * @return uint8_t Number of lines.
 */
uint8_t menu_CalculateLines(char *string, unsigned int charsPerLine, uint8_t maxLines);

/**
 * @brief Draw a still frame of a menu.
 * 
 * @param shellPrefs Shell preferences struct.
 * @param x X coordinate of the left side of the menu box.
 * @param clipY Y coordinate of the top of the on-screen menu clip region.
 * @param startY Y coordinate of the top of the full menu (can be off-screen).
 * @param width Width of menu clpping box.
 * @param height Height of menu clipping box.
 * @param menuContext Menu context information 
 */
void menu_Draw(struct preferences_t *shellPrefs, unsigned int x, uint8_t clipY, int startY, uint8_t width, uint8_t height, struct menu_t *menuContext);

/**
 * @brief Opens a menu (like settings / customization).
 * 
 * @param shellPrefs Shell preferences struct.
 * @param shellContext Shell context struct.
 * @param menuContext Menu context / config struct.
 * @param exitKey Key to exit menu.
 */
void menu_Open(struct preferences_t *shellPrefs, struct context_t *shellContext, struct menu_t *menuContext, kb_lkey_t exitKey);

/**
 * @brief Opens a menu with two options.
 * 
 * @param shellPrefs Shell preferences struct.
 * @param shellContext Shell context struct.
 * @param x X coordinate to begin drawing menu at.
 * @param optionWidth Width of option selector.
 * @param option1 First option text.
 * @param option2 Second option text.
 * @return true Second option was selected.
 * @return false First option was selected.
 */
bool menu_YesNo(struct preferences_t *shellPrefs, struct context_t *shellContext, unsigned int x, unsigned int optionWidth, char *option1, char *option2);

/**
 * @brief Opens a menu to delete a file.
 * 
 * @param shellPrefs Shell preferences struct.
 * @param shellContext Shell context struct.
 * @param fileInfo Struct containing info on the file to delete.
 * @return bool Whether a file was deleted.
 */
bool menu_DeleteFile(struct preferences_t *shellPrefs, struct context_t *shellContext, struct file_t *fileInfo);

/**
 * @brief Opens a menu to rename a file.
 * 
 * @param shellPrefs Shell preferences struct.
 * @param shellContext Shell context struct.
 * @param fileInfo Struct containing info on the file to rename.
 * @return bool Whether a file was renamed.
 */
bool menu_RenameFile(struct preferences_t *shellPrefs, struct context_t *shellContext, struct file_t *fileInfo);

/**
 * @brief Opens a menu to either copy or create a new file.
 * 
 * @param shellPrefs Shell preferences struct.
 * @param shellContext Shell context struct.
 * @param fileInfo Struct containing info on the file to copy.
 */
void menu_CopyFile(struct preferences_t *shellPrefs, struct context_t *shellContext, struct file_t *fileInfo);

/**
 * @brief Screen displaying credits and other info about CEaShell.
 * 
 * @param shellPrefs Shell perferences struct.
 * @param shellContext Shell context struct.
 */
void menu_AboutScreen(struct preferences_t *shellPrefs, struct context_t *shellContext);

#ifdef __cplusplus
}
#endif

#endif
