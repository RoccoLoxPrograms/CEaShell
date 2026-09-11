;--------------------------------------
;
; CEaShell Source Code - runProgram.s
; By RoccoLox Programs and TIny_Hacker
; Copyright 2022 - 2026
; License: GPL-3.0
;
;--------------------------------------

    .assume adl=1

    .include "src/asm/include/equates.inc"

    .global _asm_runProgram_run
    .type   _asm_runProgram_run, @function
    .global _asm_runProgram_main
    .type   _asm_runProgram_main, @function
    .global _asm_runProgram_returnOS.restoreHooks
    .type   _asm_runProgram_returnOS.restoreHooks, @function
    .global _asm_runProgram_error
    .type   _asm_runProgram_error, @function

    .extern _asm_apps_reloadApp
    .extern _asm_editProgram_goto
    .extern _asm_fileOps_getPrgmType
    .extern _asm_hooks_installStopHook
    .extern _asm_hooks_removeStopHook
    .extern _asm_hooks_installGetCSCHookCont
    .extern _asm_hooks_removeGetCSCHook
    .extern _asm_hooks_installAppChangeHook
    .extern _asm_hooks_removeAppChangeHook
    .extern _asm_hooks_installHomescreenHook
    .extern _asm_hooks_removeBasicKeyHook
    .extern _asm_hooks_installBasicKeyHook
    .extern _asm_hooks_basicPrgmHook
    .extern _asm_utils_findVar
    .extern _asm_utils_backupPrgmName
    .extern _asm_utils_lcdNormal
    .extern _asm_utils_clrScrnAndUsedRAM
    .extern _asm_utils_dispTextToolbar
    .extern _asm_utils_deleteTempRunner
    .extern _asm_utils_dispQuitErr
    .extern _rodata_errorQuit
    .extern _rodata_errorGoto
    .extern _rodata_errorQuitFR
    .extern _rodata_errorGotoFR
    .extern _rodata_waitHomescreen
    .extern _rodata_waitHomescreenFR
    .extern _rodata_basicPrgmName

;--------------------------------------

    .section .text

_asm_runProgram_run:
    ld iy, 0
    add iy, sp
    ld hl, (iy + 12)
    ld de, 15
    add hl, de
    ld de, (hl)
    ld (editArcProgs), de
    xor a, a ; returnCEaShell
    ld (returnLoc), a
    inc hl
    inc hl
    inc hl
    inc hl
    push iy
    ld iy, ti.flags
    set disableBusyIndicator, (iy + shellFlags)
    bit 0, (hl)
    jr nz, $ + 6
    res disableBusyIndicator, (iy + shellFlags)
    pop iy
    ld a, (iy + 6) ; program type
    ld hl, ti.OP1
    ld (hl), a
    inc hl
    ld de, (iy + 3)
    ld bc, 8
    ex de, hl
    ldir
    ld a, (iy + 9)

; OP1 = Program name
; a = Shell type

_asm_runProgram_main:
    push af
    ld iy, ti.flags

main.debounce:
    call ti.GetCSC
    or a, a
    jr nz, main.debounce
    call _asm_utils_backupPrgmName
    ld de, (ti.asm_prgm_size)
    or a, a
    sbc hl, hl
    ld (ti.asm_prgm_size), hl
    ld hl, ti.userMem
    call ti.DelMem
    call _asm_utils_findVar + 4
    ld (ti.asm_data_ptr1), de
    ld bc, 0
    ld a, (de)
    ld c, a
    inc de
    ld a, (de)
    ld b, a
    pop af
    or a, a
    jr z, main.runASM ; assembly program
    dec a
    jr z, main.runASM ; C program
    dec a
    jp z, main.runBasic ; BASIC program
    dec a
    jp nz, main.runBasic ; ICE program

