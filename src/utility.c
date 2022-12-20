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
#include "shapes.h"
#include "ui.h"
#include "asm/sortVat.h"
#include "asm/fileOps.h"
#include "asm/hooks.h"
#include "asm/runProgram.h"
#include "asm/apps.h"
#include "asm/getVATPtrs.h"
#include "asm/misc.h"

#include <graphx.h>
#include <keypadc.h>
#include <fileioc.h>
#include <string.h>
#include <sys/timers.h>
#include <sys/util.h>
#include <ti/screen.h>

uint8_t util_SpaceSearch(const char *str, const uint8_t charPerLine) {
    for (int8_t k = charPerLine; k >= 0; k--) {
        if (str[k] == ' ') {
            return k + 1;
        }
    }

    return charPerLine - 2;
}

// This one is hideous but whatever
void util_WritePrefs(uint8_t *colors, const uint8_t transitionSpeed, const bool is24Hour, const bool displayCEaShell,
const uint8_t getCSCHook, const bool editArchivedProg, const bool editLockedProg, const bool showHiddenProg,
const bool showFileCount, const bool hideBusyIndicator, const bool lowercase, const uint8_t apdTimer,
const unsigned int fileSelected, const unsigned int fileStartLoc, const uint8_t directory,
const bool showApps, const bool showAppvars, void ***programPtrs, void ***appvarPtrs, unsigned int *fileNumbers, const bool updateVAT) {

    uint8_t ceaShell[17];
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
    ceaShell[15] = showApps;
    ceaShell[16] = showAppvars;
    scrollLoc[0] = fileSelected;
    scrollLoc[1] = fileStartLoc;

    uint8_t slot = ti_Open("CEaShell", "w+");
    ti_Write(&ceaShell, 17, 1, slot);
    ti_Seek(17, SEEK_SET, slot);
    ti_Write(&scrollLoc, 6, 1, slot);
    ti_Seek(6, SEEK_CUR, slot);
    ti_Write(&directory, 1, 1, slot);

    if (willNotGarbageCollect("CEaShell", OS_TYPE_APPVAR)) {
        ti_SetArchiveStatus(true, slot);
    } else {
        gfx_End();
        os_DrawStatusBar();
        ti_SetArchiveStatus(true, slot);
        ti_Close(slot);
        reloadApp();
    }

    ti_Close(slot);
    sortVAT();

    if (updateVAT) {
        free(*programPtrs);
        free(*appvarPtrs);

        *programPtrs = malloc(NOPROGS * 3);
        *appvarPtrs = malloc(NOAPPVARS * 3);

        getProgramPtrs(*programPtrs, !showHiddenProg);
        getAppVarPtrs(*appvarPtrs);
    }
}

