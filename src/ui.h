#ifndef UI_H
#define UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

void ui_StatusBarBG(uint8_t, char*);

void ui_Battery(uint8_t, bool, int, uint8_t);

void ui_Clock(bool, int, uint8_t);

#ifdef __cplusplus
}
#endif

#endif
