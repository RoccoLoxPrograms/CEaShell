#ifndef UTILITY_H
#define UTILITY_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

int8_t util_SpaceSearch(char *);

void util_Exit(uint8_t *, uint8_t, bool);

#ifdef __cplusplus
}
#endif

#endif