main.runASM:
    ld hl, isASM
    ld (hl), true
    inc de
    push bc
    ld hl, 128
    add hl, bc
    call ti.EnoughMem
    pop hl
    ld a, ti.E_Memory
    jp c, _asm_runProgram_error
    ld (ti.asm_prgm_size), hl
    push hl
    push hl
    ld de, ti.userMem
    call ti.InsertMem
    pop bc
    xor a, a
    ld hl, ti.userMem
    push hl
    call ti.MemSet
    ld hl, (ti.asm_data_ptr1)
    inc hl ; skip size + $EF7B
    inc hl
    inc hl
    inc hl
    pop de
    pop bc
    ld a, b
    or a, c
    jr z, _asm_runProgram_main.ASMLoaded
    ldir

_asm_runProgram_main.ASMLoaded:
    call ti.DisableAPD
    set ti.appAutoScroll, (iy + ti.appFlags)
    call runProgram_vectorsSetup
    ld hl, runProgram_return
    push hl
    jp ti.userMem

main.runBasic:
    ld hl, isASM
    ld (hl), false
    inc de
    ld a, (de)
    cp a, ti.tExtTok
    jr nz, main.noSquish
    inc de
    ld a, (de)
    cp a, ti.tAsm84CePrgm
    jr z, main.squish
    dec de

main.noSquish:
    call ti.ChkInRam
    jr z, main.loadComplete + 4
    push de
    push bc
    ld hl, 128
    add hl, bc
    call ti.EnoughMem
    jr nc, $ + 10
    pop bc
    pop de
    ld a, ti.E_Memory
    jp _asm_runProgram_error
    ld hl, _rodata_basicPrgmName
    call ti.Mov9ToOP1
    pop hl
    push hl
    call ti.CreateProtProg
    inc de
    inc de
    pop bc
    pop hl
    ld a, b
    or a, c
    jr z, main.loadComplete
    ldir

main.loadComplete:
    call ti.OP4ToOP1
    call ti.RunIndicOn
    call ti.EnableAPD
    bit disableBusyIndicator, (iy + shellFlags)
    call nz, ti.RunIndicOff
    set ti.progExecuting, (iy + ti.newDispF)
    set ti.cmdExec, (iy + ti.cmdFlags)
    call runProgram_vectorsSetup
    call _asm_hooks_installStopHook
    call _asm_hooks_installBasicKeyHook
    ld de, _asm_hooks_basicPrgmHook
    call _asm_hooks_installAppChangeHook
    ei
    ld hl, runProgram_return
    push hl
    jp ti.ParseInp

main.squish:
    ex de, hl
    inc hl
    dec bc
    dec bc ; remove Asm84CEPrgm from un-squished size
    ld a, b
    or a, c
    jr z, main.error
    push bc
    push bc

main.loopNewlines:
    ld a, (hl)
    cp a, ti.tEnter
    jr nz, main.notNewline
    pop de
    dec de
    push de

main.notNewline:
    inc hl
    dec bc
    ld a, b
    or a, c
    jr nz, main.loopNewlines
    pop hl ; un-squished size minus newlines
    bit 0, l
    jr nz, main.error - 2 ; check if an odd number of tokens
    ld a, h
    or a, l
    jr nz, main.notEmpty
    pop hl
    pop hl

main.error:
    ld a, ti.E_Syntax
    jp _asm_runProgram_error

main.notEmpty:
    push hl
    ld de, 128
    add hl, de
    call ti.EnoughMem
    pop hl
    ld a, ti.E_Memory
    jp c, _asm_runProgram_error
    ld (ti.asm_prgm_size), hl
    push hl
    ld de, ti.userMem
    call ti.InsertMem
    pop bc
    xor a, a
    ld hl, ti.userMem
    push hl
    call ti.MemSet
    ld hl, (ti.asm_data_ptr1)
    inc hl
    inc hl
    inc hl
    inc hl
    pop de ; userMem
    pop bc ; original (un-squished) size

