;--------------------------------------
;
; CEaShell Source Code - utils.asm
; By RoccoLox Programs and TIny_Hacker
; Copyright 2022 - 2023
; License: GPL-3.0
;
;--------------------------------------

    assume adl=1

    section .text

include 'include/equates.inc'

    public _asm_utils_checkEOF
    public _asm_utils_getEOF
    public _asm_utils_loadNameOP1
    public _asm_utils_findVar
    public _asm_utils_getFreeRAM
    public _asm_utils_toggleLowercase
    public _asm_utils_willNotGC
    public _asm_utils_invertPalette
    public _asm_utils_arcUnarc
    public _asm_utils_setGfxCharWidth
    public _asm_utils_getCharFromKey
    public _asm_utils_getFileName
    public _asm_utils_isFileArchived
    public _asm_utils_backupPrgmName
    public _asm_utils_lcdNormal
    public _asm_utils_clrScrnAndUsedRAM
    public _asm_utils_isNameValid
    public _asm_utils_findCEaShellAppVar
    public _asm_utils_dispTextToolbar

    extern _rodata_appVarName
    extern _rodata_characters
    extern _rodata_sizeOfCharsLUT
    extern _exit.sp

_asm_utils_checkEOF: ; bc = current address being read; destroys hl
    push bc
    push bc
    pop hl
    ld bc, (EOF)
    inc bc
    or a, a
    sbc hl, bc
    pop bc
    ret c
    cp a, a
    ret

_asm_utils_getEOF: ; args: hl = size of var; de = start of variable; preserves both registers
    push hl
    dec hl
    add hl, de
    ld (EOF), hl
    pop hl
    ret

_asm_utils_loadNameOP1: ; a is type, hl is pointer to var name
    ld de, ti.OP1
    ld (de), a ; move type to OP1
    inc de
    ld bc, 8
    ldir ; move name to OP1
    ret

_asm_utils_findVar:
    call _asm_utils_loadNameOP1
    call ti.ChkFindSym
    call ti.ChkInRam
    ret z
    push hl
    push bc
    ld hl, 10
    add hl, de
    ld a, c
    ld bc, 0
    ld c, a
    add hl, bc
    ex de, hl
    pop bc
    pop hl
    ret

_asm_utils_getFreeRAM:
    call ti.MemChk
    ld bc, (ti.asm_prgm_size)
    add hl, bc
    ret

_asm_utils_toggleLowercase:
    pop de
    ex (sp), hl
    push de
    ld iy, ti.flags
    bit 0, l
    jr z, .turnOff
    set ti.lwrCaseActive, (iy + ti.appLwrCaseFlag)
    ret

.turnOff:
    res ti.lwrCaseActive, (iy + ti.appLwrCaseFlag)
    ret

_asm_utils_willNotGC:
    ld iy, 0
    add iy, sp
    ld hl, (iy + 3) ; get name
    ld a, (iy + 6)
    call _asm_utils_loadNameOP1
    call ti.ChkFindSym
    ret c

.checkGC:
    ex de, hl
    push hl
    add hl, hl
    pop hl
    ld a, 1
    ret nc
    ld hl, (hl)
    ld a, c
    add a, 12
    ld c, a
    ld b, 0
    add.s hl, bc
    ld a, 0
    ret c
    push hl
    pop bc
    call ti.FindFreeArcSpot
    ld a, 1
    ret nz
    dec a
    ret

_asm_utils_invertPalette:
    ld hl, ti.mpLcdPalette
    ld b, 0

.loop:
    ld a, (hl)
    cpl
    ld (hl), a
    inc hl
    ld a, (hl)
    cpl
    ld (hl), a
    inc hl
    djnz .loop
    ret

_asm_utils_arcUnarc:
    call ti.ChkFindSym
    ret c
    call _asm_utils_willNotGC.checkGC

.garbageCollect:
    push af
    or a, a
    jr nz, .archiveOrUnarchive
    call ti.boot.ClearVRAM
    ld a, $2D
    ld (ti.mpLcdCtrl), a
    call ti.DrawStatusBar

.archiveOrUnarchive:
    ld hl, .errorHandler
    call ti.PushErrorHandler
    call ti.Arc_Unarc
    call ti.PopErrorHandler

.errorHandler:
    pop af
    ret

_asm_utils_setGfxCharWidth:
    ld iy, 0
    add iy, sp
    ld hl, (iy + 3)
    ld de, .gfxReturn
    push iy
    ld c, 0
    push bc
    push de
    jp (hl)

