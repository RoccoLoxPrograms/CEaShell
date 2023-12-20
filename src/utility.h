/**
 * --------------------------------------
 * 
 * CEaShell Source Code - utility.h
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2023
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#ifndef UTILITY_H
#define UTILITY_H

#include "defines.h"

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reads the shell preferences from the CEaShell AppVar.
 * 
 * @param shellPrefs Shell preferences struct.
 * @param shellContext Shell context struct.
 */
void util_ReadPrefs(struct preferences_t *shellPrefs, struct context_t *shellContext);

/**
 * @brief Writes the shell preferences to the CEaShell AppVar.
 * 
 * @param shellPrefs Shell preferences struct.
 * @param shellContext Shell context struct.
 * @param updateVAT Whether or not to re-sort the VAT afterwards. This is necessary when writing to the AppVar if not exiting the shell immediately afterwards.
 */
void util_WritePrefs(struct preferences_t *shellPrefs, struct context_t *shellContext, const bool updateVAT);

/**
 * @brief Saves the current preferences and shell context, ends the graphx library context, and turns off the calculator to reload CEaShell when turned back on.
 * 
 * @param shellPrefs Shell preferences struct.
 * @param shellContext Shell context struct.
 */
void util_PowerOff(struct preferences_t *shellPrefs, struct context_t *shellContext);

/**
 * @brief Initializes the filesystem by sorting the VAT, counting the number of files, and caching Program/AppVar VAT pointers.
 * 
 * @param shellPrefs Shell preferences struct.
 * @param shellContext Shell context struct.
 */
void util_FilesInit(struct preferences_t *shellPrefs, struct context_t *shellContext);

/**
 * @brief Sets a specific char in the Graphx font with a user-specified data and width.
 * 
 * @param charOffset Offset of the character being changed in the font.
 * @param charData Data for the new character being set.
 * @param charWidth Width of the new character being set.
 */
void util_SetGFXChar(uint8_t charOffset, const uint8_t *charData, uint8_t charWidth);

/**
 * @brief Gets various attributes for a file and stores them into a struct.
 * 
 * @param file Cursor offset of file to get attributes of.
 * @param fileInfo Struct to store file info into.
 * @param shellPrefs Shell preferences struct.
 * @param shellContext Shell context struct.
 */
void util_GetFileInfo(unsigned int file, struct file_t *fileInfo, struct preferences_t *shellPrefs, struct context_t *shellContext);

/**
 * @brief Searches backwards from a given location in a string for a space or word break.
 * 
 * @param string String to search in.
 * @param start Byte of the string to begin searching backwards from.
 * @return unsigned int The location of the word break, or, the initial location if no spaces occur.
 */
unsigned int util_SpaceSearch(const char *string, const unsigned int start);

/**
 * @brief Jumps to a file in the current directory beginning with the letter key pressed.
 * 
 * @param shellPrefs Shell preferences struct.
 * @param shellContext Shell context struct.
 * @param targetLetter Letter to jump to.
 */
void util_AlphaSearch(struct preferences_t *shellPrefs, struct context_t *shellContext, char targetLetter);

/**
 * @brief Updates the clock offset and keypressed value for timed key debouncing.
 * 
 * @param shellPrefs Shell preferences struct.
 * @param shellContext Shell context struct.
 * @param clockOffset Clock offset for timer.
 * @param keyPressed Whether a key is currently pressed.
 */
void util_UpdateKeyTimer(struct preferences_t *shellPrefs, struct context_t *shellContext, clock_t *clockOffset, bool *keyPressed);

/**
 * @brief Waits before repeating a keypress.
 * 
 * @param clockOffset Clock offset for timer.
 * @param keyPressed Whether a key is currently pressed.
 */
void util_WaitBeforeKeypress(clock_t *clockOffset, bool *keyPressed);

/**
 * @brief Safely archive a file and handle if a Garbage Collect occurs.
 * 
 * @param slot Variable handle.
 * @param fileName Name of file to archive.
 * @param type OS type of file to archive.
 */
void util_SafeArchive(uint8_t slot, char *fileName, uint8_t type);

#ifdef __cplusplus
}
#endif

#endif
