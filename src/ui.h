#ifndef UI_H
#define UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

void ui_StatusBar(uint8_t, bool, char *);

void ui_BottomBar(uint8_t, char *);

#ifdef __cplusplus
}
#endif

#endif
