/**
 * --------------------------------------
 * 
 * CEaShell Source Code - main.c
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2024
 * License: GPL-3.0
 * Last Build: January 7, 2024
 * Version: 2.0.0-dev
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

#include <graphx.h>
#include <keypadc.h>

#include <sys/rtc.h>
#include <ti/screen.h>

int main(void) {
    srand(rtc_Time());

    // Remove TI-BASIC stop hook just for safety reasons to ensure it gets removed.
    asm_hooks_removeStopHook();

    // Enable [on] key detection
    kb_EnableOnLatch();
    kb_ClearOnLatch();

    // Initialize shell preferences / context and read from AppVar.
    struct preferences_t *shellPrefs = malloc(sizeof(struct preferences_t)); // Shell preferences, which are backed up in the CEaShell AppVar.
    struct context_t *shellContext = malloc(sizeof(struct context_t)); // Shell context information, which is modified during shell usage.
    util_ReadPrefs(shellPrefs, shellContext);

    // Graphics setup
    asm_spi_setupSPI();
    gfx_Begin();

    #ifdef FR
    const uint8_t circumflexO[8] = {0x7C, 0x82, 0x7C, 0xC6, 0xC6, 0xC6, 0x7C, 0x00};    // @ = ô
    const uint8_t circumflexU[8] = {0x7C, 0x82, 0xC6, 0xC6, 0xC6, 0xC6, 0x7E, 0x00};    // ^ = û
    const uint8_t cedilla[8] = {0x00, 0x00, 0x7C, 0xC6, 0xC0, 0xC6, 0x7C, 0x30};        // ` = ç
    const uint8_t rightEAccent[8] = {0x1C, 0x00, 0x7C, 0xC6, 0xFE, 0xC0, 0x7C};         // } = é
    const uint8_t circumflexE[8] = {0x7C, 0x82, 0x7C, 0xC6, 0xFE, 0xC0, 0x7C};          // | = ê
    const uint8_t leftEAccent[8] = {0x70, 0x00, 0x7C, 0xC6, 0xFE, 0xC0, 0x7C};          // { = è
    const uint8_t leftAAccent[8] = {0x70, 0x00, 0x7C, 0x06, 0x7E, 0xC6, 0x7E, 0x00};    // ~ = à

    util_SetGFXChar('@', circumflexO, 8);
    util_SetGFXChar('^', circumflexU, 8);
    util_SetGFXChar('`', cedilla, 8);
    util_SetGFXChar('}', rightEAccent, 8);
    util_SetGFXChar('|', circumflexE, 8);
    util_SetGFXChar('{', leftEAccent, 8);
    util_SetGFXChar('~', leftAAccent, 8);
    #endif

    gfx_SetTransparentColor(240);
    gfx_SetTextBGColor(240);
    gfx_SetTextFGColor(shellPrefs->textColor);
    gfx_SetTextTransparentColor(240);
    gfx_SetDrawBuffer();

    if (shellPrefs->invertColors) {
        asm_utils_invertPalette();
    }

    // Get number of programs and appvars
    util_FilesInit(shellPrefs, shellContext);

    // Debounce keypresses when entering the shell.
    while (kb_AnyKey());

    files_Main(shellPrefs, shellContext);

    // Save preferences to CEaShell's AppVar
    shellContext->directory = PROGRAMS_FOLDER;
    shellContext->fileSelected = 0;
    shellContext->fileStartLoc = 0;
    util_WritePrefs(shellPrefs, shellContext, false);
    gfx_End();

    // Clean up for returning to the OS
    asm_apps_exitDefrag();
    kb_DisableOnLatch();

    return 0;
}
