/**
 * --------------------------------------
 * 
 * CEaShell Source Code - info.c
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2026
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

#include "asm/apps.h"
#include "asm/fileOps.h"
#include "asm/utils.h"

#include <fileioc.h>
#include <string.h>

#include <ti/ui.h>

static void info_Redraw(struct preferences_t *shellPrefs, struct file_t *fileInfo, uint8_t option, bool inSearch) {
    gfx_SetColor(shellPrefs->fgColor);
    gfx_FillRectangle_NoClip(56, 34, 208, 158);
    gfx_SetColor(shellPrefs->bgColor);
    shapes_RoundRectangleFill(12, 56, 66, 138, 56);
    shapes_RoundRectangleFill(12, 56, 104, 208, 100);
    gfx_FillRectangle_NoClip(194, 92, 12, 12);
    gfx_SetColor(shellPrefs->fgColor);
    gfx_FillCircle_NoClip(209, 88, 15);
    gfx_SetColor(shellPrefs->hlColor);
    shapes_RoundRectangleFill(12, 56, 34, 138, 26);

    if (fileInfo->icon != NULL) {
        gfx_ScaledSprite_NoClip(fileInfo->icon, 200, 34, 4, 4);
    } else {
        gfx_FillRectangle_NoClip(200, 34, 64, 64);

        if (fileInfo->shellType == DIR_TYPE) {
            shapes_Folder(4, 212, 50);
        } else {
            shapes_FileIcon(4, fileInfo->shellType, 216, 46);
        }
    }

    tempSprite->height = 12;
    tempSprite->width = 12;
    shapes_GetRoundCorners(tempSprite, shellPrefs->fgColor, 12, 200, 34);
    shapes_DrawRoundCorners(tempSprite, 64, 64, 200, 34);

    *fileInfo->name += 64 * (*fileInfo->name < 'A');
    ui_CenterStringBig(fileInfo->name, 125, 40);

    if (fileInfo->hidden) {
        *fileInfo->name ^= 64;
    }


    #ifdef FR
    static const char *fileTypeStrings[11] = {&rodata_fileTypeASM, &rodata_fileTypeC, &rodata_fileTypeBASIC, &rodata_fileTypeICE, &rodata_fileTypeICESrc, "Annuaire", "AppVar", NULL, "Var Celtic", "Appli", "Inconnu"};
    gfx_PrintStringXY("Type : ", 64, 74);
    #else
    static const char *fileTypeStrings[11] = {&rodata_fileTypeASM, &rodata_fileTypeC, &rodata_fileTypeBASIC, &rodata_fileTypeICE, &rodata_fileTypeICESrc, "Directory", "AppVar", NULL, "Celtic Var", "App", "Unknown"};
    gfx_PrintStringXY("Type: ", 64, 74);
    #endif

    gfx_PrintString(fileTypeStrings[fileInfo->shellType]);
    gfx_SetTextXY(64, 87);

    #ifdef FR
    if (fileInfo->shellType != DIR_TYPE) {
        gfx_PrintString("Taille : ");
    } else if (!inSearch) {
        gfx_PrintString("Items : ");
    }
    #else
    if (fileInfo->shellType != DIR_TYPE) {
        gfx_PrintString("Size: ");
    } else if (!inSearch) {
        gfx_PrintString("Items: ");
    }
    #endif

    gfx_PrintUInt(fileInfo->size, 5 + (fileInfo->shellType == APP_TYPE) - (fileInfo->shellType == DIR_TYPE));

    #ifdef FR
    gfx_PrintStringXY("Description :", 64, 100);
    #else
    gfx_PrintStringXY("Description:", 64, 100);
    #endif

    if (fileInfo->description[0] != '\0') {
        ui_PrintStringWrap(fileInfo->description, 64, 112, 28, 2);
    } else {
        #ifdef FR
        gfx_PrintStringXY("Aucune description.", 64, 112);
        #else
        gfx_PrintStringXY("No description.", 64, 112);
        #endif
    }

    gfx_SetColor(shellPrefs->hlColor);

    if (fileInfo->shellType != DIR_TYPE) {
        if (option < 3) {
            #ifdef FR
            shapes_PixelIndentRectangle(60 + 65 * (option > 0) + 83 * (option > 1), 149, 9, 9);
            #else
            shapes_PixelIndentRectangle(63 + 76 * (option > 0) + 63 * (option > 1), 149, 9, 9);
            #endif
        } else if (option < 6) {
            #ifdef FR
            shapes_PixelIndentRectangle(64 + 64 * (option - 3), 178, 64, 9);
            #else
            shapes_PixelIndentRectangle(68 + 65 * (option - 3), 178, 54, 9);
            #endif
        } else if (fileInfo->type != OS_TYPE_APPVAR) {
            #ifdef FR
            shapes_PixelIndentRectangle(182, 190, 39, 9);
            #else
            shapes_PixelIndentRectangle(181, 190, 39, 9);
            #endif
        }

        if (fileInfo->shellType != APP_TYPE) {
            #ifdef FR
            gfx_PrintStringXY("Propri""\x15""t""\x15""s :", 64, 137);
            gfx_PrintStringXY("Archiv""\x15""    Verrouill""\x15""    Cach""\x15", 71, 150);
            gfx_PrintStringXY("Suppr.       Renom.    Modifier", 75, 179);

            ui_CheckBox(fileInfo->archived, 61, 150);
            ui_CheckBox(fileInfo->locked, 126, 150);
            ui_CheckBox(fileInfo->hidden, 209, 150);
            #else
            gfx_PrintStringXY("Attributes:", 64, 137);
            gfx_PrintStringXY("Archived     Locked     Hidden", 74, 150);
            gfx_PrintStringXY("Delete      Rename         Edit", 74, 179);

            ui_CheckBox(fileInfo->archived, 64, 150);
            ui_CheckBox(fileInfo->locked, 140, 150);
            ui_CheckBox(fileInfo->hidden, 203, 150);
            #endif
        } else {
            #ifdef FR
            gfx_PrintStringXY("Version d'OS minimum :", 64, 137);
            gfx_PrintStringXY("Suppr.", 74, 179);
            #else
            gfx_PrintStringXY("Minimum OS Version:", 64, 137);
            gfx_PrintStringXY("Delete", 74, 179);
            #endif

            gfx_PrintStringXY(fileInfo->minimumOSVersion, 64, 150);
        }

        static const uint8_t leftBracket[8] = {0xF0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xF0, 0x00};
        static const uint8_t thetaChar[8] = {0x7C, 0xC6, 0xC6, 0xFE, 0xC6, 0xC6, 0x7C, 0x00};
        util_SetGFXChar('[', leftBracket, 5);

        if (fileInfo->type != OS_TYPE_APPVAR) {
            #ifdef FR
            gfx_PrintStringXY("[on] touche:", 102, 191);
            menu_DrawValueString(222, 191, MENU_TYPE_HKEY, fileInfo->shortcut);
            #else
            gfx_PrintStringXY("[on] Hotkey:", 101, 191);
            menu_DrawValueString(221, 191, MENU_TYPE_HKEY, fileInfo->shortcut);
            #endif
        }

        util_SetGFXChar('[', thetaChar, 8);

        #ifdef FR
        gfx_PrintStringXY("Op""\x15""rations du fichier :", 64, 165);
        #else
        gfx_PrintStringXY("File Operations:", 64, 165);
        #endif
    }
}

void info_Open(struct preferences_t *shellPrefs, struct context_t *shellContext, unsigned int *fileCount) {
    util_WritePrefs(shellPrefs, shellContext, true);
    uint8_t slot;
    static struct file_t fileInfo;
    util_GetFileInfo(shellContext->fileSelected, &fileInfo, shellPrefs, shellContext);

    if (fileInfo.shellType != APP_TYPE && fileInfo.shellType != DIR_TYPE) {
        slot = ti_OpenVar(fileInfo.name, "r", fileInfo.type);
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

    uint8_t option = 0 + 3 * (fileInfo.shellType == APP_TYPE);
    shapes_RoundRectangleFill(15, 50, 28, 220, 203);
    info_Redraw(shellPrefs, &fileInfo, option, shellContext->searchString[0] != '\0');
    ui_DrawUISprite(shellPrefs->fgColor, UI_DARROW, 152, 209);
    gfx_BlitBuffer();

    while (kb_AnyKey());

    bool keyPressed = false;
    clock_t clockOffset = clock(); // Keep track of an offset for timer stuff

    while (!kb_IsDown(kb_KeyClear) && !kb_IsDown(kb_KeyWindow) && !kb_IsDown(kb_KeyZoom) && !kb_IsDown(kb_KeyTrace) && !kb_IsDown(kb_KeyAlpha)) {
        kb_Scan();
        util_UpdateKeyTimer(shellPrefs, shellContext, &clockOffset, &keyPressed);

        if ((kb_Data[7] || kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter) || kb_IsDown(kb_KeyVars)) && (!keyPressed || clock() - clockOffset > CLOCKS_PER_SEC / 32)) {
            if (!keyPressed) {
                clockOffset = clock();
            }

            if (kb_IsDown(kb_KeyVars) && (fileInfo.shellType == BASIC_TYPE || fileInfo.shellType == ICE_SRC_TYPE || fileInfo.shellType == CELTIC_TYPE)) {
                util_SearchToMain(shellPrefs, shellContext);
                asm_editProgram_edit(fileInfo.name, fileInfo.shellType == CELTIC_TYPE, shellPrefs);
            }

            if ((kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter)) && fileInfo.shellType != DIR_TYPE) {
                switch (option) {
                    case 0:
                        if (fileInfo.archived && asm_utils_checkEnoughRAM(fileInfo.size)) {
                            ti_SetArchiveStatus(false, slot);
                        } else {
                            util_SafeArchive(slot);
                        }

                        fileInfo.archived = !fileInfo.archived;
                        break;
                    case 1:
                        if (fileInfo.shellType == BASIC_TYPE || fileInfo.shellType == ICE_SRC_TYPE) {
                            asm_fileOps_lockPrgm(fileInfo.name, fileInfo.type);

                            fileInfo.locked = !fileInfo.locked;

                            if (fileInfo.archived) {
                                util_SafeArchive(slot);
                            }
                        }

                        break;
                    case 2:
                        if (fileInfo.type == OS_TYPE_PRGM || fileInfo.type == OS_TYPE_PROT_PRGM) {
                            asm_fileOps_hidePrgm(fileInfo.name, fileInfo.type);
                            fileInfo.hidden = !fileInfo.hidden;
                            *fileInfo.name ^= 64;

                            if (fileInfo.archived) {
                                util_SafeArchive(slot);
                            }
                        }

                        break;
                    case 3:
                        if (menu_DeleteFile(shellPrefs, shellContext, &fileInfo)) {
                            ti_Close(slot);
                            *fileCount -= 1;
                            return;
                        }

                        ui_DrawUISprite(shellPrefs->fgColor, UI_DARROW, 152, 209);
                        break;
                    case 4:
                        if (menu_RenameFile(shellPrefs, shellContext, &fileInfo)) {
                            ti_Close(slot);
                            return;
                        }

                        ui_DrawUISprite(shellPrefs->fgColor, UI_DARROW, 152, 209);
                        break;
                    case 5:
                        if (hexaEdit || fileInfo.shellType == BASIC_TYPE || fileInfo.shellType == ICE_SRC_TYPE || fileInfo.shellType == CELTIC_TYPE) {
                            ti_Close(slot);
                            while (kb_AnyKey());

                            if (hexaEdit) {
                                if (fileInfo.shellType == BASIC_TYPE || fileInfo.shellType == ICE_SRC_TYPE || fileInfo.shellType == CELTIC_TYPE) {
                                    gfx_SetColor(shellPrefs->bgColor);
                                    shapes_RoundRectangleFill(9, 56, 205, 208, 20);
                                    hexaEdit = menu_YesNo(shellPrefs, shellContext, 92, 67, "TI-OS", "HexaEdit");

                                    if (kb_IsDown(kb_KeyClear) || kb_IsDown(kb_KeyMode)) {
                                        gfx_SetColor(shellPrefs->fgColor);
                                        gfx_FillRectangle_NoClip(56, 205, 208, 20);
                                        ui_DrawUISprite(shellPrefs->fgColor, UI_DARROW, 152, 209);
                                        hexaEdit = true;
                                        while (kb_AnyKey());
                                        break;
                                    }
                                }

                                if (hexaEdit) {
                                    asm_utils_initHexaEditStart(fileInfo.name, strlen(fileInfo.name), fileInfo.type);
                                    util_SearchToMain(shellPrefs, shellContext);
                                    asm_runProgram_run("HEXAEDIT", OS_TYPE_PROT_PRGM, C_TYPE, shellPrefs);
                                }
                            }

                            util_SearchToMain(shellPrefs, shellContext);
                            asm_editProgram_edit(fileInfo.name, fileInfo.shellType == CELTIC_TYPE, shellPrefs);
                        }

                        break;
                    case 6: {
                        uint8_t value = fileInfo.shortcut;

                        while (kb_AnyKey());

                        while (!kb_IsDown(kb_KeyClear) && !kb_IsDown(kb_Key2nd) && !kb_IsDown(kb_KeyEnter)) {
                            kb_Scan();
                            util_UpdateKeyTimer(shellPrefs, shellContext, &clockOffset, &keyPressed);

                            if (kb_Data[7] && (!keyPressed || clock() - clockOffset > CLOCKS_PER_SEC / 16)) {
                                if (!keyPressed) {
                                    clockOffset = clock();
                                }

                                if (kb_IsDown(kb_KeyLeft)) {
                                    if (value) {
                                        value--;
                                    } else {
                                        value = 10;
                                    }
                                } else if (kb_IsDown(kb_KeyRight)) {
                                    if (value < 10) {
                                        value++;
                                    } else {
                                        value = 0;
                                    }
                                }

                                gfx_SetColor(shellPrefs->hlColor);

                                #ifdef FR
                                gfx_FillRectangle_NoClip(184, 191, 35, 7);
                                menu_DrawValueString(222, 191, MENU_TYPE_HKEY, value);
                                #else
                                gfx_FillRectangle_NoClip(183, 191, 35, 7);
                                menu_DrawValueString(221, 191, MENU_TYPE_HKEY, value);
                                #endif

                                gfx_BlitBuffer();

                                util_WaitBeforeKeypress(&clockOffset, &keyPressed);
                            }
                        }

                        if (!kb_IsDown(kb_KeyClear)) {
                            if (fileInfo.shortcut != 10) {
                                shellPrefs->launchFiles[fileInfo.shortcut].type = 0;
                            }

                            if (value != 10) {
                                shellPrefs->launchFiles[value].type = fileInfo.type;
                                memcpy(shellPrefs->launchFiles[value].name, fileInfo.name, 9);
                            }

                            fileInfo.shortcut = value;
                        }

                        util_WaitBeforeKeypress(&clockOffset, &keyPressed);
                    }
                    default:
                        break;
                }
            }

            if (fileInfo.shellType != APP_TYPE) {
                if (kb_IsDown(kb_KeyUp)) {
                    if (option < 3) {
                        option += 3 + (option == 2);
                    } else if (option != 6) {
                        option -= 3;
                    } else {
                        option--;
                    }
                } else if (kb_IsDown(kb_KeyDown)) {
                    if (option == 6) {
                        option = 2;
                    } else if (option > 2) {
                        option = 6;
                    } else {
                        option += 3;
                    }
                }

                if (kb_IsDown(kb_KeyLeft)) {
                    if (option) {
                        option--;
                    } else {
                        option = 5 + (fileInfo.type != OS_TYPE_APPVAR);
                    }
                } else if (kb_IsDown(kb_KeyRight)) {
                    if (option < 5 + (fileInfo.type != OS_TYPE_APPVAR)) {
                        option++;
                    } else {
                        option = 0;
                    }
                }
            } else {
                if (kb_Data[7]) {
                    if (option == 3) {
                        option = 6;
                    } else {
                        option = 3;
                    }
                }
            }

            gfx_SetDrawBuffer();
            info_Redraw(shellPrefs, &fileInfo, option, shellContext->searchString[0] != '\0');
            gfx_BlitBuffer();

            util_WaitBeforeKeypress(&clockOffset, &keyPressed);
        }
    }

    ti_Close(slot);
    util_WaitBeforeKeypress(&clockOffset, &keyPressed);
}
