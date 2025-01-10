/**
 * --------------------------------------
 * 
 * CEaShell Source Code - utils.h
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2025
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Returns the amount of RAM free on the calculator.
 * 
 * @return unsigned int Current amount of free RAM.
 */
unsigned int asm_utils_getFreeRAM(void); 

/**
 * @brief Turns lowercase in TI-OS on or off.
 * 
 * @param status False if turning lowercase off, true if turning it on.
 */
void asm_utils_toggleLowercase(bool status);

/**
 * @brief Checks whether archiving a file will result in a garbage collect.
 * 
 * @param vatPtr File's VAT pointer.
 * @return true Archiving will not trigger a garbage collect.
 * @return false Archiving will trigger a garbage collect.
 */
bool asm_utils_willNotGC(void *vatPtr);

/**
 * @brief Inverts the palette used in CEaShell.
 * 
 */
void asm_utils_invertPalette(void);

/**
 * @brief Safely archive a file and handle when a Garbage Collect occurs.
 * 
 * @param vatPtr File's VAT pointer.
 */
void asm_utils_arcOnGC(void *vatPtr);

/**
 * @brief Sets the width of a specific character in the Graphx font to a custom value.
 * 
 * @param charNum Offset of the character in the font to modify the width of.
 * @param width Custom width to set the character to.
 */
void asm_utils_setGfxCharWidth(uint8_t charNum, uint8_t width);

/**
 * @brief Scans the keypad and returns a character based on the key pressed.
 * 
 * @param inputMode Input mode, like 2nd mode, uppercase mode, or lowercase mode.
 * @return char Character found from keypress.
 */
char asm_utils_getCharFromKey(uint8_t inputMode);

/**
 * @brief Gets a the name of a file from a pointer to the VAT entry of that file.
 * 
 * @param filePtr Poiner to the file's VAT entry.
 * @return void* Pointer to the file name.
 */
void *asm_utils_getFileName(void *filePtr);

/**
 * @brief Checks if a file is archived.
 * 
 * @param vatPtr File's VAT pointer.
 * @param type OS type for the program.
 * @return true File is archived.
 * @return false File is in RAM.
 */
bool asm_utils_isFileArchived(void *vatPtr);

/**
 * @brief Checks if a file name is valid.
 * 
 * @param name Name of the file.
 * @param appvarDir If we're in the appvars directory.
 * @return true Name is valid.
 * @return false Name is invalid.
 */
bool asm_utils_isNameValid(char *name, bool appvarDir);

/**
 * @brief Sets up Ans for HexaEdit headless start.
 * 
 * @param name Name of variable to edit.
 * @param nameLength Length of name of variable to edit.
 * @param type Type of variable to edit.
 */
void asm_utils_initHexaEditStart(char *name, uint8_t nameLength, uint8_t type);

/**
 * @brief Checks if there's enough RAM for an operation.
 * 
 * @param amount Amount of RAM to check for.
 * @return true There is enough RAM.
 * @return false There isn't enough RAM.
 */
bool asm_utils_checkEnoughRAM(unsigned int amount);

/**
 * @brief Create Ans if it doesn't exist.
 * 
 */
void asm_utils_restoreAns(void);

#ifdef __cplusplus
}
#endif

#endif