void util_FilesInit(unsigned int *fileNumbers, const bool displayCEaShell, const bool showHiddenProg, const bool showApps, const bool showAppvars) {
    uint8_t fileType;
    char *fileName;
    void *vatPtr = NULL;
    sortVAT();
    NOPROGS = showApps + showAppvars;    // Accounts for folders
    NOAPPVARS = 1;
    NOAPPS = 1;

    while ((fileName = ti_DetectAny(&vatPtr, NULL, &fileType))) {
        if (*fileName == '!' || *fileName == '#') {
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

    char appName[9] = "\0";
    unsigned int appPointer;    // Just a number, not a "pointer" (But the number is the pointer)
    while ((detectApp(appName, &appPointer))) {
        if (!displayCEaShell && !strcmp(appName, "CEaShell")) {
            continue;
        } else {
            NOAPPS++;
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
        case APPVAR_TYPE:
            if (abbreviated) {
                fileTypeString = "VAR";
            } else {
                fileTypeString = "Appvar";
            }

            break;
        case CELTIC_TYPE:
            if (abbreviated) {
                fileTypeString = "CLV";
            } else {
                fileTypeString = "Celtic Appvar";
            }

            break;
        case APP_TYPE:
            if (abbreviated) {
                fileTypeString = "APP";
            } else {
                fileTypeString = "Application";
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

void util_RunPrgm(unsigned int fileSelected, unsigned int fileStartLoc, void **programPtrs, const bool editLockedProg, const bool showApps, const bool showAppvars) {
    gfx_End();
    uint8_t fileType; // Different from C, ICE, ASM, etc. This is stuff like OS_TYPE_APPVAR and OS_TYPE_PRGM
    unsigned int filesSearched = showApps + showAppvars; // Account for folders

    if (fileStartLoc != 0) {
        filesSearched = fileStartLoc;
    }

    char *fileName;
    void *vatPtr = programPtrs[fileStartLoc - ((showApps + showAppvars) * (fileStartLoc > 0))];

    while ((fileName = ti_DetectAny(&vatPtr, NULL, &fileType))) { // Suspiciously similar to the example in the docs :P
        if (*fileName == '!' || *fileName == '#') {
            continue;
        }

        if ((fileType == OS_TYPE_PRGM || fileType == OS_TYPE_PROT_PRGM)) {
            if (fileSelected == filesSearched) {
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

bool util_AlphaSearch(unsigned int *fileSelected, unsigned int *fileStartLoc, const uint8_t key, const unsigned int fileCount, const bool displayCEaShell, const uint8_t directory, const bool showApps, const bool showAppvars) {
    const char *alphabetCSC = "\0\0\0\0\0\0\0\0\0\0\0WRMH\0\0\0[VQLG\0\0\0ZUPKFC\0\0YTOJEBX\0XSNIDA\0\0\0\0\0\0\0\0";
    const char *alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ[";   // '[' is actually the 'θ' symbol, but graphx thinks it's '['
    uint8_t fileType;
    unsigned int filesSearched = 1;  // ignore folder

    if (directory == PROGRAMS_FOLDER) {
        filesSearched = (showApps + showAppvars);
    }

    char *fileName;
    void *vatPtr = NULL;
    uint8_t alpha;
    bool reverse = false;

    for (alpha = 0; alpha < 27; alpha++) {
        if (alphabetCSC[key] == alphabet[alpha]) {
            break;
        }
    }

    while (alpha != (27 * !reverse)) {
        if (directory == APPS_FOLDER) {
            char appName[9] = "\0";
            unsigned int appPointer;
            while (detectApp(appName, &appPointer)) {
                if (!displayCEaShell && !strcmp(appName, "CEaShell")) {
                    continue;
                }

                if (appName[0] == alphabet[alpha]) {
                    *fileSelected = filesSearched;
                    if (*fileSelected > *fileStartLoc + 7 || *fileSelected < *fileStartLoc) {
                        *fileStartLoc = filesSearched;

                        if (*fileStartLoc > fileCount - 7) {
                            *fileStartLoc = fileCount - 7;
                        }

                        if (*fileStartLoc % 2) {
                            *fileStartLoc = *fileStartLoc - 1;
                        }
                    }

                    return true;
                }

                filesSearched++;
            }
        } else {
            while ((fileName = ti_DetectAny(&vatPtr, NULL, &fileType))) {
                if (*fileName == '!' || *fileName == '#') {
                    continue;
                }

                if ((fileType == OS_TYPE_PRGM || fileType == OS_TYPE_PROT_PRGM) && getPrgmType(fileName, fileType) == HIDDEN_TYPE) {
                    continue;
                }

                if ((fileType == OS_TYPE_PRGM || fileType == OS_TYPE_PROT_PRGM) && directory == PROGRAMS_FOLDER) {
                    if (fileName[0] + 64 * (fileName[0] < 65) == alphabet[alpha]) { // Check the first letter
                        *fileSelected = filesSearched;

                        if (*fileSelected > *fileStartLoc + 7 || *fileSelected < *fileStartLoc) {
                            *fileStartLoc = filesSearched;

                            if (*fileStartLoc > fileCount - 7) {
                                *fileStartLoc = fileCount - 7;
                            }

                            if (*fileStartLoc % 2) {
                                *fileStartLoc = *fileStartLoc - 1;
                            }
                        }

                        return true;
                    }

                    filesSearched++;
                } else if ((fileType == OS_TYPE_APPVAR) && directory == APPVARS_FOLDER) {
                    if (fileName[0] == alphabet[alpha]) {
                        *fileSelected = filesSearched;

                        if (*fileSelected > *fileStartLoc + 7 || *fileSelected < *fileStartLoc) {
                            *fileStartLoc = filesSearched;

                            if (*fileStartLoc > fileCount - 7) {
                                *fileStartLoc = fileCount - 7;
                            }

                            if (*fileStartLoc % 2) {
                                *fileStartLoc = *fileStartLoc - 1;
                            }
                        }

                        return true;
                    }

                    filesSearched++;
                }
            }
        }

        vatPtr = NULL;
        filesSearched = 1;

        if (directory == PROGRAMS_FOLDER) {
            filesSearched = (showApps + showAppvars);
        }

        alpha += 1 + (-2 * reverse);    // If the letter is after all existing filenames, go backwards to the last name

        if (alpha == 27) {
            reverse = true;
        } 
    }

    return false;
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

// code by jacobly from here:
// https://ce-programming.github.io/toolchain/libraries/keypadc.html#getting-getcsc-codes-with-keypadc
uint8_t util_GetSingleKeyPress(void) {
    static uint8_t last_key;
    uint8_t only_key = 0;
    kb_Scan();

    for (uint8_t key = 1, group = 7; group; --group) {
        for (uint8_t mask = 1; mask; mask <<= 1, ++key) {
            if (kb_Data[group] & mask) {
                if (only_key) {
                    last_key = 0;
                    return false;
                } else {
                    only_key = key;
                }
            }
        }
    }

    if (only_key == last_key) {
        return false;
    }

    last_key = only_key;
    return only_key;
}

void util_RunApp(const unsigned int fileSelected, const bool displayCEaShell) {
    while (kb_AnyKey());

    unsigned int filesSearched = 1; // account for folders
    char appName[9] = "\0";
    unsigned int appPointer;    // Will we ever use this? 🤷‍♂️

    while (detectApp(appName, &appPointer)) {
        if (!displayCEaShell && !strcmp(appName, "CEaShell")) {
            continue;
        }

        if (fileSelected == filesSearched) {
            break;
        }

        filesSearched++;
    }

    gfx_End();
    executeApp(appName);
}

void util_Secret(uint8_t *colors) { // 🤫
    gfx_SetDrawBuffer();
    gfx_SetTextScale(2, 2);
    shapes_RoundRectangleFill(colors[0], 7, 290, 155, 15, 46);
    gfx_SetColor(colors[1]);
    gfx_FillRectangle_NoClip(101, 8, 116, 16);
    gfx_PrintStringXY("Easter Egg", 87, 8);
    gfx_BlitBuffer();
    timer_Set(1, 0);

    while(kb_AnyKey());

    srandom(1337);  // Secret number
    uint8_t radiusMax;
    uint8_t radius;
    unsigned int x;
    unsigned int titleX;
    uint8_t y;

    while (!kb_IsDown(kb_KeyAlpha) && !kb_IsDown(kb_KeyClear) && !kb_IsDown(kb_KeyGraph)) {
        kb_Scan();

        if (kb_On) {
            triggerAPD();
        }

        if (kb_IsDown(kb_KeyAlpha) || kb_IsDown(kb_KeyClear) || kb_IsDown(kb_KeyGraph)) {
            gfx_SetColor(colors[1]);
            gfx_SetTextScale(2, 2);
            gfx_FillRectangle_NoClip(87, 8, 146, 16);
            gfx_PrintStringXY("Settings", 101, 8);
            return;
        }

        if (kb_AnyKey()) {
            shapes_RoundRectangleFill(colors[0], 7, 290, 155, 15, 46);
            gfx_BlitBuffer();
            while(kb_AnyKey());
        }

        if (timer_Get(1) > 1000) {
            radiusMax = randInt(4, 22);
            gfx_SetColor(colors[randInt(0, 2)]);
            x = randInt(39, 281);
            y = randInt(70, 177);

            for (radius = 0; radius < radiusMax; radius++) {    // Circle fill animation
                gfx_FillCircle_NoClip(x, y, radius);
                gfx_SetTextScale(2, 2);
                titleX = 160 - gfx_GetStringWidth("CEaShell v"VERSION_NO) / 2;  // Versions can be different, so we need to make sure it's properly centered
                gfx_PrintStringXY("CEaShell v"VERSION_NO, titleX, 100);
                gfx_SetTextScale(1, 1);
                gfx_PrintStringXY("By RoccoLox Programs", 85, 122);
                gfx_PrintStringXY("and TIny_Hacker", 102, 135);
                gfx_BlitBuffer();
            }

            timer_Set(1, 0);
        }
    }
}
