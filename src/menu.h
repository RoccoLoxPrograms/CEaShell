#ifndef MENU_H
#define MENU_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void menu_Looks(uint8_t *, const uint8_t, const uint8_t, const unsigned int, const bool, const uint8_t, const bool, const bool);

void menu_Info(uint8_t *, bool *, uint8_t, const unsigned int, uint8_t *, const bool, const bool);

void menu_Settings(uint8_t);

#ifdef __cplusplus
}
#endif

#endif
