;--------------------------------------
;
; CEaShell Source Code - editProgram.asm
; By RoccoLox Programs and TIny_Hacker
; Copyright 2022 - 2024
; License: GPL-3.0
;
;--------------------------------------

    assume adl=1

    section .text

include 'include/equates.inc'

    public _asm_editProgram_edit
    public _asm_editProgram_goto
    public _asm_editProgram_main
    public _asm_editProgram_restoreAppVar

    extern _asm_hooks_installAppChangeHook
    extern _asm_hooks_editorHook
    extern _asm_runProgram_returnOS.restoreHooks
    extern _asm_runProgram_error
    extern _asm_utils_arcUnarc
    extern _asm_utils_backupPrgmName
    extern _asm_utils_clrScrnAndUsedRAM
    extern _asm_utils_lcdNormal
    extern _rodata_tempAppVarPrgm

_asm_editProgram_edit: ; editing from CEaShell
    ld iy, 0
    add iy, sp
    ld hl, (iy + 9)
    ld de, 15
    add hl, de
    ld de, (hl)
    ld (editArcProgs), de
    xor a, a
    ld (returnLoc), a
    ld (isCelticVar), a
    ld hl, ti.OP1
    ld (hl), ti.ProgObj
    ld de, (iy + 3) ; program name
    ld a, (iy + 6) ; celtic var status
    bit 0, a
    jr z, .loadName
    ld (hl), ti.AppVarObj
    ld a, true
    ld (isCelticVar), a

.loadName:
    inc hl
    ex de, hl
    ld bc, 8
    ldir
    xor a, a
    ld (arcOnExit), a
    ld a, (isCelticVar)
    or a, a
    call nz, editProgram_prepAppVar
    xor a, a
    sbc hl, hl
    ld (errorOffset), hl
    ld (editMode), a
    call _asm_utils_lcdNormal
    xor a, a
    jr _asm_editProgram_main + 5

_asm_editProgram_goto:
    call _asm_runProgram_returnOS.restoreHooks
    ld a, resultError
    ld (editMode), a
    xor a, a
    ld (isCelticVar), a

_asm_editProgram_main: ; OP1 = File name to edit
    xor a, a
    ld (arcOnExit), a
    ld (lockOnExit), a
    ld (exitLaunchHook), a
    ld de, (ti.asm_prgm_size)
    or a, a
    sbc hl, hl
    ld (ti.asm_prgm_size), hl
    ld hl, ti.userMem
    call ti.DelMem
    ; make sure these flags are these values, for Celtic compatibility
    res keyPressed, (iy + celticFlags2)
    set showLineNum, (iy + celticFlags1)
    call ti.ChkFindSym
    ld a, (hl)
    cp a, ti.ProgObj
    jr z, .notLocked
    ld (hl), ti.ProgObj
    ld hl, lockOnExit
    ld (hl), true

.notLocked:
    call ti.ChkInRam
    jr z, .inRam
    call _asm_utils_arcUnarc
    ld hl, arcOnExit
    ld (hl), true

.inRam:
    call _asm_utils_backupPrgmName
    ld de, _asm_hooks_editorHook
    call _asm_hooks_installAppChangeHook
    xor a, a
    ld (ti.menuCurrent), a
    call ti.CursorOff
    call ti.RunIndicOff
    call ti.DrawStatusBar
    ld hl, backupPrgmName
    push hl
    inc hl
    ld de, ti.progToEdit
    call ti.Mov9b
    pop hl
    ld de, ti.basic_prog
    call ti.Mov9b
    ld hl, editProgram_editHelper
    ld de, ti.cursorImage + 256
    ld bc, editHelperSize
    ldir ; relocate editProgram_editHelper to ti.cursorImage + 256
    jp ti.cursorImage + 256

editProgram_editHelper:
    ld a, ti.cxPrgmEdit
    call ti.NewContext
    xor a, a
    ld (ti.winTop), a
    call ti.ScrollUp
    call ti.HomeUp
    ld a, ':'
    call ti.PutC
    ld a, (editMode)
    or a, a
    jr z, .noGoto
    ld hl, (ti.editTop)
    ld de, (ti.editCursor)
    or a, a
    sbc hl, de
    add hl, de
    jr nz, .endGoto
    ld bc, (errorOffset)
    ld a, b
    or a, c
    jr z, .endGoto
    ld hl, (ti.editTail)
    ldir
    ld (ti.editTail), hl
    ld (ti.editCursor), de
    call ti.cursorImage + 256 + (.newLineGoto - editProgram_editHelper)
    call ti.DispEOW
    ld hl, $100
    ld (ti.curRow), hl

