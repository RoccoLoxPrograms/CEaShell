/**
 * --------------------------------------
 * 
 * CEaShell Source Code - main.c
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2024
 * License: GPL-3.0
 * Last Build: August 10, 2024
 * Version: 2.0.1
 * 
 * --------------------------------------
**/

#include "defines.h"

#include "utility.h"
#include "files.h"

#include "asm/apps.h"
#include "asm/fileSystem.h"
#include "asm/hooks.h"
#include "asm/spi.h"
#include "asm/utils.h"

#include <keypadc.h>

#include <sys/rtc.h>
#include <ti/screen.h>

gfx_UninitedSprite(fileIcon, 16, 16);
gfx_UninitedSprite(tempSprite, 12, 12);

int main(void) {
    srand(rtc_Time());
    asm_utils_restoreAns();

    // Remove TI-BASIC stop hook just for safety reasons to ensure it gets removed.
    asm_hooks_removeStopHook();

    // Enable [on] key detection
    kb_EnableOnLatch();
    kb_ClearOnLatch();

    // Graphics setup
    asm_spi_setupSPI();
    gfx_Begin();

    // Initialize shell preferences / context and read from AppVar.
    static struct preferences_t shellPrefs; // Shell preferences, which are backed up in the CEaShell AppVar.
    static struct context_t shellContext; // Shell context information, which is modified during shell usage.
    shellContext.searchString[0] = '\0';
    util_ReadPrefs(&shellPrefs, &shellContext);

    #ifdef FR
    const uint8_t circumflexO[8] = {0x7C, 0x82, 0x7C, 0xC6, 0xC6, 0xC6, 0x7C, 0x00};        // \x0B = ô
    const uint8_t circumflexU[8] = {0x7C, 0x82, 0xC6, 0xC6, 0xC6, 0xC6, 0x7E, 0x00};        // \x0C = û
    const uint8_t cedilla[8] = {0x00, 0x00, 0x7C, 0xC6, 0xC0, 0xC6, 0x7C, 0x30};            // \x13 = ç
    const uint8_t capRightEAccent[8] = {0x1E, 0x00, 0xFE, 0xC0, 0xF8, 0xC0, 0xFE, 0x00};    // \x14 = É
    const uint8_t rightEAccent[8] = {0x1C, 0x00, 0x7C, 0xC6, 0xFE, 0xC0, 0x7C};             // \x15 = é
    const uint8_t circumflexE[8] = {0x7C, 0x82, 0x7C, 0xC6, 0xFE, 0xC0, 0x7C};              // \x16 = ê
    const uint8_t leftEAccent[8] = {0x70, 0x00, 0x7C, 0xC6, 0xFE, 0xC0, 0x7C};              // \x1C = è
    const uint8_t leftAAccent[8] = {0x70, 0x00, 0x7C, 0x06, 0x7E, 0xC6, 0x7E, 0x00};        // \x7F = à

    util_SetGFXChar(0x0B, circumflexO, 8);
    util_SetGFXChar(0x0C, circumflexU, 8);
    util_SetGFXChar(0x13, cedilla, 8);
    util_SetGFXChar(0x14, capRightEAccent, 8);
    util_SetGFXChar(0x15, rightEAccent, 8);
    util_SetGFXChar(0x16, circumflexE, 8);
    util_SetGFXChar(0x1C, leftEAccent, 8);
    util_SetGFXChar(0x7F, leftAAccent, 8);
    #endif

    util_CorrectTransparentColor(&shellPrefs);
    gfx_SetTransparentColor(240);
    gfx_SetTextFGColor(shellPrefs.textColor);
    gfx_SetDrawBuffer();

    if (shellPrefs.invertColors) {
        asm_utils_invertPalette();
    }

    // Get number of programs and appvars
    asm_fileSystem_sortVAT();
    util_FilesInit(&shellPrefs, &shellContext);

    files_Main(&shellPrefs, &shellContext);

    // Save preferences to CEaShell's AppVar
    shellContext.directory = PROGRAMS_FOLDER;
    shellContext.fileSelected = 0;
    shellContext.fileStartLoc = 0;
    util_WritePrefs(&shellPrefs, &shellContext, false);
    gfx_End();

    // Clean up for returning to the OS
    asm_apps_exitDefrag();
    kb_DisableOnLatch();

    return 0;
}
