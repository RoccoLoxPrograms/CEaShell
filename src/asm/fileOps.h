#ifndef FILEOPS_H
#define FILEOPS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t getPrgmType(char *, uint8_t);

void hidePrgm(char *, uint8_t);

void lockPrgm(char *, uint8_t);

#ifdef __cplusplus
}
#endif

#endif
