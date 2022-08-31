#include "utility.h"
#include "main.h"
#include "asm/sortVat.h"
#include "asm/fileOps.h"
#include "asm/hooks.h"

#include <graphx.h>
#include <keypadc.h>
#include <fileioc.h>

uint8_t util_SpaceSearch(char *str, uint8_t charPerLine) {
    for (int8_t k = charPerLine; k >= 0; k--) {
        if (str[k] == ' ') {
            return k + 1;
        }
    }
    return charPerLine - 2;
}

void util_WritePrefs(uint8_t *colors, uint8_t transitionSpeed, bool is24Hour) {
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
    ti_Close(slot);
}

void util_FilesInit(uint8_t *fileNumbers) {
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

void util_PrintFreeRamRom(void) {
    unsigned int ramFree = os_MemChk(NULL) + os_AsmPrgmSize;
    os_ArcChk();
    gfx_PrintStringXY("RAM Free: ", 82, 205);
    gfx_PrintUInt(ramFree, 6);
    gfx_PrintStringXY("ROM Free: ", 82, 216);
    gfx_PrintInt(os_TempFreeArc, 7);
}

void util_RunPrgm(unsigned int fileSelected, unsigned int fileStartLoc) {
    gfx_End();
    uint8_t fileType; // Different from C, ICE, ASM, etc. This is stuff like OS_TYPE_APPVAR and OS_TYPE_PRGM
    unsigned int filesSearched = 0;
    char *fileName;
    void *vatPtr = NULL;
    while ((fileName = ti_DetectAny(&vatPtr, NULL, &fileType))) { // Suspiciously similar to the example in the docs :P
        if ((fileType == OS_TYPE_PRGM || fileType == OS_TYPE_PROT_PRGM)) {
            if (fileSelected - 1 == filesSearched) {
                break;
            }
            filesSearched++;
        }
    }
    if (getPrgmType(fileName, fileType) == BASIC_TYPE) {
        installStopHook();
    }
    unsigned int returnInfo[] = {fileSelected, fileStartLoc};
    os_RunPrgm(fileName, (void *)returnInfo, 6, util_EndPrgm);
}

int util_EndPrgm(void *data, int retVal) {
    (void)(retVal); // Ignore this for now
    removeStopHook();
    while(kb_AnyKey());
    shellMain(*(unsigned int*)&data[0], *(unsigned int *)&data[3]);    // Typecasting brings us home to the shell (data[3] is the 3rd byte of data, since it's a void *)
    return 0;
}

bool util_AlphaSearch(unsigned int *fileSelected, unsigned int *fileStartLoc, uint8_t key, unsigned int fileCount, bool appvars) {
    const char *alphabet = "\0\0\0\0\0\0\0\0\0\0\0WRMH\0\0\0[VQLG\0\0\0ZUPKFC\0\0YTOJEBX\0XSNIDA\0\0\0\0\0\0\0\0";
    uint8_t fileType;
    unsigned int filesSearched = 1;  // ignore folder
    char *fileName;
    void *vatPtr = NULL;
    while ((fileName = ti_DetectAny(&vatPtr, NULL, &fileType))) {
        if ((fileType == OS_TYPE_PRGM || fileType == OS_TYPE_PROT_PRGM) && !appvars) {
            if (fileName[0] + 64 * (fileName[0] < 65) == alphabet[key]) {
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
            if (fileName[0] == alphabet[key]) {
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

    return 0;
}
