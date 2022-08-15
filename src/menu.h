#ifndef MENU_H
#define MENU_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t *menu_Looks(uint8_t *, uint8_t, uint8_t, uint8_t, bool);

bool menu_Info(uint8_t *, uint8_t, bool);

void menu_Settings(uint8_t);

#ifdef __cplusplus
}
#endif

#endif
