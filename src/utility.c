#include "utility.h"
#include "asm/sortVat.h"

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
    uint8_t ceaShell[6];
    ceaShell[0] = colors[0];
    ceaShell[1] = colors[1];
    ceaShell[2] = colors[2];
    ceaShell[3] = colors[3];
    ceaShell[4] = transitionSpeed;
    ceaShell[5] = is24Hour;

    ti_var_t slot = ti_Open("CEaShell", "w+");
    ti_Write(&ceaShell, 6, 1, slot);
    ti_SetArchiveStatus(true, slot);
}

char *util_FixHiddenName(char *fileName) {
    if (fileName[0] < 65) {
        fileName[0] += 64;
    }
    
    return fileName;
}

uint8_t *util_FilesInit(uint8_t *fileNumbers) {
    uint8_t fileType;
    char *fileName;
    void *vatPtr = NULL;
    sortVAT();
    NOPROGS = 0;
    NOAPPVARS = 0;

    while ((fileName = ti_DetectAny(&vatPtr, NULL, &fileType))) {
        if (*fileName == '!' || *fileName == '#') {
            continue;
        }
        if (fileType == TI_PRGM_TYPE || fileType == TI_PPRGM_TYPE) {
            NOPROGS++;
        } else if (fileType == TI_APPVAR_TYPE) {
            NOAPPVARS++;
        }
    }

    return fileNumbers;
}
