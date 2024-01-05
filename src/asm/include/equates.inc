;--------------------------------------
;
; CEaShell Source Code - equates.inc
; By RoccoLox Programs and TIny_Hacker
; Copyright 2022 - 2024
; License: GPL-3.0
;
;--------------------------------------

include 'ti84pceg.inc'

; Flags

sortFlag := ti.asm_Flag1
sortFirstItemFound := 0
sortFirstHidden := 1
sortSecondHidden := 2

shellFlags := ti.asm_Flag2
disableBusyIndicator := 0
updateProgInfo := 1
exitF5Menu := 2
useASMToken := 3

sysHookFlg := ti.hookflags2
appInpPrmptInit := 0
appInpPrmptDone := 1
appWantHome := 4

; Expanded file types

typeASM := 0
typeC := 1
typeBasic := 2
typeICE := 3
typeSrc := 4
typeDir := 5
typeAppVar := 6
typeHidden := 7
typeCeltic := 8
typeApp := 9

; SPI defines

spiValid := 8
spiSearchStrSize := 11

; use appData RAM for these variables
;-------------------------------------

; Editor

isCelticVar := returnLoc + 1
lockOnExit := isCelticVar + 1
arcOnExit := lockOnExit + 1

errorOffset := arcOnExit + 1
editMode := errorOffset + 3 ; true for goto, false for no goto
monFlagBackup := editMode + 1

backupSP := monFlagBackup + 1
backupAppChangeRet := backupSP + 3

; Program running

isASM := backupAppVarName + 9
editArcProgs := isASM + 1
editLockProgs := editArcProgs + 1
getCSCHooks := editLockProgs + 1
returnLoc := getCSCHooks + 1

true := $FF
false := $00

returnOS := true
returnCEaShell := false

; Hooks

parserChainLoc := ti.appData
rawKeyHookLoc := parserChainLoc + 4
getcscHookLoc := rawKeyHookLoc + 4
appChangeHookLoc := getcscHookLoc + 4
backupPrgmName := appChangeHookLoc + 4
backupAppVarName := backupPrgmName + 9

hookTokenStop := $D9 - $CE

; use cursorImage RAM for these variables
;-----------------------------------------

; Apps

appPtr := ti.cursorImage + 100

; General

EOF := appPtr + 3

prgmName := appPtr - 9

; Icons / Descriptions

itemSelected := EOF + 3
iconData := itemSelected + 1
description := iconData + 256 ; this has to be at the end because it could be any size

; VAT / Filesystem

sortFirstItemFoundPtr := ti.mpLcdCrsrImage
sortEndOfPartPtr := ti.mpLcdCrsrImage + 3
sortVatEntrySize := ti.mpLcdCrsrImage + 6
sortVatEntryNewLoc := ti.mpLcdCrsrImage + 9
sortVatEntryTempEnd := ti.mpLcdCrsrImage + 12 + 15

; Label jumper

editCursor := ti.cursorImage + 3
editTail := editCursor + 3
labelNumber := editTail + 3
labelNumberOfPages := labelNumber + 3
labelPage := labelNumberOfPages + 3
labelName := labelPage + 3
