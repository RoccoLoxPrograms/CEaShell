#include "utility.h"

#include <fileioc.h>

int8_t util_SpaceSearch(char *str) {
    for (int8_t k = 23; k >= 0; k--) {
        if (str[k] == ' ') {
            return k + 1;
        }
    }
    return 24;
}

void util_Exit(uint8_t *colors, uint8_t transitionSpeed, bool is24Hour) {
    uint8_t ceaShell[6] = {0, 0, 0, 0, 0, 0};
    ceaShell[0] = colors[0];
    ceaShell[1] = colors[1];
    ceaShell[2] = colors[2];
    ceaShell[3] = colors[3];
    ceaShell[4] = transitionSpeed;
    ceaShell[5] = is24Hour;

    ti_var_t slot = ti_Open("CEASHELL", "w+");
    ti_Write(&ceaShell, 6, 1, slot);
    ti_SetArchiveStatus(true, slot);
}
