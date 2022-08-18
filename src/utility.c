#include "utility.h"
#include "asm/sortVat.h"

#include <graphx.h>
#include <fileioc.h>

uint8_t util_SpaceSearch(char *str, uint8_t charPerLine) {
    for (int8_t k = charPerLine; k >= 0; k--) {
        if (str[k] == ' ') {
            return k + 1;
        }
    }
    return charPerLine;
}

void util_Exit(uint8_t *colors, uint8_t transitionSpeed, bool is24Hour) {
    uint8_t ceaShell[6];
    ceaShell[0] = colors[0];
    ceaShell[1] = colors[1];
    ceaShell[2] = colors[2];
    ceaShell[3] = colors[3];
    ceaShell[4] = transitionSpeed;
    ceaShell[5] = is24Hour;

    uint8_t slot = ti_Open("CEaShell", "w+");
    ti_Write(&ceaShell, 6, 1, slot);
    ti_SetArchiveStatus(true, slot);
}

void util_FilesInit(uint8_t *fileNumbers) {
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
        if (fileType == OS_TYPE_PRGM || fileType == OS_TYPE_PROT_PRGM) {
            NOPROGS++;
        } else if (fileType == OS_TYPE_APPVAR) {
            NOAPPVARS++;
        }
    }
}

char *util_FileTypeToString(uint8_t fileType, bool abbreviated) {
    char *fileTypeString= NULL;
    switch (fileType) {
        case ASM_TYPE:
            if (abbreviated) {  // Abbreviations are shown on the desktop, while the full type is shown in the info menu
                fileTypeString = "ASM";
            } else {
                fileTypeString = "eZ80";
            }
            break;
        case C_TYPE:
            fileTypeString = "C";   // C has no abbreviation, it's just "C"
            break;
        case BASIC_TYPE:
            if (abbreviated) {
                fileTypeString = "BSC";
            } else {
                fileTypeString = "TI-BASIC";
            }
            break;
        case ICE_TYPE:
            fileTypeString = "ICE";
            break;
        case ICE_SRC_TYPE:
            if (abbreviated) {
                fileTypeString = "SRC";
            } else {
                fileTypeString = "ICE Source";
            }
            break;
        case DIR_TYPE:
            fileTypeString = "Appvars"; // We'll use this for the Appvar folder later
            break;
        case APPVAR_TYPE:
            if (abbreviated) {
                fileTypeString = "VAR";
            } else {
                fileTypeString = "Appvar";
            }
            break;
        default:
            break;
    }

    return fileTypeString;
}
