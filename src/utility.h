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

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NOPROGS         fileNumbers[0]
#define NOAPPVARS       fileNumbers[1]
#define NOAPPS          fileNumbers[2]

#define OS_TYPE_APP     14  // This should be a part of the toolchain

// Cesium file types (codes are the same)
#define ASM_TYPE        0
#define C_TYPE          1
#define BASIC_TYPE      2
#define ICE_TYPE        3
#define ICE_SRC_TYPE    4
#define DIR_TYPE        5   // For appvars folder?
#define APPVAR_TYPE     6
#define HIDDEN_TYPE     7   // Top Secret Government Stuff
#define CELTIC_TYPE     8   // Celtic appvar
#define APP_TYPE        9

#define CELTIC_HEADER   0x3e, 0x43, 0x45, 0x4c, 0x3f

// Active directory
#define PROGRAMS_FOLDER 0
#define APPVARS_FOLDER  1
#define APPS_FOLDER     2

uint8_t util_SpaceSearch(const char *str, const uint8_t charPerLine);

void util_WritePrefs(uint8_t *colors, const uint8_t transitionSpeed, const bool is24Hour, const bool displayCEaShell,
const uint8_t getCSCHook, const bool editArchivedProg, const bool editLockedProg, const bool showHiddenProg,
const bool showFileCount, const bool hideBusyIndicator, const bool lowercase, const uint8_t apdTimer,
const unsigned int fileSelected, const unsigned int fileStartLoc, const uint8_t directory,
const bool showApps, const bool showAppvars, void ***programPtrs, void ***appvarPtrs, unsigned int *fileNumbers, const bool updateVAT);

void util_FilesInit(unsigned int *fileNumbers, const bool displayCEaShell, const bool showHiddenProg, const bool showApps, const bool showAppvars);

char *util_FileTypeToString(const uint8_t fileType, const bool abbreviated);

void util_PrintFreeRamRom(void);

void util_RunPrgm(unsigned int fileSelected, unsigned int fileStartLoc, void **programPtrs, const bool editLockedProg, const bool showApps, const bool showAppvars);

bool util_AlphaSearch(unsigned int *fileSelected, unsigned int *fileStartLoc, const uint8_t key, const unsigned int fileCount, const bool displayCEaShell, const uint8_t directory, const bool showApps, const bool showAppvars);

bool util_CheckNameExists(const char *name, const bool appvars);

uint8_t util_GetSingleKeyPress(void); // Returns a GetCSC code using the keypad.c stuff (written jacobly)

void util_RunApp(const unsigned int fileSelected, const bool displayCEaShell);  // Wrapper for executeApp

void util_Secret(uint8_t *colors);  // 🤫

#ifdef __cplusplus
}
#endif

#endif