.gfxReturn:
    pop hl
    pop iy
    ld a, (iy + 6)
    push bc
    pop hl
    ld bc, 0
    ld c, a
    add hl, bc
    ld a, (iy + 9)
    ld (hl), a
    ret

_asm_utils_getCharFromKey:
    di
    ld hl, $F50200
    ld (hl), h
    xor a, a

.loop:
    cp a, (hl)
    jr nz, .loop
    ld hl, ti.mpKeyRange + ti.keyData
    ld bc, 56 shl 8

.getKeyLoop:
    ld a, b
    and a, 7
    jr nz, .sameGroup
    inc hl
    inc hl
    ld e, (hl)

.sameGroup:
    sla e
    jr nc, .loopCode
    xor a, a
    cp a, c
    jr nz, .return
    ld c, b

.loopCode:
    djnz .getKeyLoop

.return:
    pop de
    ex (sp), hl
    push de
    ld h, _rodata_sizeOfCharsLUT
    mlt hl
    ld de, _rodata_characters
    add hl, de
    ld a, c
    sub a, 10
    jr c, $ + 8
    cp a, _rodata_sizeOfCharsLUT + 1
    jr nc, $ + 4
    ld c, a
    add hl, bc
    ld a, (hl)
    ret

_asm_utils_getFileName:
    pop de
    ex (sp), hl
    push de
    ld de, -6
    add hl, de
    ld b, (hl)
    dec hl
    ld de, prgmName
    push de
    ex de, hl

.storeLoop:
    ld a, (de)
    ld (hl), a
    dec de
    inc hl
    djnz .storeLoop
    ld (hl), b
    pop hl
    ret

_asm_utils_isFileArchived:
    ld iy, 0
    add iy, sp
    ld hl, (iy + 3)
    ld a, (iy + 6)
    call _asm_utils_loadNameOP1
    call ti.ChkFindSym
    call ti.ChkInRam
    ld a, 0
    ret z
    inc a
    ret

_asm_utils_backupPrgmName:
    ld hl, ti.OP1
    ld de, backupPrgmName
    jp ti.Mov9b

_asm_utils_lcdNormal:
    ld a, (returnLoc)
    or a, a
    jr nz, .returnOS
    call ti.boot.ClearVRAM
    call _exit.sp + 3
    jr $ + 6

.returnOS:
    call ti.DrawStatusBar
    ld a, $2D
    ld (ti.mpLcdCtrl), a
    ret

_asm_utils_clrScrnAndUsedRAM:
    call ti.ForceFullScreen
    call ti.ClrScrn
    call ti.HomeUp
    ld hl, ti.pixelShadow
    ld bc, 8400 * 3
    call ti.MemClear
    call ti.ClrTxtShd
    ld hl, ti.textShadow
    ld de, ti.cmdShadow
    ld bc, $104
    ldir
    ret

_asm_utils_isNameValid:
    pop de
    ex (sp), hl
    push de
    ld a, (hl)
    cp a, ti.tA
    ld a, 0
    ret c
    push hl
    inc a
    ld hl, 9
    add hl, sp
    bit 0, (hl)
    pop hl
    ret nz
    dec hl

.checkForLowercase:
    inc hl
    ld a, (hl)
    or a, a
    jr z, .validName
    cp a, ti.t0
    jr c, .invalidName
    cp a, ti.tTheta + 1
    jr nc, .invalidName
    cp a, ti.t9 + 1
    jr c, .checkForLowercase
    cp a, ti.tA
    jr nc, .checkForLowercase

.invalidName:
    xor a, a
    ret

.validName:
    ld a, 1
    ret

_asm_utils_findCEaShellAppVar:
    ld hl, _rodata_appVarName
    call ti.Mov9ToOP1
    call ti.ChkFindSym
    call ti.ChkInRam
    ret z
    ld hl, 18
    add hl, de
    ex de, hl
    ret

_asm_utils_dispTextToolbar:
    ld de, $E71C
    ld.sis (ti.drawFGColor and $FFFF), de
    ld.sis de, (ti.statusBarBGColor and $FFFF)
    ld.sis (ti.drawBGColor and $FFFF), de
    ld a, 14
    ld (ti.penRow), a
    ld de, 2
    ld.sis (ti.penCol and $FFFF), de
    call ti.VPutS
    ld de, $FFFF
    ld.sis (ti.drawBGColor and $FFFF), de
    ret
