/**
 * --------------------------------------
 * 
 * CEaShell Source Code - defines.h
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2024
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#ifndef DEFINES_H
#define DEFINES_H

#include <graphx.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Check a specified bit in a number.
 * 
 */
#define bit(number, bit) ((number) & (1U << (bit)))

/**
 * @brief Toggles a specified bit in a number.
 * 
 */
#define toggle(number, bit) (number ^ (1U << bit))

/**
 * @brief PixelShadow RAM location.
 * 
 */
#define os_PixelShadow          ((uint8_t *)0xD031F6)

/**
 * Entry format for launch shortcut.
 */
struct launch_t {
    uint8_t type;               /** Type of the file to launch (Program / App) */
    char name[9];               /** Name of the file to launch. */
};

/** 
 * Shell preferences, which are backed up in the CEaShell AppVar.
 */
struct preferences_t {
    bool invertColors;                  /** Whether or not to invert the color palette used in CEaShell. */
    uint8_t bgColor;                    /** CEaShell theme background color. */
    uint8_t fgColor;                    /** CEaShell theme foreground color. */
    uint8_t hlColor;                    /** CEaShell theme highlight color. */
    uint8_t textColor;                  /** Color to use for text. */
    uint8_t hiddenTextColor;            /** Color to use for hidden text. */
    uint8_t transitionSpeed;            /** Speed of CEaShell's transition animation. 0 = Off, 1 = Slow, 2 = Default, 3 = Fast. */
    uint8_t uiScale;                    /** Scale factor for programs in the file manager context. Default value is 4. */
    bool timeFormat;                    /** Format to display the clock in CEaShell. If the value is true, use 24 hour time, and if it is false, use 12 hour time. */
    bool showCEaShellApp;               /** Whether or not to display the CEaShell app in the list of apps found in CEaShell. */
    bool showFileCount;                 /** Whether or not to display the number of files in the currently opened directory in CEaShell's status bar. */
    uint8_t apdTimer;                   /** Timer to turn off the calculator after a certain amount of inactivity. If the value is 0, this feature is disaled, if it is 1, it will turn off after 1 minute, etc. Default is 3 minutes. */
    bool showAppsFolder;                /** Whether or not to display the apps directory in CEaShell. */
    bool showAppVarsFolder;             /** Whether or not to display the AppVars directory in CEaShell. */
    bool showHiddenProgs;               /** Whether or not to show hidden programs in the programs directory of CEaShell. */
    uint8_t editArchivedProgs;          /** Whether or not to allow the editing of archived programs in TI-OS. */
    bool editLockedProgs;               /** Whether or not to allow the editing of locked TI-BASIC and ICE source files in CEaShell. */
    uint8_t getCSCHooks;                /** Combination of GetCSC hooks currently installed. Bit 0 for icon / description hook, bit 1 for on shortcuts, bit 2 for fast alpha scrolling. */
    uint8_t hidePrgmOptions;            /** Hide menu for Python or TI-BASIC programs in the program menu, skipping immediately to TI-BASIC programs. */
    bool hideBusyIndicator;             /** Whether or not to hide the busy indicator when running TI-BASIC programs with CEaShell or the homescreen hook. */
    bool osLowercase;                   /** Whether or not to enable lowercase in TI-OS. */
    struct launch_t launchFiles[10];    /** Information for [on] + [number] launch shortcut. */
};

/**
 * Shell context information, which is modified during shell usage.
 */
struct context_t {
    char searchString[9];       /** Contains a search condition if the user is in search mode, otherwise contains NULL. */
    uint8_t batteryLevel;       /** Calculator's current battey level. */
    uint8_t directory;          /** Current directory open in CEaShell. */
    unsigned int fileSelected;  /** Current file selected in the list of files. 0 is the first file, 1 is the second, etc. */
    unsigned int fileStartLoc;  /** File to begin displaying the current page of files at. 0 is the first file, 1 is the second, etc. */
    unsigned int programCount;  /** Total count of programs on the calculator. This does not include hidden programs if showHiddenProgs is false. */
    unsigned int appVarCount;   /** Total count of AppVars on the calculator. */
    unsigned int appCount;      /** Total count of apps on the calculator. */
    void **programPtrs;         /** Array containing a cache of all program VAT pointers. */
    void **appVarPtrs;          /** Array containing a cache of all AppVar VAT pointers. */
    void **appPtrs;             /** Array containing a cache of all app pointers. */
};

/** 
 * Various important file attributes.
 */
struct file_t {
    char name[9];               /** Name of the file. */
    gfx_sprite_t *icon;         /** File icon, or NULL if no icon exists. */
    char *minimumOSVersion;     /** Minimum OS version for apps. */
    char description[53];       /** File description, or NULL if no description exists. */
    uint8_t type;               /** OS type. */
    uint8_t shellType;          /** Shell type. */
    unsigned int size;          /** File size. */
    bool archived;              /** Archive status. */
    bool locked;                /** Locked status. */
    bool hidden;                /** Hidden status. */
    uint8_t shortcut;           /** Number key assigned to the file for launch shortcut. */
};

/** 
 * Menu context information.
 */
