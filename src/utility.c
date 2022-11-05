/**
 * --------------------------------------
 * 
 * CEaShell Source Code - utility.c
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#include "utility.h"
#include "asm/sortVat.h"
#include "asm/fileOps.h"
#include "asm/hooks.h"
#include "asm/runProgram.h"

#include <graphx.h>
#include <keypadc.h>
#include <fileioc.h>
#include <string.h>

uint8_t util_SpaceSearch(const char *str, const uint8_t charPerLine) {
    for (int8_t k = charPerLine; k >= 0; k--) {
        if (str[k] == ' ') {
            return k + 1;
        }
    }
    return charPerLine - 2;
}

void util_WritePrefs(uint8_t *colors, const uint8_t transitionSpeed, const bool is24Hour, const bool displayCEaShell, const uint8_t getCSCHook, const bool editArchivedProg, const bool editLockedProg, const bool showHiddenProg, const bool showFileCount, const bool hideBusyIndicator, const bool lowercase, const uint8_t apdTimer, const unsigned int fileSelected, const unsigned int fileStartLoc) {
    uint8_t ceaShell[15];
    unsigned int scrollLoc[2];
    ceaShell[0] = colors[0];
    ceaShell[1] = colors[1];
    ceaShell[2] = colors[2];
    ceaShell[3] = colors[3];
    ceaShell[4] = transitionSpeed;
    ceaShell[5] = is24Hour;
    ceaShell[6] = displayCEaShell;
    ceaShell[7] = getCSCHook;
    ceaShell[8] = editArchivedProg;
    ceaShell[9] = editLockedProg;
    ceaShell[10] = showHiddenProg;
    ceaShell[11] = showFileCount;
    ceaShell[12] = hideBusyIndicator;
    ceaShell[13] = lowercase;
    ceaShell[14] = apdTimer;
    scrollLoc[0] = fileSelected;
    scrollLoc[1] = fileStartLoc;

    uint8_t slot = ti_Open("CEaShell", "w+");
    ti_Write(&ceaShell, 15, 1, slot);
    ti_Seek(15, SEEK_SET, slot);
    ti_Write(&scrollLoc, 6, 1, slot);
    ti_SetArchiveStatus(true, slot);
    ti_Close(slot);
}

void util_FilesInit(unsigned int *fileNumbers, const bool displayCEaShell, const bool showHiddenProg) {
    uint8_t fileType;
    char *fileName;
    void *vatPtr = NULL;
    sortVAT();
    NOPROGS = 1;    // Accounts for folder
    NOAPPVARS = 1;

    while ((fileName = ti_DetectAny(&vatPtr, NULL, &fileType))) {
        if (*fileName == '!' || *fileName == '#') {
            continue;
        }
        if (!displayCEaShell && !strcmp(fileName, "CEASHELL")) {
            continue;
        }
        if (!showHiddenProg && fileName[0] < 65) {
            continue;
        }
        if ((fileType == OS_TYPE_PRGM || fileType == OS_TYPE_PROT_PRGM) && getPrgmType(fileName, fileType) == HIDDEN_TYPE) {    // Program uses DCS "Hidden header"
            continue;
        }
        if (fileType == OS_TYPE_PRGM || fileType == OS_TYPE_PROT_PRGM) {
            NOPROGS++;
        } else if (fileType == OS_TYPE_APPVAR) {
            NOAPPVARS++;
        }
    }
}

char *util_FileTypeToString(const uint8_t fileType, const bool abbreviated) {
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

void util_PrintFreeRamRom(void) {
    unsigned int ramFree = os_MemChk(NULL) + getASMPrgmSize();
    os_ArcChk();
    gfx_PrintStringXY("RAM Free: ", 82, 205);
    gfx_PrintUInt(ramFree, 6);
    gfx_PrintStringXY("ROM Free: ", 82, 216);
    gfx_PrintInt(os_TempFreeArc, 7);
}

void util_RunPrgm(unsigned int fileSelected, const bool displayCEaShell, const bool editLockedProg) {
    gfx_End();
    uint8_t fileType; // Different from C, ICE, ASM, etc. This is stuff like OS_TYPE_APPVAR and OS_TYPE_PRGM
    unsigned int filesSearched = 0;
    char *fileName;
    void *vatPtr = NULL;
    while ((fileName = ti_DetectAny(&vatPtr, NULL, &fileType))) { // Suspiciously similar to the example in the docs :P
        if (*fileName == '!' || *fileName == '#') {
            continue;
        }
        if (!displayCEaShell && !strcmp(fileName, "CEASHELL")) {
            continue;
        }
        if ((fileType == OS_TYPE_PRGM || fileType == OS_TYPE_PROT_PRGM)) {
            if (fileSelected - 1 == filesSearched) {
                break;
            }
            filesSearched++;
        }
    }
    uint8_t shellType = getPrgmType(fileName, fileType);
    if (shellType == BASIC_TYPE) {
        installStopHook();
    }
    runProgram(fileName, fileType, shellType, editLockedProg);
    return;
}

bool util_AlphaSearch(unsigned int *fileSelected, unsigned int *fileStartLoc, const uint8_t key, const unsigned int fileCount, const bool appvars, const bool displayCEaShell) {
    const char *alphabetCSC = "\0\0\0\0\0\0\0\0\0\0\0WRMH\0\0\0[VQLG\0\0\0ZUPKFC\0\0YTOJEBX\0XSNIDA\0\0\0\0\0\0\0\0";
    const char *alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ[";
    uint8_t fileType;
    unsigned int filesSearched = 1;  // ignore folder
    char *fileName;
    void *vatPtr = NULL;
    uint8_t alpha;
    for (alpha = 0; alpha < 27; alpha++) {
        if (alphabetCSC[key] == alphabet[alpha]) {
            break;
        }
    }
    while (alpha < 27) {
        while ((fileName = ti_DetectAny(&vatPtr, NULL, &fileType))) {
            if (*fileName == '!' || *fileName == '#') {
                continue;
            }
            if (!displayCEaShell && !strcmp(fileName, "CEASHELL")) {
                continue;
            }
            if ((fileType == OS_TYPE_PRGM || fileType == OS_TYPE_PROT_PRGM) && getPrgmType(fileName, fileType) == HIDDEN_TYPE) {
                continue;
            }
            if ((fileType == OS_TYPE_PRGM || fileType == OS_TYPE_PROT_PRGM) && !appvars) {
                if (fileName[0] + 64 * (fileName[0] < 65) == alphabet[alpha]) {
                    *fileSelected = filesSearched;
                    if (*fileSelected > *fileStartLoc + 7 || *fileSelected < *fileStartLoc) {
                        *fileStartLoc = filesSearched;
                        if (*fileStartLoc > fileCount - 5) {
                            *fileStartLoc = fileCount - 7;
                        }
                        if (*fileStartLoc % 2) {
                            *fileStartLoc = *fileStartLoc - 1;  // I had to do this instead of fileStartLoc--
                        }
                    }
                    return 1;
                }
                filesSearched++;
            } else if ((fileType == OS_TYPE_APPVAR) && appvars) {
                if (fileName[0] == alphabet[alpha]) {
                    *fileSelected = filesSearched;
                    if (*fileSelected > *fileStartLoc + 7 || *fileSelected < *fileStartLoc) {
                        *fileStartLoc = filesSearched;
                        if (*fileStartLoc > fileCount - 5) {
                            *fileStartLoc = fileCount - 7;
                        }
                        if (*fileStartLoc % 2) {
                            *fileStartLoc = *fileStartLoc - 1;
                        }
                    }
                    return 1;
                }
                filesSearched++;
            }
        }
        vatPtr = NULL;
        filesSearched = 1;
        alpha++;
    }

    return 0;
}

bool util_CheckNameExists(const char *name, const bool appvars) {
    uint8_t fileType;
    char *fileName;
    void *vatPtr = NULL;

    while ((fileName = ti_DetectAny(&vatPtr, NULL, &fileType))) {
        if ((fileType == OS_TYPE_PRGM || fileType == OS_TYPE_PROT_PRGM) && !appvars) {
            if (!strcmp(fileName, name)) {
                return true;
            }
        } else if ((fileType == OS_TYPE_APPVAR) && appvars) {
            if (!strcmp(fileName, name)) {
                return true;
            }
        }
    }
    return false;
}