main.loopLoad:
    ld a, b
    or a, c
    jp z, _asm_runProgram_main.ASMLoaded
    ld a, (hl)
    inc hl
    dec bc
    cp a, ti.tEnter
    jr z, main.loopLoad
    push de
    call runProgram_convertTokenToHex
    add a, a
    add a, a
    add a, a
    add a, a
    ld e, a
    ld a, (hl)
    inc hl
    dec bc
    call runProgram_convertTokenToHex
    add a, e
    pop de
    ld (de), a
    inc de
    jr main.loopLoad

runProgram_return:
    ld a, (returnLoc)
    or a, a
    jr nz, return.chkDone
    ld hl, ti.textShadow
    ld bc, 260

return.chkHomescreen:
    ld a, (hl)
    cp a, ' '
    jr nz, return.chkDonePause
    inc hl
    dec bc
    ld a, b
    or a, c
    jr nz, return.chkHomescreen
    jr return.chkDone

return.chkDonePause:
    ld.sis hl, (ti.localLanguage & $FFFF)
    or a, a
    ld de, $010C ; check for French language
    sbc hl, de
    ld hl, _rodata_waitHomescreen
    jr nz, $ + 6
    ld hl, _rodata_waitHomescreenFR
    call _asm_utils_dispTextToolbar

return.loopPause:
    call ti.GetCSC
    or a, a
    jr z, return.loopPause
    call ti.os.ClearStatusBarLow

return.chkDone:
    call ti.PopErrorHandler
    xor a, a
    res error, (iy + shellFlags)

runProgram_return.error:
    push af
    res ti.progExecuting, (iy + ti.newDispF)
    res ti.cmdExec, (iy + ti.cmdFlags)
    res ti.textInverse, (iy + ti.textFlags)
    res ti.onInterrupt, (iy + ti.onFlags)
    call ti.ReloadAppEntryVecs
    call _asm_hooks_removeStopHook
    call _asm_hooks_removeBasicKeyHook
    ld de, _asm_hooks_basicPrgmHook
    call _asm_hooks_removeAppChangeHook
    pop bc
    ld a, b
    or a, a
    call nz, runProgram_showError

return.quit:
    call ti.DeleteTempPrograms
    call ti.CleanAll
    call ti.ReloadAppEntryVecs
    call ti.ForceFullScreen
    call _asm_utils_deleteTempRunner
    ld de, (ti.asm_prgm_size)
    or a, a
    sbc hl, hl
    ld (ti.asm_prgm_size), hl
    ld hl, ti.userMem
    call ti.DelMem
    res appWantHome, (iy + sysHookFlg)

return.debounce:
    call ti.GetCSC
    or a, a
    jr nz, return.debounce
    xor a, a
    ld (ti.kbdGetKy), a
    bit ti.monAbandon, (iy + ti.monFlags)
    jr nz, _asm_runProgram_returnOS
    ld a, (returnLoc)
    or a, a
    jp z, _asm_apps_reloadApp

_asm_runProgram_returnOS:
    call ti.SaveCmdShadow
    ld a, ti.cxCmd
    call ti.NewContext0
    bit error, (iy + shellFlags)
    jr z, returnOS.skipClear
    call ti.ClrScrn
    call ti.HomeUp

returnOS.skipClear:
    call ti.PPutAway

_asm_runProgram_returnOS.restoreHooks:
    ld a, (editArcProgs)
    bit 0, a
    jr nz, $ + 8
    ld a, (editLockProgs)
    bit 0, a
    call nz, _asm_hooks_installHomescreenHook
    ld a, (getCSCHooks)
    call _asm_hooks_installGetCSCHookCont
    xor a, a
    or a, 2
    ret

