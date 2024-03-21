/**
 * --------------------------------------
 * 
 * CEaShell Source Code - custom.c
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2024
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#include "defines.h"

#include "custom.h"
#include "menu.h"
#include "shapes.h"
#include "ui.h"
#include "utility.h"

#include "asm/utils.h"

#include <keypadc.h>
#include <string.h>

static void custom_ThemePreview(uint8_t *theme) {
    shapes_PixelIndentRectangle(163, 98, 143, 106);

    gfx_SetColor(theme[0]);
    shapes_PixelIndentRectangle(169, 104, 131, 94);
    gfx_SetColor(theme[1]);
    shapes_PixelIndentRectangle(172, 107, 61, 72);
    gfx_SetColor(theme[2]);
    shapes_PixelIndentRectangle(236, 107, 61, 36);

    #ifdef FR
    gfx_SetTextFGColor(theme[3]);
    gfx_PrintStringXY("Normal", 180, 185);
    gfx_SetTextFGColor(theme[4]);
    gfx_PrintStringXY("Cach""\x15", 244, 185);
    #else
    gfx_SetTextFGColor(theme[3]);
    gfx_PrintStringXY("Normal", 180, 185);
    gfx_SetTextFGColor(theme[4]);
    gfx_PrintStringXY("Hidden", 244, 185);
    #endif
}

static void custom_CreateTheme(struct preferences_t *shellPrefs, struct context_t *shellContext) {
    // Try to minimize relocations this way
    #ifdef FR
    static const char *modifyingStrings = "contexte\0\0\0\0\0\0\0\0\0premier plan\0\0\0\0\0surlignage\0\0\0\0\0\0\0texte par d""\x15""faut\0texte cach""\x15""\0";
    #else
    static const char *modifyingStrings = "Background\0\0\0Foreground\0\0\0Highlight\0\0\0\0Default text\0Hidden text\0";
    #endif

    static uint8_t newTheme[5];
    memcpy(newTheme, &shellPrefs->bgColor, 5);
    uint8_t selected = shellPrefs->bgColor;
    uint8_t modifying = 0;
    bool justOpened = true; // Kind of a weird way to do this but minimizes function calls

    while (kb_AnyKey());

    bool keyPressed = false;
    clock_t clockOffset = clock(); // Keep track of an offset for timer stuff

    while (!kb_IsDown(kb_KeyClear) && !kb_IsDown(kb_Key2nd) && !kb_IsDown(kb_KeyEnter)) {
        kb_Scan();
        util_UpdateKeyTimer(shellPrefs, shellContext, &clockOffset, &keyPressed);

        if ((kb_Data[7] || kb_IsDown(kb_KeyMode) || justOpened) && (!keyPressed || clock() - clockOffset > CLOCKS_PER_SEC / 32)) {
            justOpened = false;

            if (kb_IsDown(kb_KeyMode)) {
                if (modifying != 4) {
                    modifying++;
                } else {
                    modifying = 0;
                }

                selected = newTheme[modifying];
            }

            if (kb_IsDown(kb_KeyUp)) {
                if (selected > 31) {
                    selected -= 32;
                } else {
                    selected += 224;
                }
            } else if (kb_IsDown(kb_KeyDown)) {
                if (selected < 224) {
                    selected += 32;
                } else {
                    selected -= 224;
                }
            }

            if (kb_IsDown(kb_KeyLeft)) {
                selected--;
            } else if (kb_IsDown(kb_KeyRight)) {
                selected++;
            }

            newTheme[modifying] = selected;

            gfx_SetColor(shellPrefs->fgColor);
            gfx_FillRectangle_NoClip(163, 34, 143, 170);
            gfx_SetColor(shellPrefs->bgColor);
            shapes_PixelIndentRectangle(163, 34, 143, 58);

            for (uint8_t x = 0; x < 32; x++) {
                for (uint8_t y = 0; y < 8; y++) {
                    gfx_SetColor(y * 32 + x);
                    #ifdef FR
                    gfx_FillRectangle_NoClip(170 + 4 * x, 38 + 4 * y, 4, 4);
                    #else
                    gfx_FillRectangle_NoClip(170 + 4 * x, 41 + 4 * y, 4, 4);
                    #endif

                    if (y * 32 + x == selected) {
                        gfx_SetColor(!(selected > 131 && selected % 8 > 3) * 255);
                        #ifdef FR
                        gfx_Rectangle_NoClip(170 + 4 * x, 38 + 4 * y, 4, 4);
                        #else
                        gfx_Rectangle_NoClip(170 + 4 * x, 41 + 4 * y, 4, 4);
                        #endif
                    }
                }
            }

            gfx_SetColor(shellPrefs->bgColor);
            #ifdef FR
            gfx_SetPixel(170, 38);
            gfx_SetPixel(297, 38);
            gfx_SetPixel(170, 69);
            gfx_SetPixel(297, 69);
            #else
            gfx_SetPixel(170, 41);
            gfx_SetPixel(297, 41);
            gfx_SetPixel(170, 72);
            gfx_SetPixel(297, 72);
            #endif

            custom_ThemePreview(newTheme);

            gfx_SetTextFGColor(shellPrefs->textColor);

            #ifdef FR
            gfx_PrintStringXY("Couleur de ", 171, 72);
            gfx_PrintStringXY(&modifyingStrings[modifying * 17], 171, 80);
            #else
            gfx_PrintStringXY(&modifyingStrings[modifying * 13], 171, 79);
            gfx_PrintString(" color");
            #endif

            gfx_BlitBuffer();

            util_WaitBeforeKeypress(&clockOffset, &keyPressed);
        }
    }

    if (!kb_IsDown(kb_KeyClear)) {
        memcpy(&shellPrefs->bgColor, newTheme, sizeof(uint8_t) * 5);
    }

    while (kb_AnyKey());
}

static void custom_PresetTheme(struct preferences_t *shellPrefs, struct context_t *shellContext) {
    static uint8_t presetThemes[THEME_COUNT * 5] = {
        246, 237, 236, 0, 181,
        148, 74, 0, 255, 222,
        137, 128, 96, 255, 181,
        228, 226, 162, 255, 222,
        100, 3, 2, 255, 181,
        125, 28, 58, 255, 222,
        243, 210, 208, 255, 222,
        114, 81, 48, 255, 181,
        255, 222, 181, 0, 181
    };

    for (uint8_t i = 0; i < 5; i++) {
        presetThemes[(THEME_COUNT - 1) * 5 + i] = 255 - *(&shellPrefs->bgColor + i);
    }

    static uint8_t newTheme[5];
    memcpy(newTheme, &shellPrefs->bgColor, 5);
    uint8_t selected = 0;
    bool justOpened = true; // Kind of a weird way to do this but minimizes function calls

    while (kb_AnyKey());

    bool keyPressed = false;
    clock_t clockOffset = clock(); // Keep track of an offset for timer stuff

    while (!kb_IsDown(kb_KeyClear) && !kb_IsDown(kb_Key2nd) && !kb_IsDown(kb_KeyEnter)) {
        kb_Scan();
        util_UpdateKeyTimer(shellPrefs, shellContext, &clockOffset, &keyPressed);

        if ((kb_Data[7] || justOpened) && (!keyPressed || clock() - clockOffset > CLOCKS_PER_SEC / 32)) {
            justOpened = false;

            if (kb_IsDown(kb_KeyUp) || kb_IsDown(kb_KeyDown)) {
                if (selected > 4) {
                    selected -= 5;
                } else {
                    selected += 5;
                }
            }

            if (kb_IsDown(kb_KeyLeft)) {
                if (!selected) {
                    selected = 9;
                } else {
                    selected--;
                }
            } else if (kb_IsDown(kb_KeyRight)) {
                if (selected == 9) {
                    selected = 0;
                } else {
                    selected++;
                }
            }

            gfx_SetColor(shellPrefs->fgColor);
            gfx_FillRectangle_NoClip(163, 34, 143, 170);
            gfx_SetColor(shellPrefs->bgColor);
            shapes_PixelIndentRectangle(163, 34, 143, 58);

            for (uint8_t x = 0; x < 5; x++) {
                for (uint8_t y = 0; y < 2; y++) {
                    if (x + y * 5 == selected) {
                        gfx_SetColor(shellPrefs->hlColor);
                        shapes_RoundRectangleFill(3, 168 + x * 27, 38 + y * 25, 25, 25);
                    }

                    gfx_SetColor(presetThemes[(x + y * 5) * 5 + 1]);
                    shapes_PixelIndentRectangle(170 + x * 27, 40 + y * 25, 21, 21);
                }
            }

            gfx_SetColor(0);
            shapes_PixelIndentRectangle(278, 65, 21, 21);
            gfx_SetColor(255);
            gfx_FillTriangle_NoClip(278, 66, 297, 85, 278, 85);
            gfx_SetColor(gfx_GetPixel(279, 86));
            gfx_SetPixel(278, 85);

            memcpy(newTheme, &presetThemes[selected * 5], sizeof(uint8_t) * 5);

            gfx_SetColor(shellPrefs->bgColor);

            custom_ThemePreview(newTheme);

            gfx_SetTextFGColor(shellPrefs->textColor);
            gfx_BlitBuffer();

            util_WaitBeforeKeypress(&clockOffset, &keyPressed);
        }
    }

    if (!kb_IsDown(kb_KeyClear)) {
        if (selected != 9) {
            memcpy(&shellPrefs->bgColor, newTheme, sizeof(uint8_t) * 5);
        } else {
            shellPrefs->invertColors = !shellPrefs->invertColors;
            asm_utils_invertPalette();
        }
    }

    while (kb_AnyKey());
}

void custom_Open(struct preferences_t *shellPrefs, struct context_t *shellContext) {
    #ifdef FR
    ui_DrawStatusBar(shellPrefs, shellContext, "Customiser");
    #else
    ui_DrawStatusBar(shellPrefs, shellContext, "Customize");
    #endif

    gfx_BlitBuffer();
    gfx_SetColor(shellPrefs->fgColor);

    if (shellPrefs->transitionSpeed) {
        for (int8_t frame = 3; frame < 16 / shellPrefs->transitionSpeed; frame++) {
            shapes_RoundRectangleFill(6, 8, 232 - frame * 12 * shellPrefs->transitionSpeed, frame * (19 * shellPrefs->transitionSpeed), frame * (12 * shellPrefs->transitionSpeed));
            gfx_SwapDraw();
        }
    }

    shapes_RoundRectangleFill(6, 8, 28, 304, 204);
    ui_DrawUISprite(shellPrefs->fgColor, UI_LARROW, 14, 210);
    gfx_BlitBuffer();

    static struct menu_t menuContext;

    menuContext.totalOptions = 12;
    menuContext.optionSelected = 0;
    #ifdef FR
    menuContext.totalHeight = 343;
    #else
    menuContext.totalHeight = 235;
    #endif

    #ifdef FR
    menuContext.options[0] = "Transitions";
    menuContext.options[1] = "Vitesse de transition";
    menuContext.options[2] = "\x14""chelle d'ic""\x0B""nes";
    menuContext.options[3] = "Horloge";
    menuContext.options[4] = "Afficher CEaShell";
    menuContext.options[5] = "Afficher les dossiers d'applis";
    menuContext.options[6] = "Afficher le dossier d'AppVars";
    menuContext.options[7] = "Afficher les programmes cach""\x15""s";
    menuContext.options[8] = "Nombre de fichiers";
    menuContext.options[9] = "Minuterie APD";
    menuContext.options[10] = "Th""\x1C""mes personnalis""\x15""s";
    menuContext.options[11] = "Th""\x1C""mes pr""\x1C""d""\x1C""finis";

    menuContext.details[0] = "Activer/D""\x15""sactiver la transition d'animation lorsque les menus s'ouvrent/ferment.";
    menuContext.details[1] = "Choisissez la vitesse d'animation pour la transition d'animations.";
    menuContext.details[2] = "\x14""chelle d'ic""\x0B""nes dans la visionneuse principale.";
    menuContext.details[3] = "Choisissez si l'heure doit ""\x16""tre affich""\x15""e en mode 24 heures ou 12 heures.";
    menuContext.details[4] = "Afficher CEaShell dans le menu des fichiers.";
    menuContext.details[5] = "Afficher le dossier d'applications dans le menu principal de fichiers.";
    menuContext.details[6] = "Afficher le dossier d'AppVars dans le menu principal de fichiers.";
    menuContext.details[7] = "Montrer les programmes cach""\x15""s dans le menu principal de fichiers.";
    menuContext.details[8] = "Afficher le nombre de fichiers dans le r""\x15""pertoire actuel.";
    menuContext.details[9] = "Temps d'attente apr""\x1C""s inactivit""\x15"" dans CEaShell avant d'""\x15""teindre la calculatrice.";
    menuContext.details[10] = "Cr""\x15""ez vos propres th""\x1C""mes de couleurs ""\x7F"" utiliser sur CEaShell.";
    menuContext.details[11] = "S""\x15""lectionnez un th""\x1C""me de couleurs pr""\x1C""d""\x1C""fini ""\x7F"" utiliser par CEaShell.";
    #else
    menuContext.options[0] = "Transitions";
    menuContext.options[1] = "Transition Speed";
    menuContext.options[2] = "Icon Scale";
    menuContext.options[3] = "Clock";
    menuContext.options[4] = "Show CEaShell";
    menuContext.options[5] = "Show Apps Folder";
    menuContext.options[6] = "Show AppVars Folder";
    menuContext.options[7] = "Show Hidden Programs";
    menuContext.options[8] = "File Count";
    menuContext.options[9] = "APD Timer";
    menuContext.options[10] = "Custom Theme";
    menuContext.options[11] = "Preset Themes";

    menuContext.details[0] = "Turn on/off the transition animations when opening or closing menus.";
    menuContext.details[1] = "Speed for transition animaitons between menus.";
    menuContext.details[2] = "Scale of file icons in the main file viewer.";
    menuContext.details[3] = "Choose whether the clock uses 24-Hour or 12-Hour time.";
    menuContext.details[4] = "Show CEaShell in the main file viewer.";
    menuContext.details[5] = "Show the Apps folder in the main file viewer.";
    menuContext.details[6] = "Show the AppVars folder in the main file viewer.";
    menuContext.details[7] = "Show hidden programs in the main file viewer.";
    menuContext.details[8] = "Displays the number of files in the current directory on the status bar.";
    menuContext.details[9] = "Number of minutes to wait after inactivity in CEaShell before turning off the calculator.";
    menuContext.details[10] = "Create your own color theme for CEaShell to use.";
    menuContext.details[11] = "Select a preset color theme for CEaShell to use.";
    #endif

    menuContext.types[0] = MENU_TYPE_BOOL;
    menuContext.types[1] = MENU_TYPE_SPEED;
    menuContext.types[2] = MENU_TYPE_SCALE;
    menuContext.types[3] = MENU_TYPE_TIME;
    menuContext.types[4] = MENU_TYPE_BOOL;
    menuContext.types[5] = MENU_TYPE_BOOL;
    menuContext.types[6] = MENU_TYPE_BOOL;
    menuContext.types[7] = MENU_TYPE_BOOL;
    menuContext.types[8] = MENU_TYPE_BOOL;
    menuContext.types[9] = MENU_TYPE_APD;
    menuContext.types[10] = MENU_TYPE_MENU;
    menuContext.types[11] = MENU_TYPE_MENU;

    menuContext.values[0] = shellPrefs->transitionSpeed;
    menuContext.values[1] = shellPrefs->transitionSpeed;
    menuContext.values[2] = shellPrefs->uiScale;
    menuContext.values[3] = shellPrefs->timeFormat;
    menuContext.values[4] = shellPrefs->showCEaShellApp;
    menuContext.values[5] = shellPrefs->showAppsFolder;
    menuContext.values[6] = shellPrefs->showAppVarsFolder;
    menuContext.values[7] = shellPrefs->showHiddenProgs;
    menuContext.values[8] = shellPrefs->showFileCount;
    menuContext.values[9] = shellPrefs->apdTimer;
    menuContext.values[10] = '\0';
    menuContext.values[11] = '\0';

    int startY = 38;
    uint8_t optionY = 38;

    menu_Draw(shellPrefs, 15, 35, 38, 141, 168, &menuContext);
    gfx_BlitBuffer();

    while(kb_AnyKey());

    bool keyPressed = false;
    clock_t clockOffset = clock(); // Keep track of an offset for timer stuff

    while (!kb_IsDown(kb_KeyClear) && !kb_IsDown(kb_KeyYequ)) {
        kb_Scan();
        util_UpdateKeyTimer(shellPrefs, shellContext, &clockOffset, &keyPressed);

        if ((kb_Data[7] || kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter)) && (!keyPressed || clock() - clockOffset > CLOCKS_PER_SEC / 32)) {
            if (kb_IsDown(kb_KeyUp)) {
                if (menuContext.optionSelected) {
                    int nextY = optionY - 5 - menu_CalculateLines(menuContext.options[menuContext.optionSelected - 1], (141 - menu_DrawValueString(0, 0, menuContext.types[menuContext.optionSelected - 1], 0) - 3) / 8, 3) * 12;

                    if (nextY < 38) {
                        startY += 38 - nextY;
                        optionY = 38;
                    } else {
                        optionY = nextY;
                    }

                    menuContext.optionSelected -= 1;
                } else {
                    #ifdef FR
                    startY = -138;
                    #else
                    startY = -30;
                    #endif
                    optionY = 193;
                    menuContext.optionSelected = 11;
                }
            } else if (kb_IsDown(kb_KeyDown)) {
                if (menuContext.optionSelected + 1 < menuContext.totalOptions) {
                    // Create variables to not call these functions so much
                    uint8_t nextY = optionY + 5 + menu_CalculateLines(menuContext.options[menuContext.optionSelected], (141 - menu_DrawValueString(0, 0, menuContext.types[menuContext.optionSelected], 0) - 3) / 8, 3) * 12;
                    uint8_t nextOptionHeight = menu_CalculateLines(menuContext.options[menuContext.optionSelected + 1], (141 - menu_DrawValueString(0, 0, menuContext.types[menuContext.optionSelected + 1], 0) - 3) / 8, 3) * 8;

                    if (nextOptionHeight > 8) {
                        nextOptionHeight += 4 * (nextOptionHeight / 8 - 1);
                    }

                    if (nextY + nextOptionHeight + 1 > 201) {
                        startY -= nextY + nextOptionHeight - 201;
                        optionY = 201 - nextOptionHeight;
                    } else {
                        optionY = nextY;
                    }

                    menuContext.optionSelected += 1;
                } else {
                    startY = 38;
                    optionY = 38;
                    menuContext.optionSelected = 0;
                }
            }

            if (kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter)) {
                switch (menuContext.optionSelected) {
                    case 0:
                        shellPrefs->transitionSpeed = TRANSITION_MED * !shellPrefs->transitionSpeed;
                        menuContext.values[0] = shellPrefs->transitionSpeed;
                        menuContext.values[1] = shellPrefs->transitionSpeed;
                        break;
                    case 4:
                        shellPrefs->showCEaShellApp = !shellPrefs->showCEaShellApp;
                        menuContext.values[4] = shellPrefs->showCEaShellApp;

                        if (shellContext->fileSelected == shellContext->appCount - 1 && !menuContext.values[4] && shellContext->directory == APPS_FOLDER) {
                            shellContext->fileSelected -= 1;
                        }

                        break;
                    case 5:
                        shellPrefs->showAppsFolder = !shellPrefs->showAppsFolder;
                        menuContext.values[5] = shellPrefs->showAppsFolder;
                        break;
                    case 6:
                        shellPrefs->showAppVarsFolder = !shellPrefs->showAppVarsFolder;
                        menuContext.values[6] = shellPrefs->showAppVarsFolder;
                        break;
                    case 7:
                        shellPrefs->showHiddenProgs = !shellPrefs->showHiddenProgs;
                        menuContext.values[7] = shellPrefs->showHiddenProgs;

                        if (!menuContext.values[7]) {
                            shellContext->fileStartLoc = 0;
                            shellContext->fileSelected = 0;
                        }

                        break;
                    case 8:
                        shellPrefs->showFileCount = !shellPrefs->showFileCount;
                        menuContext.values[8] = shellPrefs->showFileCount;
                        break;
                    case 10:
                        custom_CreateTheme(shellPrefs, shellContext);
                        break;
                    case 11:
                        custom_PresetTheme(shellPrefs, shellContext);
                        break;
                    default:
                        break;
                }

                if (menuContext.optionSelected > 9) {
                    gfx_FillScreen(shellPrefs->bgColor);
                    gfx_SetTextFGColor(shellPrefs->textColor);

                    #ifdef FR
                    ui_DrawStatusBar(shellPrefs, shellContext, "Customiser");
                    #else
                    ui_DrawStatusBar(shellPrefs, shellContext, "Customize");
                    #endif

                    gfx_SetColor(shellPrefs->fgColor);

                    shapes_RoundRectangleFill(6, 8, 28, 304, 204);
                    ui_DrawUISprite(shellPrefs->fgColor, UI_LARROW, 14, 210);
                }
            } else if (kb_IsDown(kb_KeyLeft)) {
                switch (menuContext.optionSelected) {
                    case 1:
                        if (shellPrefs->transitionSpeed == TRANSITION_SLOW) {
                            shellPrefs->transitionSpeed = TRANSITION_FAST;
                        } else {
                            shellPrefs->transitionSpeed -= 1;
                        }

                        menuContext.values[0] = shellPrefs->transitionSpeed;
                        menuContext.values[1] = shellPrefs->transitionSpeed;
                        break;
                    case 2:
                        if (shellPrefs->uiScale == SCALE_SMALLEST) {
                            shellPrefs->uiScale = SCALE_LARGE;
                        } else {
                            shellPrefs->uiScale -= 1;
                        }

                        menuContext.values[2] = shellPrefs->uiScale;
                        shellContext->fileSelected = 0;
                        shellContext->fileStartLoc = 0;
                        break;
                    case 3:
                        shellPrefs->timeFormat = !shellPrefs->timeFormat;
                        menuContext.values[3] = shellPrefs->timeFormat;
                        break;
                    case 9:
                        if (!shellPrefs->apdTimer) {
                            shellPrefs->apdTimer = 5;
                        } else {
                            shellPrefs->apdTimer -= 1;
                        }

                        menuContext.values[9] = shellPrefs->apdTimer;
                        break;
                    default:
                        break;
                }
            } else if (kb_IsDown(kb_KeyRight)) {
                switch (menuContext.optionSelected) {
                    case 1:
                        if (shellPrefs->transitionSpeed == TRANSITION_FAST) {
                            shellPrefs->transitionSpeed = TRANSITION_SLOW;
                        } else {
                            shellPrefs->transitionSpeed += 1;
                        }

                        menuContext.values[0] = shellPrefs->transitionSpeed;
                        menuContext.values[1] = shellPrefs->transitionSpeed;
                        break;
                    case 2:
                        if (shellPrefs->uiScale == SCALE_LARGE) {
                            shellPrefs->uiScale = SCALE_SMALLEST;
                        } else {
                            shellPrefs->uiScale += 1;
                        }

                        menuContext.values[2] = shellPrefs->uiScale;
                        shellContext->fileSelected = 0;
                        shellContext->fileStartLoc = 0;
                        break;
                    case 3:
                        shellPrefs->timeFormat = !shellPrefs->timeFormat;
                        menuContext.values[3] = shellPrefs->timeFormat;
                        break;
                    case 9:
                        if (shellPrefs->apdTimer == 5) {
                            shellPrefs->apdTimer = 0;
                        } else {
                            shellPrefs->apdTimer += 1;
                        }

                        menuContext.values[9] = shellPrefs->apdTimer;
                        break;
                    default:
                        break;
                }
            }

            gfx_SetDrawBuffer();
            menu_Draw(shellPrefs, 15, 35, startY, 141, 168, &menuContext);
            gfx_BlitBuffer();

            util_WaitBeforeKeypress(&clockOffset, &keyPressed);
        }
    }

    util_CorrectTransparentColor(shellPrefs);
}
