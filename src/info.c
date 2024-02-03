/**
 * --------------------------------------
 * 
 * CEaShell Source Code - info.c
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2024
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#include "defines.h"
#include "info.h"
#include "menu.h"
#include "shapes.h"
#include "ui.h"
#include "utility.h"

#include "asm/fileOps.h"
#include "asm/runProgram.h"
#include "asm/utils.h"

#include <fileioc.h>
#include <keypadc.h>
#include <string.h>

#include <ti/ui.h>

void info_Redraw(struct preferences_t *shellPrefs, struct file_t *fileInfo, uint8_t option) {
    gfx_SetColor(shellPrefs->fgColor);
    gfx_FillRectangle_NoClip(56, 45, 208, 158);
    gfx_SetColor(shellPrefs->bgColor);
    shapes_RoundRectangleFill(12, 56, 77, 138, 56);
    shapes_RoundRectangleFill(12, 56, 115, 208, 88);
    gfx_FillRectangle_NoClip(194, 103, 12, 12);
    gfx_SetColor(shellPrefs->fgColor);
    gfx_FillCircle_NoClip(209, 99, 15);
    gfx_SetColor(shellPrefs->hlColor);
    shapes_RoundRectangleFill(12, 56, 45, 138, 26);

    if (fileInfo->icon != NULL) {
        gfx_ScaledSprite_NoClip(fileInfo->icon, 200, 45, 4, 4);
    } else {
        gfx_FillRectangle_NoClip(200, 45, 64, 64);

        if (fileInfo->shellType == DIR_TYPE) {
            shapes_Folder(4, 212, 61);
        } else {
            shapes_FileIcon(4, fileInfo->shellType, 216, 57);
        }
    }

    gfx_sprite_t *corner1 = gfx_MallocSprite(12, 12);
    shapes_GetRoundCorners(corner1, shellPrefs->fgColor, 12, 200, 45);
    shapes_DrawRoundCorners(corner1, 64, 64, 200, 45);
    free(corner1);

    *fileInfo->name += 64 * (*fileInfo->name < 'A');
    ui_CenterStringBig(fileInfo->name, 125, 51);

    if (fileInfo->hidden) {
        *fileInfo->name ^= 64;
    }


    #ifdef FR
    static const char *fileTypeStrings[11] = {"ASM", "C", "TI-BASIC", "ICE", "ICE Source", "Directory", "AppVar", NULL, "Celtic Var", "App", "Unknown"};
    gfx_PrintStringXY("Type: ", 64, 85);
    #else
    static const char *fileTypeStrings[11] = {"ASM", "C", "TI-BASIC", "ICE", "ICE Source", "Directory", "AppVar", NULL, "Celtic Var", "App", "Unknown"};
    gfx_PrintStringXY("Type: ", 64, 85);
    #endif

    gfx_PrintString(fileTypeStrings[fileInfo->shellType]);
    gfx_SetTextXY(64, 98);

    #ifdef FR
    if (fileInfo->shellType != DIR_TYPE) {
        gfx_PrintString("Size: ");
    } else {
        gfx_PrintString("Items: ");
    }
    #else
    if (fileInfo->shellType != DIR_TYPE) {
        gfx_PrintString("Size: ");
    } else {
        gfx_PrintString("Items: ");
    }
    #endif

    gfx_PrintUInt(fileInfo->size, 5 + (fileInfo->shellType == APP_TYPE) - (fileInfo->shellType == DIR_TYPE));

    #ifdef FR
    gfx_PrintStringXY("Description:", 64, 111);
    #else
    gfx_PrintStringXY("Description:", 64, 111);
    #endif

    if (fileInfo->description != NULL) {
        ui_PrintStringWrap(fileInfo->description, 64, 123, 28, 2);
    } else {
        #ifdef FR
        gfx_PrintStringXY("No description.", 64, 123);
        #else
        gfx_PrintStringXY("No description.", 64, 123);
        #endif
    }

    gfx_SetColor(shellPrefs->hlColor);

    if (fileInfo->shellType != DIR_TYPE) {
        if (fileInfo->shellType != APP_TYPE) {
            if (option > 2) {
                shapes_PixelIndentRectangle(68 + 65 * (option - 3), 189, 54, 9);
            } else {
                shapes_PixelIndentRectangle(63 + 76 * (option > 0) + 63 * (option > 1), 160, 9, 9);
            }

            #ifdef FR
            gfx_PrintStringXY("Attributes:", 64, 148);
            gfx_PrintStringXY("Archived     Locked     Hidden", 74, 161);
            gfx_PrintStringXY("Delete      Rename         Edit", 74, 190);
            #else
            gfx_PrintStringXY("Attributes:", 64, 148);
            gfx_PrintStringXY("Archived     Locked     Hidden", 74, 161);
            gfx_PrintStringXY("Delete      Rename         Edit", 74, 190);
            #endif

            for (uint8_t i = 0; i < 3; i++) {
                ui_CheckBox(*(&fileInfo->archived + i), 64 + 76 * (i > 0) + 63 * (i > 1), 161);
            }
        } else {
            shapes_PixelIndentRectangle(68, 161, 53, 9);
            #ifdef FR
            gfx_PrintStringXY("Delete", 74, 162);
            #else
            gfx_PrintStringXY("Delete", 74, 162);
            #endif
        }

        #ifdef FR
        gfx_PrintStringXY("File Operations:", 64, 176 - 28 * (fileInfo->shellType == APP_TYPE));
        #else
        gfx_PrintStringXY("File Operations:", 64, 176 - 28 * (fileInfo->shellType == APP_TYPE));
        #endif
    }
}

void info_Open(struct preferences_t *shellPrefs, struct context_t *shellContext, unsigned int *fileCount) {
    util_WritePrefs(shellPrefs, shellContext, true);
    uint8_t slot;
    struct file_t *fileInfo = malloc(sizeof(struct file_t));
    util_GetFileInfo(shellContext->fileSelected, fileInfo, shellPrefs, shellContext);

    if (fileInfo->shellType != APP_TYPE && fileInfo->shellType != DIR_TYPE) {
        slot = ti_OpenVar(fileInfo->name, "r", fileInfo->type);
    }

    #ifdef FR
    ui_DrawStatusBar(shellPrefs, shellContext, "Infos fichier");
    #else
    ui_DrawStatusBar(shellPrefs, shellContext, "File Info");
    #endif

    gfx_BlitBuffer();
    gfx_SetColor(shellPrefs->fgColor);

    if (shellPrefs->transitionSpeed) {
        for (int8_t frame = 3; frame < 16 / shellPrefs->transitionSpeed; frame++) {
            shapes_RoundRectangleFill(15, 50, 231 - frame * 12 * shellPrefs->transitionSpeed, 220, frame * 12 * shellPrefs->transitionSpeed);
            gfx_SwapDraw();
        }
    }

    uint8_t hexaEdit = ti_OpenVar("HEXAEDIT", "r", OS_TYPE_PROT_PRGM);
    ti_Close(hexaEdit);

    uint8_t option = 0 + 3 * (fileInfo->shellType == APP_TYPE);
    shapes_RoundRectangleFill(15, 50, 39, 220, 192);
    info_Redraw(shellPrefs, fileInfo, 0);
    ui_DrawUISprite(shellPrefs->fgColor, UI_DARROW, 152, 209);
    gfx_BlitBuffer();

    while (kb_AnyKey());

    bool keyPressed = false;
    clock_t clockOffset = clock(); // Keep track of an offset for timer stuff

    while (!kb_IsDown(kb_KeyClear) && !kb_IsDown(kb_KeyWindow) && !kb_IsDown(kb_KeyZoom) && !kb_IsDown(kb_KeyTrace) && !kb_IsDown(kb_KeyAlpha)) {
        kb_Scan();
        util_UpdateKeyTimer(shellPrefs, shellContext, &clockOffset, &keyPressed);

        if ((kb_Data[7] || kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter)) && (!keyPressed || clock() - clockOffset > CLOCKS_PER_SEC / 32)) {
            if ((kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter)) && fileInfo->shellType != DIR_TYPE) {
                switch (option) {
                    case 0:
                        if (fileInfo->archived && asm_utils_checkEnoughRAM(fileInfo->size)) {
                            ti_SetArchiveStatus(false, slot);
                        } else {
                            util_SafeArchive(slot, fileInfo->name, fileInfo->type);
                        }

                        fileInfo->archived = !fileInfo->archived;

                        break;
                    case 1:
                        if (fileInfo->shellType == BASIC_TYPE || fileInfo->shellType == ICE_SRC_TYPE) {
                            asm_fileOps_lockPrgm(shellContext->programPtrs[shellContext->fileSelected - shellPrefs->showAppsFolder - shellPrefs->showAppVarsFolder]);
                            fileInfo->locked = !fileInfo->locked;
                        }

                        break;
                    case 2:
                        if (fileInfo->type == OS_TYPE_PRGM || fileInfo->type == OS_TYPE_PROT_PRGM) {
                            asm_fileOps_hidePrgm(fileInfo->name, fileInfo->type);
                            fileInfo->hidden = !fileInfo->hidden;
                            *fileInfo->name ^= 64;

                            if (fileInfo->archived) {
                                util_SafeArchive(slot, fileInfo->name, fileInfo->type);
                            }
                        }

                        break;
                    case 3:
                        if (menu_DeleteFile(shellPrefs, shellContext, fileInfo)) {
                            free(fileInfo->name);
                            free(fileInfo->icon);
                            free(fileInfo->description);
                            free(fileInfo);

                            ti_Close(slot);
                            *fileCount -= 1;
                            return;
                        }

                        ui_DrawUISprite(shellPrefs->fgColor, UI_DARROW, 152, 209);
                        while (kb_AnyKey());
                        break;
                    case 4:
                        if (menu_RenameFile(shellPrefs, shellContext, fileInfo)) {
                            free(fileInfo->name);
                            free(fileInfo->icon);
                            free(fileInfo->description);
                            free(fileInfo);

                            ti_Close(slot);
                            return;
                        }

                        ui_DrawUISprite(shellPrefs->fgColor, UI_DARROW, 152, 209);
                        break;
                    case 5:
                        if (hexaEdit || fileInfo->shellType == BASIC_TYPE || fileInfo->shellType == ICE_SRC_TYPE || fileInfo->shellType == CELTIC_TYPE) {
                            ti_Close(slot);
                            util_WritePrefs(shellPrefs, shellContext, true);
                            while (kb_AnyKey());

                            if (hexaEdit) {
                                if (fileInfo->shellType == BASIC_TYPE || fileInfo->shellType == ICE_SRC_TYPE || fileInfo->shellType == CELTIC_TYPE) {
                                    gfx_SetColor(shellPrefs->bgColor);
                                    shapes_RoundRectangleFill(9, 56, 205, 208, 20);
                                    hexaEdit = menu_YesNo(shellPrefs, shellContext, 92, 67, "TI-OS", "HexaEdit");

                                    if (kb_IsDown(kb_KeyClear) || kb_IsDown(kb_KeyMode)) {
                                        gfx_SetColor(shellPrefs->fgColor);
                                        gfx_FillRectangle_NoClip(56, 205, 208, 20);
                                        ui_DrawUISprite(shellPrefs->fgColor, UI_DARROW, 152, 209);
                                        while (kb_AnyKey());
                                        break;
                                    }
                                }

                                if (hexaEdit) {
                                    asm_utils_initHexaEditStart(fileInfo->name, strlen(fileInfo->name), fileInfo->type);
                                    gfx_End();
                                    asm_runProgram_run("HEXAEDIT", OS_TYPE_PROT_PRGM, C_TYPE, shellPrefs);
                                }
                            }

                            while (kb_AnyKey());
                            gfx_End();
                            asm_editProgram_edit(fileInfo->name, fileInfo->shellType == CELTIC_TYPE, shellPrefs);
                        }

                        break;
                    default:
                        break;
                }
            }

            if (fileInfo->shellType != APP_TYPE) {
                if (kb_IsDown(kb_KeyUp) || kb_IsDown(kb_KeyDown)) {
                    if (option < 3) { // Loop around other side
                        option += 3;
                    } else {
                        option -= 3;
                    }
                }

                if (kb_IsDown(kb_KeyLeft)) {
                    if (option) {
                        option--;
                    } else {
                        option = 5;
                    }
                } else if (kb_IsDown(kb_KeyRight)) {
                    if (option < 5) {
                        option++;
                    } else {
                        option = 0;
                    }
                }
            }

            gfx_SetDrawBuffer();
            info_Redraw(shellPrefs, fileInfo, option);
            gfx_BlitBuffer();

            util_WaitBeforeKeypress(&clockOffset, &keyPressed);
        }
    }

    free(fileInfo->name);
    free(fileInfo->icon);
    free(fileInfo->description);
    free(fileInfo);

    ti_Close(slot);
}
