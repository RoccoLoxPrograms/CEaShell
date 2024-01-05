/**
 * --------------------------------------
 * 
 * CEaShell Source Code - hooks.h
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2024
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#ifndef HOOKS_H
#define HOOKS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Installs the TI-BASIC stop token detection hook.
 * 
 */
void asm_hooks_installStopHook(void);

/**
 * @brief Removes the TI-BASIC stop token detection hook.
 * 
 */
void asm_hooks_removeStopHook(void);

/**
 * @brief Installs the homescreen hook to run programs from the homescreen.
 * 
 */
void asm_hooks_installHomescreenHook(void);

/**
 * @brief Removes the homescreen hook.
 * 
 */
void asm_hooks_removeHomescreenHook(void);

/**
 * @brief Detects which of the menu hooks needs to be installed and installs it.
 * 
 * @param hook Value for what hook to install.
 */
void asm_hooks_installMenuHook(uint8_t hook);

/**
 * @brief Removes the all menu hooks.
 * 
 */
void asm_hooks_removeMenuHook(void);

/**
 * @brief Detects which of the on shortcuts and icon/description GetCSC hooks to install and installs them.
 * 
 * @param hook Value for what hook to install.
 */
void asm_hooks_installGetCSCHook(uint8_t hook);

/**
 * @brief Removes all GetCSC hooks installed.
 * 
 */
void asm_hooks_removeGetCSCHook(void);

/**
 * @brief Triggers APD to return to CEaShell after.
 * 
 */
void asm_hooks_triggerAPD(void);

#ifdef __cplusplus
}
#endif

#endif