runProgram_showError:
    set error, (iy + shellFlags)
    xor a, a
    ld (ti.menuCurrent), a
    ld a, (ti.errNo)
    or a, a
    ret z
    call ti.boot.ClearVRAM
    ld a, $2D
    ld (ti.mpLcdCtrl), a
    call ti.CursorOff
    call ti.DrawStatusBar
    call runProgram_getErrorOffset
    ld hl, _rodata_basicPrgmName
    push hl
    call ti.Mov9ToOP1
    call ti.ChkFindSym
    call nc, ti.DelVarArc
    ld hl, ti.basic_prog
    pop de
    push hl
    ld b, 9
    call ti.StrCmpre
    pop de
    jr z, $ + 17
    ld a, (ti.basic_prog)
    cp a, ti.EquObj ; check for possible temp var
    jr nz, $ + 17
    or a, a
    sbc hl, hl
    ld (errorOffset), hl
    ld hl, backupPrgmName
    call ti.Mov9b
    call ti.DispErrorScreen
    ld hl, 1
    ld (ti.curRow), hl
    ld.sis hl, (ti.localLanguage & $FFFF)
    or a, a
    ld de, $010C ; check for French language
    sbc hl, de
    ld hl, _rodata_errorQuit
    jr nz, $ + 6
    ld hl, _rodata_errorQuitFR
    set ti.textInverse, (iy + ti.textFlags)
    call ti.PutS
    res ti.textInverse, (iy + ti.textFlags)
    call ti.PutS
    ld hl, ti.basic_prog
    ld a, (hl)
    cp a, ti.ProtProgObj
    ld a, false
    ld (lockOnExit), a
    jr nz, showError.notProtected
    ld a, (isASM)
    or a, a
    jp nz, showError.onlyAllowQuit
    call ti.Mov9ToOP1
    call ti.ChkFindSym
    push hl
    call _asm_fileOps_getPrgmType
    pop hl
    cp a, typeBasic
    jr z, showError.checkAllowLock
    cp a, typeSrc
    jp nz, showError.onlyAllowQuit

showError.checkAllowLock:
    ld a, (editLockProgs)
    bit 0, a
    jp z, showError.onlyAllowQuit

showError.notProtected:
    ld hl, 2
    ld (ti.curRow), hl
    ld.sis hl, (ti.localLanguage & $FFFF)
    or a, a
    ld de, $010C ; check for French language
    sbc hl, de
    ld hl, _rodata_errorGoto
    jr nz, $ + 6
    ld hl, _rodata_errorGotoFR
    call ti.PutS
    call ti.PutS

showError.input:
    call ti.GetCSC
    cp a, ti.skUp
    jr z, showError.highlight1
    cp a, ti.skDown
    jr z, showError.highlight2
    cp a, ti.sk2
    jr z, showError.goto
    cp a, ti.sk1
    ret z
    cp a, ti.skEnter
    jr z, showError.getOption
    jr showError.input

showError.highlight1:
    ld hl, 1
    ld de, 2
    ld a, '1'
    ld b, '2'
    jr showError.highlight

showError.highlight2:
    ld hl, 2
    ld de, 1
    ld a, '2'
    ld b, '1'

showError.highlight:
    push bc
    push de
    ld (ti.curRow), hl
    ld hl, ti.OP6
    ld (hl), a
    inc hl
    ld (hl), ':'
    inc hl
    ld (hl), 0
    dec hl
    dec hl
    push hl
    scf
    sbc hl, hl
    ld.sis (ti.fillRectColor & $FFFF), hl
    inc hl
    ld de, 25
    ld bc, (55 << 8) | 96
    call ti.FillRect
    pop hl
    set ti.textInverse, (iy + ti.textFlags)
    call ti.PutS
    res ti.textInverse, (iy + ti.textFlags)
    pop de
    pop bc
    ld (ti.curRow), de
    ld hl, ti.OP6
    ld (hl), b
    call ti.PutS
    jr showError.input

showError.getOption:
    ld a, (ti.curRow)
    dec a
    ret nz

