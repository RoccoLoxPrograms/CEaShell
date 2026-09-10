;--------------------------------------
;
; CEaShell Source Code - equates.inc
; By RoccoLox Programs and TIny_Hacker
; Copyright 2022 - 2026
; License: GPL-3.0
;
;--------------------------------------

.include "src/asm/include/ti84pceg.inc"

; Flags

.equ shellFlags, ti.asm_Flag2
.equ disableBusyIndicator, 0
.equ updateProgInfo, 1
.equ useASMToken, 2
.equ frenchLanguage, 4
.equ error, 5

.equ sysHookFlg, ti.hookflags2
.equ appInpPrmptInit, 0
.equ appWantHome, 4

.equ celticFlags1, ti.asm_Flag1
.equ showLineNum, 5

.equ celticFlags2, ti.asm_Flag2
.equ keyPressed, 3

; System calls (should be in the include)

.equ ShowResult, 0x021890

; Result types

.equ resultError, 0x0E
.equ resultDone, 0x15

; Expanded file types

.equ typeASM, 0
.equ typeC, 1
.equ typeBasic, 2
.equ typeICE, 3
.equ typeSrc, 4
.equ typeDir, 5
.equ typeAppVar, 6
.equ typeHidden, 7
.equ typeCeltic, 8
.equ typeApp, 9

; SPI defines

.equ spiValid, 8

; use appData RAM for these variables
;-------------------------------------

; Editor

.equ isCelticVar, returnLoc + 1
.equ lockOnExit, isCelticVar + 1
.equ arcOnExit, lockOnExit + 1
.equ exitLaunchHook, arcOnExit + 1

.equ errorOffset, exitLaunchHook + 1
.equ editMode, errorOffset + 3 ; resultError for goto, false for no goto
.equ stackBackup, editMode + 1 ; 18 bytes of memory for copying the top 6 stack entries for exiting CEaShell's editor

; Program running

.equ isASM, backupAppVarName + 9
.equ editArcProgs, isASM + 1
.equ editLockProgs, editArcProgs + 1
.equ getCSCHooks, editLockProgs + 1
.equ returnLoc, getCSCHooks + 1

.equ true, 0xFF
.equ false, 0x00

.equ returnOS, true
.equ returnCEaShell, false

; Hooks

.equ parserChainLoc, ti.appData
.equ rawKeyHookLoc, parserChainLoc + 4
.equ getcscHookLoc, rawKeyHookLoc + 4
.equ appChangeHookLoc, getcscHookLoc + 4
.equ backupPrgmName, appChangeHookLoc + 4
.equ backupAppVarName, backupPrgmName + 9

.equ hookTokenStop, 0xD9 - 0xCE

; Renaming
.equ oldName, backupPrgmName
.equ newName, backupAppVarName

; use cursorImage RAM for these variables
;-----------------------------------------

; General

.equ EOF, ti.cursorImage + 100

.equ prgmName, EOF - 9

; Icons / Descriptions

.equ itemSelected, EOF + 3
.equ iconData, getCSCvalBC + 3
.equ description, iconData + 256 ; this has to be at the end because it could be any size

; Fast alpha scrolling

.equ newlineCount, itemSelected + 1

; GetCSC hook backup values

.equ getCSCvalA, newlineCount + 1
.equ getCSCvalBC, getCSCvalA + 1

; VAT / Filesystem

.equ sortFirstItemFoundPtr, ti.mpLcdCrsrImage
.equ sortEndOfPartPtr, ti.mpLcdCrsrImage + 3
.equ sortVatEntrySize, ti.mpLcdCrsrImage + 6
.equ sortVatEntryNewLoc, ti.mpLcdCrsrImage + 9
.equ sortVatEntryTempEnd, ti.mpLcdCrsrImage + 12 + 15

; Label jumper

.equ editCursor, ti.cursorImage + 3
.equ editTail, editCursor + 3
.equ labelNumber, editTail + 3
.equ labelNumberOfPages, labelNumber + 3
.equ labelPage, labelNumberOfPages + 3
.equ labelName, labelPage + 3
