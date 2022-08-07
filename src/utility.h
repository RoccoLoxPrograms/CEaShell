#ifndef UTILITY_H
#define UTILITY_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NOPROGS     fileNumbers[0]
#define NOAPPVARS   fileNumbers[1]

int8_t util_SpaceSearch(char *);

void util_Exit(uint8_t *, uint8_t, bool);

char *util_FixHiddenName(char *);

uint8_t *util_FilesInit(uint8_t *);

#ifdef __cplusplus
}
#endif

#endif
