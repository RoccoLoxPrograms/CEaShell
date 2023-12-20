/**
 * --------------------------------------
 * 
 * CEaShell Source Code - utility.c
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2023
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#include "defines.h"
#include "utility.h"

#include "asm/apps.h"
#include "asm/fileOps.h"
#include "asm/fileSystem.h"
#include "asm/hooks.h"
#include "asm/runProgram.h"
#include "asm/utils.h"

#include <fileioc.h>
#include <graphx.h>
#include <keypadc.h>
#include <string.h>
#include <time.h>

#include <ti/ui.h>

void util_ReadPrefs(struct preferences_t *shellPrefs, struct context_t *shellContext) {
    uint8_t slot = ti_Open("CEaShell", "r");
    uint8_t appvarVersion = 0;

    ti_Read(&appvarVersion, 1, 1, slot);
    ti_Seek(sizeof(uint8_t), SEEK_SET, slot);

    if (slot && (appvarVersion == APPVAR_VERSION)) {
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
        shellPrefs->getCSCHooks = BOTH;
        shellPrefs->hidePrgmOptions = 0;
        shellPrefs->hideBusyIndicator = false;
        shellPrefs->osLowercase = false;

        shellContext->directory = PROGRAMS_FOLDER;
        shellContext->fileSelected = 0;
        shellContext->fileStartLoc = 0;
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

    if (shellPrefs->editArchivedProgs) {
        asm_hooks_installHomescreenHook();
    } else {
        asm_hooks_removeHomescreenHook();
    }

    uint8_t appvarVersion = APPVAR_VERSION;
    uint8_t slot = ti_Open("CEaShell", "w+");

    ti_Write(&appvarVersion, sizeof(uint8_t), 1, slot);
    ti_Seek(sizeof(uint8_t), SEEK_SET, slot);
    ti_Write(shellPrefs, sizeof(struct preferences_t), 1, slot);
    ti_Seek(sizeof(struct preferences_t), SEEK_CUR, slot);
    ti_Write(&(shellContext->directory), sizeof(uint8_t) + sizeof(unsigned int) * 2, 1, slot);

    util_SafeArchive(slot, "CEaShell", OS_TYPE_APPVAR);

    ti_Close(slot);

    if (updateVAT) {
        free(shellContext->programPtrs);
        free(shellContext->appVarPtrs);
        free(shellContext->appPtrs);
        util_FilesInit(shellPrefs, shellContext);
    }
}

void util_PowerOff(struct preferences_t *shellPrefs, struct context_t *shellContext) {
    util_WritePrefs(shellPrefs, shellContext, false);
    gfx_End();
    asm_hooks_triggerAPD();
}

void util_FilesInit(struct preferences_t *shellPrefs, struct context_t *shellContext) {
    asm_fileSystem_sortVAT();
    asm_fileSystem_findAllVars(&(shellContext->programCount), &(shellContext->appVarCount), shellPrefs->showHiddenProgs);
    shellContext->appCount = asm_apps_findAllApps();

    // Account for folders
    shellContext->programCount += shellPrefs->showAppsFolder + shellPrefs->showAppVarsFolder;
    shellContext->appVarCount++;
    shellContext->appCount += shellPrefs->showCEaShellApp; // since we'd need to subtract one if we're hiding CEaShell anyway, this takes care of both at once

    shellContext->programPtrs = malloc(shellContext->programCount * sizeof(void *));
    shellContext->appVarPtrs = malloc(shellContext->appVarCount * sizeof(void *));
    shellContext->appPtrs = malloc(shellContext->appCount * sizeof(void *));

    shellContext->programCount -= 2 * (shellPrefs->showHiddenProgs);

    asm_fileSystem_getProgramPtrs(shellContext->programPtrs, shellPrefs->showHiddenProgs);
    asm_fileSystem_getAppVarPtrs(shellContext->appVarPtrs);
    asm_apps_getAppPtrs(shellContext->appPtrs, shellPrefs->showCEaShellApp);
}

void util_SetGFXChar(uint8_t charOffset, const uint8_t *charData, uint8_t charWidth) {
    asm_utils_setGfxCharWidth(*gfx_GetCharWidth, charOffset, charWidth);
    gfx_SetCharData(charOffset, charData);
}

void util_GetFileInfo(unsigned int file, struct file_t *fileInfo, struct preferences_t *shellPrefs, struct context_t *shellContext) {
    fileInfo->name = malloc(9);
    fileInfo->icon = gfx_MallocSprite(16, 16);
    fileInfo->description = malloc(MAX_DESC_LENGTH);
    fileInfo->shellType = UNKNOWN_TYPE;

    if (file <= 1) { // handle directories differently
        if (shellContext->directory == PROGRAMS_FOLDER) {
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
                    memcpy(fileInfo->description, "Dossier d'applications", 12);
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
        } else if (!file) {
            #ifdef FR
            memcpy(fileInfo->name, "Prgms", 6);
            memcpy(fileInfo->description, "Dossier de programmes", 22);
            #else
            memcpy(fileInfo->name, "Programs", 9);
            memcpy(fileInfo->description, "Programs folder", 16);
            #endif

            fileInfo->shellType = DIR_TYPE;
            fileInfo->size = shellContext->programCount;
        }
    }

    if (fileInfo->shellType != DIR_TYPE) {
        switch (shellContext->directory) {
            case PROGRAMS_FOLDER:
                memcpy(fileInfo->name, asm_utils_getFileName(shellContext->programPtrs[file - (shellPrefs->showAppsFolder + shellPrefs->showAppVarsFolder)]), 9);
                fileInfo->type = *(uint8_t *)(shellContext->programPtrs[file - shellPrefs->showAppsFolder - shellPrefs->showAppVarsFolder]);
                fileInfo->shellType = asm_fileOps_getPrgmType(fileInfo->name, OS_TYPE_PRGM);
                break;
            case APPVARS_FOLDER:
                memcpy(fileInfo->name, asm_utils_getFileName(shellContext->appVarPtrs[file - 1]), 9); // Subtract 1 to account for the "Programs" folder
                fileInfo->type = OS_TYPE_APPVAR;
                fileInfo->shellType = asm_fileOps_getAppVarType(fileInfo->name);
                break;
            case APPS_FOLDER:
                asm_apps_getAppName(shellContext->appPtrs[file - 1], fileInfo->name);
                fileInfo->type = OS_TYPE_APP;
                fileInfo->shellType = APP_TYPE;
                break;
            default:
                break;
        }
    }

    if (fileInfo->shellType == BASIC_TYPE) {
        if (!asm_fileOps_getIconDCS(fileInfo->name, OS_TYPE_PRGM, fileInfo->icon)) {
            free(fileInfo->icon);
            fileInfo->icon = NULL;
        }

        if (!asm_fileOps_getDescBASIC(fileInfo->name, OS_TYPE_PRGM, fileInfo->description)) {
            free(fileInfo->description);
            fileInfo->description = NULL;
        }
    } else if (fileInfo->shellType == APP_TYPE) {
        if (!asm_apps_getAppIcon(shellContext->appPtrs[file - 1], fileInfo->icon)) {
            free(fileInfo->icon);
            fileInfo->icon = NULL;
        }

        if (!asm_apps_getAppCopyrightInfo(shellContext->appPtrs[file - 1], fileInfo->description)) {
            free(fileInfo->description);
            fileInfo->description = NULL;
        }
    } else if (fileInfo->shellType != ICE_SRC_TYPE && fileInfo->shellType != DIR_TYPE) {
        if (!asm_fileOps_getIconASM(fileInfo->name, OS_TYPE_PRGM + (OS_TYPE_APPVAR - OS_TYPE_PRGM) * (fileInfo->shellType == APPVAR_TYPE), fileInfo->shellType, fileInfo->icon)) {
            free(fileInfo->icon);
            fileInfo->icon = NULL;
        }

        if (!asm_fileOps_getDescASM(fileInfo->name, OS_TYPE_PRGM, fileInfo->shellType, fileInfo->description)) {
            free(fileInfo->description);
            fileInfo->description = NULL;
        }
    } else {
        free(fileInfo->icon);
        fileInfo->icon = NULL;

        if (fileInfo->shellType != DIR_TYPE) {
            free(fileInfo->description);
            fileInfo->description = NULL;
        }
    }

    if (fileInfo->shellType == APP_TYPE) {
        fileInfo->size = asm_apps_getAppSize(shellContext->appPtrs[shellContext->fileSelected - 1]);
        fileInfo->archived = true;
    } else if (fileInfo->shellType != DIR_TYPE) {
        fileInfo->size = asm_fileOps_getPrgmSize(fileInfo->name, fileInfo->type);
        fileInfo->archived = asm_utils_isFileArchived(fileInfo->name, fileInfo->type);
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
    char *currentName = NULL;
    void **filePtrs = *(&shellContext->programPtrs + shellContext->directory);
    uint8_t dirCount = 1;
    unsigned int fileCount = *(&(shellContext->programCount) + shellContext->directory); // Number of files in the current directory
    uint8_t rows = 6 / shellPrefs->uiScale + (shellPrefs->uiScale == 4);
    uint8_t columns = 4 - (shellPrefs->uiScale == 1);

    if (shellContext->directory == PROGRAMS_FOLDER) {
        dirCount = shellPrefs->showAppVarsFolder + shellPrefs->showAppsFolder;
    }

    if (targetLetter == '\0' || fileCount - dirCount == 0) {
        return;
    }

    if (shellContext->fileSelected < dirCount) { // Ensure jumping only works within the range of files, not folders
        shellContext->fileSelected = dirCount;
    }

    if (shellContext->directory == APPS_FOLDER) {
        currentName = malloc(sizeof(char) * 9);
        asm_apps_getAppName(filePtrs[shellContext->fileSelected - dirCount], currentName);
    } else {
        currentName = asm_utils_getFileName(filePtrs[shellContext->fileSelected - dirCount]);
    }

    bool reverse = (targetLetter <= *currentName);

    while (!((shellContext->fileSelected - dirCount == 0 && reverse) || (shellContext->fileSelected == fileCount - 1 && !reverse))) {
        if (shellContext->directory == APPS_FOLDER) {
            asm_apps_getAppName(filePtrs[shellContext->fileSelected - dirCount], currentName);
        } else {
            currentName = asm_utils_getFileName(filePtrs[shellContext->fileSelected - dirCount]);
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
                if (shellContext->directory == APPS_FOLDER) {
                    free(currentName);
                }

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

    if (shellContext->directory == APPS_FOLDER) {
        free(currentName);
    }
}

void util_UpdateKeyTimer(struct preferences_t *shellPrefs, struct context_t *shellContext, clock_t *clockOffset, bool *keyPressed) {
    // Power off the calculator after specified time of inactivity or if the [on] key is pressed
    if ((clock() - *clockOffset >= ONE_MINUTE * shellPrefs->apdTimer && shellPrefs->apdTimer) || kb_On) {
        util_PowerOff(shellPrefs, shellContext);
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
        while ((clock() - *clockOffset < CLOCKS_PER_SEC / 4) && kb_AnyKey()) {
            kb_Scan();
        }
    }

    *keyPressed = true;
    *clockOffset = clock();
}

void util_SafeArchive(uint8_t slot, char *fileName, uint8_t type) {
    if (asm_utils_willNotGC(fileName, type)) {
        ti_SetArchiveStatus(true, slot);
    } else {
        gfx_End();
        os_DrawStatusBar();
        ti_SetArchiveStatus(true, slot);
        ti_Close(slot);
        asm_apps_reloadApp();
    }
}
