/**
 * --------------------------------------
 * 
 * CEaShell Source Code - utility.c
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2024
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#include "defines.h"

#include "shapes.h"
#include "ui.h"
#include "utility.h"

#include "asm/apps.h"
#include "asm/fileOps.h"
#include "asm/fileSystem.h"
#include "asm/hooks.h"
#include "asm/spi.h"
#include "asm/utils.h"

#include <fileioc.h>
#include <keypadc.h>
#include <string.h>

#include <sys/power.h>
#include <ti/ui.h>

void util_ReadPrefs(struct preferences_t *shellPrefs, struct context_t *shellContext) {
    uint8_t slot = ti_Open(&rodata_appName, "r");
    uint8_t appvarVersion = 0;

    ti_Read(&appvarVersion, 1, 1, slot);
    ti_Seek(sizeof(uint8_t), SEEK_SET, slot);

    if (slot && (appvarVersion == APPVAR_VERSION) && ti_GetSize(slot) == APPVAR_SIZE) {
        ti_Read(shellPrefs, sizeof(struct preferences_t), 1, slot);
        ti_Seek(sizeof(struct preferences_t), SEEK_CUR, slot);
        ti_Read(&(shellContext->directory), sizeof(uint8_t) + sizeof(unsigned int) * 2, 1, slot);
    } else {
        shellPrefs->invertColors = false;
        shellPrefs->bgColor = 246;
        shellPrefs->fgColor = 237;
        shellPrefs->hlColor = 236;
        shellPrefs->textColor = 0;
        shellPrefs->hiddenTextColor = 181;
        shellPrefs->transitionSpeed = TRANSITION_MED;
        shellPrefs->uiScale = SCALE_LARGE;
        shellPrefs->timeFormat = true; // true 24 hour false 12 hour
        shellPrefs->showCEaShellApp = false;
        shellPrefs->showFileCount = false;
        shellPrefs->apdTimer = 3;
        shellPrefs->showAppsFolder = true;
        shellPrefs->showAppVarsFolder = true;
        shellPrefs->showHiddenProgs = true;
        shellPrefs->editArchivedProgs = 3;
        shellPrefs->editLockedProgs = true;
        shellPrefs->getCSCHooks = ALL_HOOKS;
        shellPrefs->hidePrgmOptions = 0;
        shellPrefs->hideBusyIndicator = false;
        shellPrefs->osLowercase = false;

        shellContext->directory = PROGRAMS_FOLDER;
        shellContext->fileSelected = 0;
        shellContext->fileStartLoc = 0;

        asm_spi_beginFrame();
        gfx_ZeroScreen();
        gfx_SetColor(237);
        shapes_RoundRectangleFill(6, 36, 76, 249, 89);
        gfx_SetColor(246);
        shapes_PixelIndentRectangle(42, 82, 237, 77);
        gfx_PrintStringXY("v"VERSION_NO, 117, 108);
        gfx_PrintStringXY("(C) Copyright 2022 - 2024", 48, 132);
        gfx_PrintStringXY("RoccoLox Programs, TIny_Hacker", 48, 144);
        gfx_SetTextScale(2, 2);
        gfx_PrintStringXY(&rodata_appName, 103, 88);
        asm_spi_endFrame();
        while (!kb_AnyKey());
        while (kb_AnyKey());
    }
}

void util_WritePrefs(struct preferences_t *shellPrefs, struct context_t *shellContext, const bool updateVAT) {
    asm_utils_toggleLowercase(shellPrefs->osLowercase);

    if (shellPrefs->getCSCHooks) {
        asm_hooks_installGetCSCHook(shellPrefs->getCSCHooks);
    } else {
        asm_hooks_removeGetCSCHook();
    }

    if (shellPrefs->editArchivedProgs || shellPrefs->hidePrgmOptions) {
        asm_hooks_installMenuHook(shellPrefs->editArchivedProgs ^ shellPrefs->hidePrgmOptions);
    } else {
        asm_hooks_removeMenuHook();
    }

    if (shellPrefs->editArchivedProgs || shellPrefs->editLockedProgs) {
        asm_hooks_installHomescreenHook();
    } else {
        asm_hooks_removeHomescreenHook();
    }

    uint8_t appvarVersion = APPVAR_VERSION;
    uint8_t slot = ti_Open(&rodata_appName, "w+");

    if (!slot || !asm_utils_checkEnoughRAM(APPVAR_SIZE)) {
        return;
    }

    ti_Write(&appvarVersion, sizeof(uint8_t), 1, slot);
    ti_Seek(sizeof(uint8_t), SEEK_SET, slot);
    ti_Write(shellPrefs, sizeof(struct preferences_t), 1, slot);
    ti_Seek(sizeof(struct preferences_t), SEEK_CUR, slot);
    ti_Write(&(shellContext->directory), sizeof(uint8_t) + sizeof(unsigned int) * 2, 1, slot);

    util_SafeArchive(slot);
    ti_Close(slot);

    if (updateVAT) {
        asm_fileSystem_sortVAT();
        util_FilesInit(shellPrefs, shellContext);
    }
}

void util_PowerOff(struct preferences_t *shellPrefs, struct context_t *shellContext) {
    util_SearchToMain(shellPrefs, shellContext);
    gfx_End();
    asm_hooks_triggerAPD();
}

void util_FilesInit(struct preferences_t *shellPrefs, struct context_t *shellContext) {
    asm_fileSystem_findAllVars(&(shellContext->programCount), &(shellContext->appVarCount), shellPrefs->showHiddenProgs, shellContext->searchString);
    shellContext->appCount = asm_apps_findAllApps();

    // Account for folders
    if (shellContext->searchString[0] == '\0') {
        shellContext->programCount += shellPrefs->showAppsFolder + shellPrefs->showAppVarsFolder;
    } else {
        shellContext->programCount += 1;
    }

    shellContext->appVarCount += 1;
    shellContext->appCount += shellPrefs->showCEaShellApp; // since we'd need to subtract one if we're hiding CEaShell anyway, this takes care of both at once

    shellContext->programPtrs = (void **)os_PixelShadow;
    asm_fileSystem_initPtrArrays(shellContext->programCount, shellContext->appVarCount, shellContext->appCount, &shellContext->appVarPtrs, &shellContext->appPtrs);

    if (shellContext->searchString[0] == '\0') {
        shellContext->programCount -= 2 * (shellPrefs->showHiddenProgs);
    }

    asm_fileSystem_getProgramPtrs(shellContext->programPtrs, shellPrefs->showHiddenProgs, shellContext->searchString);
    asm_fileSystem_getAppVarPtrs(shellContext->appVarPtrs, shellContext->searchString);
    asm_apps_getAppPtrs(shellContext->appPtrs, shellPrefs->showCEaShellApp);
}

void util_SetGFXChar(uint8_t charOffset, const uint8_t *charData, uint8_t charWidth) {
    asm_utils_setGfxCharWidth(*gfx_GetCharWidth, charOffset, charWidth);
    gfx_SetCharData(charOffset, charData);
}

void util_GetFileInfo(unsigned int file, struct file_t *fileInfo, struct preferences_t *shellPrefs, struct context_t *shellContext) {
    fileIcon->width = 16;
    fileIcon->height = 16;
    fileInfo->icon = fileIcon;
    fileInfo->shellType = UNKNOWN_TYPE;

    void *vatPtr = NULL;

    if (file <= 1) { // handle directories differently
        if (shellContext->directory == PROGRAMS_FOLDER && shellContext->searchString[0] == '\0') {
            if (file && shellPrefs->showAppsFolder && shellPrefs->showAppVarsFolder) {
                #ifdef FR
                memcpy(fileInfo->name, "AppVars", 8);
                memcpy(fileInfo->description, "Dossier AppVars", 16);
                #else
                memcpy(fileInfo->name, "AppVars", 8);
                memcpy(fileInfo->description, "AppVars folder", 15);
                #endif

                fileInfo->shellType = DIR_TYPE;
                fileInfo->size = shellContext->appVarCount;
            } else if (!file) {
                if (shellPrefs->showAppsFolder) {
                    #ifdef FR
                    memcpy(fileInfo->name, "Applis", 7);
                    memcpy(fileInfo->description, "Dossier d'applications", 23);
                    #else
                    memcpy(fileInfo->name, "Apps", 5);
                    memcpy(fileInfo->description, "Apps folder", 12);
                    #endif

                    fileInfo->shellType = DIR_TYPE;
                    fileInfo->size = shellContext->appCount;
                } else if (shellPrefs->showAppVarsFolder) {
                    #ifdef FR
                    memcpy(fileInfo->name, "AppVars", 8);
                    memcpy(fileInfo->description, "Dossier AppVars", 16);
                    #else
                    memcpy(fileInfo->name, "AppVars", 8);
                    memcpy(fileInfo->description, "AppVars folder", 15);
                    #endif

                    fileInfo->shellType = DIR_TYPE;
                    fileInfo->size = shellContext->appVarCount;
                }
            }
        } else if (!file && (shellContext->searchString[0] == '\0' || shellContext->directory == PROGRAMS_FOLDER)) {
            #ifdef FR
            memcpy(fileInfo->name, "Prgms", 6);
            memcpy(fileInfo->description, "Dossier de programmes", 22);
            #else
            memcpy(fileInfo->name, "Programs", 9);
            memcpy(fileInfo->description, "Programs folder", 16);
            #endif

            fileInfo->shellType = DIR_TYPE;
            fileInfo->size = shellContext->programCount;
        } else if (!file) {
            #ifdef FR
            memcpy(fileInfo->name, "AppVars", 8);
            memcpy(fileInfo->description, "Dossier AppVars", 16);
            #else
            memcpy(fileInfo->name, "AppVars", 8);
            memcpy(fileInfo->description, "AppVars folder", 15);
            #endif

            fileInfo->shellType = DIR_TYPE;
            fileInfo->size = shellContext->appVarCount;
        }
    }

    if (fileInfo->shellType != DIR_TYPE) {
        switch (shellContext->directory) {
            case PROGRAMS_FOLDER:
                if (shellContext->searchString[0] == '\0') {
                    vatPtr = shellContext->programPtrs[file - shellPrefs->showAppsFolder - shellPrefs->showAppVarsFolder];
                } else {
                    vatPtr = shellContext->programPtrs[file - 1];
                }

                memcpy(fileInfo->name, asm_utils_getFileName(vatPtr), 9);
                fileInfo->type = *(uint8_t *)(vatPtr);
                fileInfo->shellType = asm_fileOps_getPrgmType(vatPtr);
                break;
            case APPVARS_FOLDER:
                vatPtr = shellContext->appVarPtrs[file - 1];
                memcpy(fileInfo->name, asm_utils_getFileName(vatPtr), 9); // Subtract 1 to account for the "Programs" folder
                fileInfo->type = OS_TYPE_APPVAR;
                fileInfo->shellType = asm_fileOps_getAppVarType(vatPtr);
                break;
            case APPS_FOLDER:
                vatPtr = shellContext->appPtrs[file - 1];
                asm_apps_getAppName(vatPtr, fileInfo->name);
                fileInfo->type = OS_TYPE_APP;
                fileInfo->shellType = APP_TYPE;
                break;
            default:
                break;
        }
    }

    if (fileInfo->shellType == BASIC_TYPE) {
        if (!asm_fileOps_getIconDCS(vatPtr, fileInfo->icon)) {
            fileInfo->icon = NULL;
        }

        asm_fileOps_getDescBASIC(vatPtr, fileInfo->description);
    } else if (fileInfo->shellType == APP_TYPE) {
        if (!asm_apps_getAppIcon(vatPtr, fileInfo->icon)) {
            fileInfo->icon = NULL;
        }

        asm_apps_getAppCopyrightInfo(vatPtr, fileInfo->description);
    } else if (fileInfo->shellType != ICE_SRC_TYPE && fileInfo->shellType != DIR_TYPE) {
        if (!asm_fileOps_getIconASM(vatPtr, fileInfo->shellType, fileInfo->icon)) {
            fileInfo->icon = NULL;
        }

        asm_fileOps_getDescASM(vatPtr, fileInfo->shellType, fileInfo->description);
    } else {
        fileInfo->icon = NULL;
    }

    if (fileInfo->shellType == APP_TYPE) {
        fileInfo->minimumOSVersion = asm_apps_getAppMinOSVersion(shellContext->appPtrs[shellContext->fileSelected - 1]);
        fileInfo->size = asm_apps_getAppSize(vatPtr);
        fileInfo->archived = true;
    } else if (fileInfo->shellType != DIR_TYPE) {
        fileInfo->size = asm_fileOps_getPrgmSize(vatPtr);
        fileInfo->archived = asm_utils_isFileArchived(vatPtr);
    }

    fileInfo->hidden = (fileInfo->name[0] < 'A');
    fileInfo->locked = !(fileInfo->type == OS_TYPE_PRGM || fileInfo->shellType == CELTIC_TYPE);
}

unsigned int util_SpaceSearch(const char *string, const unsigned int start) {
    for (int k = start; k >= 0; k--) {
        if (string[k] == '\n') {
            return k + 1;
        }
    }

    for (int k = start; k >= 0; k--) {
        if (string[k] == ' ') {
            return k + 1;
        }
    }

    return start;
}

void util_AlphaSearch(struct preferences_t *shellPrefs, struct context_t *shellContext, char targetLetter) {
    static char currentName[9];
    void **filePtrs = *(&shellContext->programPtrs + shellContext->directory);
    uint8_t dirCount = 1;
    unsigned int fileCount = *(&(shellContext->programCount) + shellContext->directory); // Number of files in the current directory
    uint8_t rows = 6 / shellPrefs->uiScale + (shellPrefs->uiScale == 4);
    uint8_t columns = 4 - (shellPrefs->uiScale == 1);

    if (shellContext->directory == PROGRAMS_FOLDER && shellContext->searchString[0] == '\0') {
        dirCount = shellPrefs->showAppVarsFolder + shellPrefs->showAppsFolder;
    }

    if (targetLetter == '\0' || fileCount - dirCount == 0) {
        return;
    }

    if (shellContext->fileSelected < dirCount) { // Ensure jumping only works within the range of files, not folders
        shellContext->fileSelected = dirCount;
    }

    if (shellContext->directory == APPS_FOLDER) {
        asm_apps_getAppName(filePtrs[shellContext->fileSelected - dirCount], currentName);
    } else {
        memcpy(currentName, asm_utils_getFileName(filePtrs[shellContext->fileSelected - dirCount]), 9);
    }

    bool reverse = (targetLetter <= *currentName);

    while (!((shellContext->fileSelected - dirCount == 0 && reverse) || (shellContext->fileSelected == fileCount - 1 && !reverse))) {
        if (shellContext->directory == APPS_FOLDER) {
            asm_apps_getAppName(filePtrs[shellContext->fileSelected - dirCount], currentName);
        } else {
            memcpy(currentName, asm_utils_getFileName(filePtrs[shellContext->fileSelected - dirCount]), 9);
        }

        if (reverse) {
            if ((*currentName + 64 * (*currentName < 'A')) < targetLetter) {
                reverse = false;
            } else {
                if (!(shellContext->fileSelected + 1 - shellContext->fileStartLoc)) {
                    shellContext->fileStartLoc -= rows;
                }

                shellContext->fileSelected -= 1;
            }
        } else {
            if ((*currentName + 64 * (*currentName < 'A')) >= targetLetter) {
                break;
            } else {
                if (shellContext->fileSelected - shellContext->fileStartLoc == rows * columns) {
                    shellContext->fileStartLoc += rows;
                }

                shellContext->fileSelected += 1;
            }
        }
    }

    if (!(shellContext->fileSelected + 1 - shellContext->fileStartLoc)) {
        shellContext->fileStartLoc -= rows;
    } else if (shellContext->fileSelected - shellContext->fileStartLoc >= rows * columns) {
        shellContext->fileStartLoc += rows;
    }
}

void util_UpdateKeyTimer(struct preferences_t *shellPrefs, struct context_t *shellContext, clock_t *clockOffset, bool *keyPressed) {
    // Power off the calculator after specified time of inactivity or if the [on] key is pressed
    if ((clock() - *clockOffset >= ONE_MINUTE * shellPrefs->apdTimer && shellPrefs->apdTimer) || kb_On) {
        if (!boot_BatteryCharging()) {
            util_PowerOff(shellPrefs, shellContext);
        } else {
            kb_ClearOnLatch();
        }
    }

    if (!kb_AnyKey() && *keyPressed) {
        *keyPressed = false;
        *clockOffset = clock();
    }

    // Reset timer for inactivity when a key is pressed (the user is now active again)
    if (kb_AnyKey() && !(*keyPressed)) {
        *clockOffset = clock();
    }
}

void util_WaitBeforeKeypress(clock_t *clockOffset, bool *keyPressed) {
    if (!(*keyPressed)) {
        while ((clock() - *clockOffset < CLOCKS_PER_SEC / 3) && kb_AnyKey()) {
            kb_Scan();
        }
    }

    *keyPressed = true;
    *clockOffset = clock();
}

void util_SafeArchive(uint8_t slot) {
    void *vatPtr = ti_GetVATPtr(slot);

    if (asm_utils_willNotGC(vatPtr)) {
        ti_SetArchiveStatus(true, slot);
    } else {
        while (kb_AnyKey());
        ti_Close(slot);
        gfx_End();
        asm_utils_arcOnGC(vatPtr);
    }
}

void util_CorrectTransparentColor(struct preferences_t *shellPrefs) {
    uint8_t transparentColor = 0;

    while (transparentColor == shellPrefs->textColor || transparentColor == shellPrefs->hiddenTextColor) {
        transparentColor++;
    }

    gfx_SetTextBGColor(transparentColor);
    gfx_SetTextTransparentColor(transparentColor);
}

void util_CorrectCursorRemove(struct preferences_t *shellPrefs, struct context_t *shellContext) {
    util_WritePrefs(shellPrefs, shellContext, true);
    unsigned int fileCount = *(&(shellContext->programCount) + shellContext->directory);

    uint8_t rows = 6 / shellPrefs->uiScale + (shellPrefs->uiScale == 4);
    uint8_t columns = 4 - (shellPrefs->uiScale == 1);

    if (shellContext->fileSelected == fileCount) {
        shellContext->fileSelected -= 1;
    }

    if (shellContext->fileStartLoc && shellContext->fileStartLoc + rows * (columns - 1) >= fileCount) {
        shellContext->fileStartLoc -= rows;
    }

    util_WritePrefs(shellPrefs, shellContext, true);
}

void util_SearchToMain(struct preferences_t *shellPrefs, struct context_t *shellContext) {
    if (shellContext->searchString[0] != '\0') {
        shellContext->searchString[0] = '\0';

        void *selected = NULL;

        if (shellContext->directory == PROGRAMS_FOLDER) {
            selected = shellContext->programPtrs[shellContext->fileSelected - 1];
        } else {
            selected = shellContext->appVarPtrs[shellContext->fileSelected - 1];
        }

        util_FilesInit(shellPrefs, shellContext);

        unsigned int offset = asm_fileSystem_findArrayOffset(selected, *(&(shellContext->programPtrs) + shellContext->directory));
        uint8_t rows = 6 / shellPrefs->uiScale + (shellPrefs->uiScale == 4);
        uint8_t columns = 4 - (shellPrefs->uiScale == 1);

        if (shellContext->directory == PROGRAMS_FOLDER) {
            offset += shellPrefs->showAppsFolder + shellPrefs->showAppVarsFolder;
        } else {
            offset++;
        }

        shellContext->fileStartLoc = 0;
        shellContext->fileSelected = 0;

        for (unsigned int i = 0; i < offset; i++) {
            if (shellContext->fileSelected - shellContext->fileStartLoc == rows * columns) {
                shellContext->fileStartLoc += rows;
            }

            shellContext->fileSelected += 1;
        }

        if (!(shellContext->fileSelected + 1 - shellContext->fileStartLoc)) {
            shellContext->fileStartLoc -= rows;
        } else if (shellContext->fileSelected - shellContext->fileStartLoc >= rows * columns) {
            shellContext->fileStartLoc += rows;
        }
    }

    util_WritePrefs(shellPrefs, shellContext, true);
}