showError.goto:
    ld hl, ti.basic_prog
    call ti.Mov9ToOP1
    jp _asm_editProgram_goto

showError.onlyAllowQuit:
    call ti.GetCSC
    cp a, ti.sk1
    ret z
    cp a, ti.skEnter
    ret z
    jr showError.onlyAllowQuit

_asm_runProgram_error:
    push af
    ld a, (returnLoc)
    or a, a
    jr nz, error.exitOS
    pop af
    ld (ti.errNo), a
    call _asm_utils_lcdNormal
    call _asm_utils_dispQuitErr

error.waitLoop:
    call ti.GetCSC
    cp a, ti.sk1
    jr z, $ + 4
    cp a, ti.skEnter
    jp z, _asm_apps_reloadApp - 4
    jr error.waitLoop

error.exitOS:
    pop af
    jp ti.JError

runProgram_vectorsSetup:
    call _asm_utils_lcdNormal
    ld a, $25
    ld ($D02687), a
    xor a, a
    ld (ti.menuCurrent), a
    ld (ti.appErr1), a
    ld (ti.kbdGetKy), a
    ld hl, runProgram_vectors
    call ti.AppInit
    call _asm_utils_clrScrnAndUsedRAM
    call _asm_hooks_removeGetCSCHook
    ld hl, runProgram_return.error
    jp ti.PushErrorHandler

runProgram_vectors:
    .d24 vectors.ret
    .d24 ti.SaveCmdShadow
    .d24 vectors.putaway
    .d24 vectors.restore
    .d24 vectors.ret
    .d24 vectors.ret

vectors.ret:
    ret

vectors.putaway:
    xor a, a
    ld (ti.currLastEntry), a
    bit appInpPrmptInit, (iy + ti.apiFlg2)
    call z, ti.ForceFullScreen
    call ti.ReloadAppEntryVecs
    call ti.PutAway
    ld b, 0
    ret

vectors.restore:
    call ti.HomeUp
    call ti.ClrScrn
    jp ti.RStrShadow

runProgram_convertTokenToHex:
    cp a, ti.t0
    jr c, convertTokenToHex.error
    cp a, ti.t9 + 1
    jr nc, convertTokenToHex.skip
    jr convertTokenToHex.convert

convertTokenToHex.error:
    pop hl
    pop hl
    ld a, ti.E_Syntax
    jp _asm_runProgram_error

convertTokenToHex.skip:
    cp a, ti.tA
    jr c, convertTokenToHex.error
    cp a, ti.tF + 1
    jr nc, convertTokenToHex.error

convertTokenToHex.convert:
    sub a, ti.t0
    cp a, $0A
    ret c
    sub a, 7
    ret

runProgram_getErrorOffset:
    ld hl, ti.basic_prog
    call ti.Mov9ToOP1
    ld de, (ti.begPC)
    ld hl, (ti.curPC)
    or a, a
    sbc hl, de

getErrorOffset.loop:
    ld (errorOffset), hl
    ld a, (ti.OP1 + 1)
    cp a, $24
    jr nz, getErrorOffset.notTempParser
    call ti.FindSym
    ex de, hl
    inc hl
    inc hl
    ld a, (hl)
    or a, a
    ret z
    inc hl
    push af
    push hl
    call ti.ZeroOP1
    pop hl
    ld b, (hl)
    ld c, b
    inc hl
    ld de, ti.OP1
    pop af
    ld (de), a
    inc de

getErrorOffset.loadName:
    ldi
    djnz getErrorOffset.loadName
    ld e, (hl)
    inc hl
    ld d, (hl)
    ld hl, (errorOffset)
    add.sil hl, de
    jr getErrorOffset.loop

getErrorOffset.notTempParser:
    push hl
    ld de, ti.basic_prog
    call ti.MovFrOP1
    pop hl
    ld (errorOffset), hl
    call ti.DeleteTempPrograms
    jp ti.CleanAll
