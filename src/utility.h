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

uint8_t util_SpaceSearch(char *);

void util_Exit(uint8_t *, uint8_t, bool);

uint8_t *util_FilesInit(uint8_t *);

char *util_FileTypeToString(uint8_t);

#ifdef __cplusplus
}
#endif

#endif