struct menu_t {
    uint8_t totalOptions;
    uint8_t optionSelected;
    unsigned int totalHeight;
    char *options[12];
    char *details[12];
    uint8_t types[12];
    uint8_t values[12];
    void (*callback)(struct preferences_t *shellPrefs, struct context_t *shellContext, struct menu_t *menuContext);
};

/** 
 * Temporary sprite space for file icons.
 */
extern gfx_sprite_t *fileIcon;

/** 
 * Temporary sprite space for drawing functions.
 */
extern gfx_sprite_t *tempSprite;

/** 
 * Timer defines for turning off the calculator after a set amount of
 * inactivity.
*/
#define ONE_SECOND      32768               /** One second. */
#define ONE_MINUTE      (ONE_SECOND * 60)   /** One minute. */

/**
 * Expanded file types for use in the shell. These are based on the file type
 * codes used in Cesium.
 */
#define ASM_TYPE        0       /** eZ80 assembly programs. */
#define C_TYPE          1       /** C programs. */
#define BASIC_TYPE      2       /** TI-BASIC programs. */
#define ICE_TYPE        3       /** Compiled ICE programs. */
#define ICE_SRC_TYPE    4       /** ICE source code. */
#define DIR_TYPE        5       /** CEaShell special directories. */
#define APPVAR_TYPE     6       /** AppVars. */
#define HIDDEN_TYPE     7       /** Deprecated. */
#define CELTIC_TYPE     8       /** AppVars with the CelticCE header. */
#define APP_TYPE        9       /** Apps. */
#define UNKNOWN_TYPE    10      /** Unknown file type. */

/**
 * Type byte for apps (ti.AppObj), which is not included in the CE C
 * toolchain.
 */
#define OS_TYPE_APP     0x14

/** 
 * Maximum length for program descriptions.
 */
#define MAX_DESC_LENGTH 53

/**
 * Directory currently opened in CEaShell.
 */
#define PROGRAMS_FOLDER 0       /** Programs folder (main screen). */
#define APPVARS_FOLDER  1       /** AppVars folder. */
#define APPS_FOLDER     2       /** Apps folder. */

/** 
 * Information on which GetCSC hooks are installed through CEaShell.
 */
#define ICON_HOOK       0       /** Icon / Description hook bit. */
#define ON_SHORTS_HOOK  1       /** [on] shorcuts hook bit. */
#define FAST_ALPHA_HOOK 2       /** Fast alpha hook bit. */
#define ALL_HOOKS       7       /** All hooks installed. (Bits 0, 1, and 2 are set.) */

/**
 * Transition speeds.
 */
#define TRANSITION_OFF  0       /** Transitions off. */
#define TRANSITION_SLOW 1       /** Slow transition speed. */
#define TRANSITION_MED  2       /** Medium transition speed. */
#define TRANSITION_FAST 3       /** Fast transition speed. */

/**  
 * File icon UI scaling.
 */
#define SCALE_SMALLEST  1       /** Smallest UI scale (16 * 16 file icons). */
#define SCALE_SMALL     2       /** Small UI scale (32 * 32 file icons). */
#define SCALE_MEDIUM    3       /** Medium UI scale (48 * 48 file icons). */
#define SCALE_LARGE     4       /** Large UI scale (64 * 64 file icons). */

/** 
 * Current input mode used when getting text input from the user.
 */
#define INPUT_NUMBER    0       /** Numbers and symbols. */
#define INPUT_UPPER     1       /** Uppercase letters and other alpha characters. */
#define INPUT_LOWER     2       /** Lowercase letters and other alpha characters. */

/** 
 * Sprite offsets used with ui_DrawSprite, used to simplify
 * displaying dark / light icons.
 */
#define UI_BATTERY      0       /** Battery icon used in CEaShell's status bar. */
#define UI_CHARGING     1       /** Charging indicator. */
#define UI_PAINT        2       /** Paint brush icon for the customization menu button. */
#define UI_INFO         3       /** Info icon for the description bar. */
#define UI_SETTINGS     4       /** Settings icon for the settings menu button. */
#define UI_LARROW       5       /** Left arrow icon used for closing menus. */
#define UI_RARROW       6       /** Right arrow icon used for closing menus. */
#define UI_DARROW       7       /** Down arrow icon used for closing menus. */
#define UI_CHECK        8       /** Checkmark icon used for checkbox options in menus. */

/** 
 * Different types of options in menus.
 */
#define MENU_TYPE_MENU  0       /** Option opens a new menu. */
#define MENU_TYPE_BOOL  1       /** Option has a boolean type. (On / Off) */
#define MENU_TYPE_APD   2       /** APD timer. */
#define MENU_TYPE_TIME  3       /** Time format. */
#define MENU_TYPE_SPEED 4       /** Transition speed. */
#define MENU_TYPE_SCALE 5       /** UI Scale. */
#define MENU_TYPE_HKEY  6       /** [on] + number hotkey. */

/** 
 * Number of preset theme options.
 */
#define THEME_COUNT     10

/** 
 * Size of CEaShell's AppVar.
 */
#define APPVAR_SIZE     129

/**
 * CEaShell app name.
 */
extern char rodata_appName;

#ifdef __cplusplus
}
#endif

#endif
