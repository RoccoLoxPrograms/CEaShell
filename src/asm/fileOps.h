/**
 * --------------------------------------
 * 
 * CEaShell Source Code - fileOps.h
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2024
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#ifndef FILEOPS_H
#define FILEOPS_H

#include <defines.h>

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
 * @param name Name of the file.
 * @param type OS type for the file.
 * @return int Size of the file.
 */
int asm_fileOps_getPrgmSize(char *name, uint8_t type);

/**
 * @brief Gets a program's more detailed shell type.
 * 
 * @param name Name of the program. 
 * @param type OS type for the program.
 * @return uint8_t Shell type for the program.
 */
uint8_t asm_fileOps_getPrgmType(char *name, uint8_t type);

/**
 * @brief Checks whether or not the AppVar is a Celtic AppVar.
 * 
 * @param name Name of the AppVar.
 * @return uint8_t Shell type for the AppVar.
 */
uint8_t asm_fileOps_getAppVarType(char *name);

/**
 * @brief Copies a program or AppVar.
 * 
 * @param name Name of the file to copy.
 * @param copy Name for the new copy of the file being created.
 * @param type OS type for the file to copy.
 */
void asm_fileOps_copyFile(char *name, char *copy, uint8_t type);

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
 * @param name Name of the program.
 * @param type OS type for the program.
 * @param shellType Shell type for the program.
 * @param description String to return the description in.
 * @return true The program has a description.
 * @return false The program does not have a description.
 */
bool asm_fileOps_getDescASM(char *name, uint8_t type, uint8_t shellType, char *description);

/**
 * @brief Checks and returns the description info of a TI-BASIC program.
 * 
 * @param name Name of the program.
 * @param type OS type for the program.
 * @param description String to return the description in.
 * @return true The program has a description.
 * @return false The program does not have a description.
 */
bool asm_fileOps_getDescBASIC(char *name, uint8_t type, char *description);

/**
 * @brief Checks and returns the icon info of an ASM program.
 * 
 * @param name Name of the program.
 * @param type OS type for the program.
 * @param shellType Shell type for the program.
 * @param icon Pointer to return the icon sprite in.
 * @return true The program has an icon.
 * @return false The program does not have an icon.
 */
bool asm_fileOps_getIconASM(char *name, uint8_t type, uint8_t shellType, gfx_sprite_t *icon);

/**
 * @brief Checks and returns the icon info of a TI-BASIC program.
 * 
 * @param name Name of the program.
 * @param type OS type for the program.
 * @param icon Pointer to return the icon sprite in.
 * @return true The program has an icon.
 * @return false The program does not have an icon.
 */
bool asm_fileOps_getIconDCS(char *name, uint8_t type, gfx_sprite_t *icon);

/**
 * CelticCE AppVar header, which can be added to AppVars created using Celtic's CreateVar command.
 * 
 */
extern char rodata_celticAppVarHeader;

#ifdef __cplusplus
}
#endif

#endif