.correctCursor:
    ld hl, (ti.editCursor)
    ld de, (ti.editTop)
    or a, a
    sbc hl, de
    ld de, (errorOffset)
    sbc hl, de
    jr nc, .skip
    call ti.CursorRight
    jr .correctCursor

.endGoto:
    call ti.DispEOW
    ld hl, $100
    ld (ti.curRow), hl
    jr .skip

.noGoto:
    call ti.DispEOW
    ld hl, $100
    ld (ti.curRow), hl
    call ti.BufToTop

.skip:
    xor a, a
    ld (ti.menuCurrent), a
    set 7, (iy + $28)
    jp ti.Mon

.newLineGoto:
    ld hl, (ti.editCursor)
    ld a, (hl)
    cp a, ti.tEnter
    jr z, .newLineGotoBack

.loop:
    ld a, (hl)
    ld de, (ti.editTop)
    or a, a
    sbc hl, de
    ret z
    add hl, de
    dec hl
    push af
    ld a, (hl)
    call ti.Isa2ByteTok
    pop de
    jr z, .newLineGotoBack
    ld a, d
    cp a, ti.tEnter
    jr z, .newLineGotoNext

.newLineGotoBack:
    call ti.BufLeft
    ld hl, (ti.editCursor)
    jr .loop

.newLineGotoNext:
    jp ti.BufRight

editHelperSize := $ - editProgram_editHelper

editProgram_prepAppVar:
    ld hl, ti.OP1
    ld de, backupAppVarName
    call ti.Mov9b
    call ti.ChkFindSym
    call ti.ChkInRam
    jr z, .inRam
    ld a, true
    ld (arcOnExit), a
    ld hl, 10
    add hl, de
    ld a, c
    ld bc, 0
    ld c, a
    add hl, bc
    ex de, hl

.inRam:
    ex de, hl
    ld de, 0
    ld e, (hl)
    inc hl
    ld d, (hl)
    push de
    push de
    ld hl, _rodata_tempAppVarPrgm
    call ti.Mov9ToOP1
    pop hl
    push hl
    call ti.EnoughMem
    pop hl
    ld a, ti.E_Memory
    jp c, _asm_runProgram_error
    call ti.CreateProg
    push de
    ld hl, backupAppVarName
    call ti.Mov9ToOP1
    call ti.ChkFindSym ; the AppVar could've moved if it was in RAM
    call ti.ChkInRam
    jr z, .inRam2
    ld hl, 10
    add hl, de
    ld a, c
    ld bc, 0
    ld c, a
    add hl, bc
    ex de, hl

.inRam2:
    ex de, hl
    inc hl
    inc hl
    pop de
    inc de
    inc de
    pop bc
    ld a, b
    or a, c
    jr z, .loadComplete
    ldir

.loadComplete:
    ld hl, _rodata_tempAppVarPrgm
    jp ti.Mov9ToOP1

_asm_editProgram_restoreAppVar:
    call ti.PushOP1 ; temp program for editing the appvar
    ld hl, backupAppVarName
    call ti.Mov9ToOP1
    call ti.ChkFindSym
    call nc, ti.DelVarArc
    call ti.PopOP1
    call ti.ChkFindSym
    push hl
    push de
    push bc
    ex de, hl
    ld bc, 0
    ld c, (hl)
    inc hl
    ld b, (hl)
    dec hl
    inc bc
    inc bc
    ld de, ti.pixelShadow
    ldir
    pop bc
    pop de
    pop hl
    call ti.DelVarArc
    ld hl, backupAppVarName
    call ti.Mov9ToOP1
    ld.sis hl, (ti.pixelShadow and $FFFF)
    push hl
    call ti.CreateAppVar
    inc de
    inc de
    pop bc
    ld hl, ti.pixelShadow + 2
    ld a, b
    or a, c
    jr z, $ + 4
    ldir
    call _asm_utils_clrScrnAndUsedRAM + 12
    set ti.graphDraw, (iy + ti.graphFlags)
    call ti.OP4ToOP1
    ld a, (arcOnExit)
    or a, a
    ret z
    jp _asm_utils_arcUnarc
