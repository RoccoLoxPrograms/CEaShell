/**
 * --------------------------------------
 * 
 * CEaShell Source Code - fileOps.h
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2025
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#ifndef FILEOPS_H
#define FILEOPS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opens an edit buffer to edit a program or AppVar.
 * 
 * @param name Name of the file to edit.
 * @param celticVar Whether or not the file is a Celtic AppVar.
 * @param shellPrefs Shell preferences struct.
 */
void asm_editProgram_edit(char *name, bool celticVar, struct preferences_t *shellPrefs);

/**
 * @brief Gets the correct size of a program or AppVar (ti_GetSize is slightly off).
 * 
 * @param vatPtr Program's VAT pointer.
 * @return int Size of the file.
 */
int asm_fileOps_getPrgmSize(void *vatPtr);

/**
 * @brief Gets a program's more detailed shell type.
 * 
 * @param vatPtr Program's VAT pointer.
 * @return uint8_t Shell type for the program.
 */
uint8_t asm_fileOps_getPrgmType(void *vatPtr);

/**
 * @brief Checks whether or not the AppVar is a Celtic AppVar.
 * 
 * @param vatPtr AppVar's VAT pointer.
 * @return uint8_t Shell type for the AppVar.
 */
uint8_t asm_fileOps_getAppVarType(void *vatPtr);

/**
 * @brief Copies a program or AppVar.
 * 
 * @param name Name of the file to copy.
 * @param copy Name for the new copy of the file being created.
 * @param type OS type for the file to copy.
 */
void asm_fileOps_copyFile(char *name, char *copy, uint8_t type);

/**
 * @brief Safely rename a file when doing so will cause a Garbage Collect to trigger.
 * 
 * @param oldName Name of file being renamed.
 * @param newName New name for file being renamed.
 * @param type Type of file being renamed.
 */
void asm_fileOps_renameOnGC(char *oldName, char *newName, uint8_t type);

/**
 * @brief Toggle the hidden status of a program.
 * 
 * @param name Name of the program.
 * @param type OS type for the program.
 */
void asm_fileOps_hidePrgm(char *name, uint8_t type);

/**
 * @brief Toggle the protected status of a program.
 * 
 * @param name Name of the program.
 * @param type OS type for the program.
 */
void asm_fileOps_lockPrgm(char *name, uint8_t type);

/**
 * @brief Checks and returns the description info of an ASM program.
 * 
 * @param vatPtr Program's VAT pointer.
 * @param shellType Shell type for the program.
 * @param description String to return the description in. (Begins with \0 if no description exists)
 */
void asm_fileOps_getDescASM(void *vatPtr, uint8_t shellType, char *description);

/**
 * @brief Checks and returns the description info of a TI-BASIC program.
 * 
 * @param vatPtr Program's VAT pointer.
 * @param description String to return the description in. (Begins with \0 if no description exists)
 */
void asm_fileOps_getDescBASIC(void *vatPtr, char *description);

/**
 * @brief Checks and returns the icon info of an ASM program.
 * 
 * @param vatPtr Program's VAT pointer.
 * @param shellType Shell type for the program.
 * @param icon Pointer to return the icon sprite in.
 * @return true The program has an icon.
 * @return false The program does not have an icon.
 */
bool asm_fileOps_getIconASM(void *vatPtr, uint8_t shellType, gfx_sprite_t *icon);

/**
 * @brief Checks and returns the icon info of a TI-BASIC program.
 * 
 * @param vatPtr Program's VAT pointer.
 * @param icon Pointer to return the icon sprite in.
 * @return true The program has an icon.
 * @return false The program does not have an icon.
 */
bool asm_fileOps_getIconDCS(void *vatPtr, gfx_sprite_t *icon);

/**
 * CelticCE AppVar header, which can be added to AppVars created using Celtic's CreateVar command.
 * 
 */
extern char rodata_celticAppVarHeader;

/**
 * @brief Runs a program.
 * 
 * @param name Name of the program.
 * @param type OS type for the program.
 * @param shellType Shell type for the program.
 * @param shellPrefs Shell preferences struct.
 */
void asm_runProgram_run(char *name, uint8_t type, uint8_t shellType, struct preferences_t *shellPrefs);

/**
 * @brief Checks if a file has an associated [on] + number key launch shortcut and returns the number key if so.
 * 
 * @param file File to check for.
 * @param shortcuts Array of existing launch shortcut info.
 * @return uint8_t Number key associated with shortcut, or 10 if no shortcut.
 */
uint8_t asm_fileOps_getLaunchKey(struct launch_t *file, struct launch_t *shortcuts);

#ifdef __cplusplus
}
#endif

#endif
