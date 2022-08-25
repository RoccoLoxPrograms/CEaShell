#ifndef UTILITY_H
#define UTILITY_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NOPROGS         fileNumbers[0]
#define NOAPPVARS       fileNumbers[1]

// Cesium file types (codes are the same)
#define ASM_TYPE        0
#define C_TYPE          1
#define BASIC_TYPE      2
#define ICE_TYPE        3
#define ICE_SRC_TYPE    4
#define DIR_TYPE        5   // For appvars folder?
#define APPVAR_TYPE     6

uint8_t util_SpaceSearch(char *, uint8_t);

void util_WritePrefs(uint8_t *, uint8_t, bool);

void util_FilesInit(uint8_t *);

char *util_FileTypeToString(uint8_t, bool);

void util_PrintFreeRamRom(void);

void util_RunPrgm(unsigned int, unsigned int);

int util_EndPrgm(void *data, int retVal);

#ifdef __cplusplus
}
#endif

#endif
