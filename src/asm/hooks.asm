;--------------------------------------
;
; CEaShell Source Code - hooks.asm
; By RoccoLox Programs and TIny_Hacker
; Copyright 2022
; License: GPL-3.0
;
;--------------------------------------

    assume adl=1

    section .text

include 'include/ti84pceg.inc'

	extern _sortVAT
    extern _showIcons
    extern _showDescription
    extern edit_basic_program
    public _installMenuHook
    public _removeMenuHook
    public _checkMenuHookInstalled
    public _installGetCSCHook
    public _removeGetCSCHook
    public _checkGetCSCHookInstalled

; CEaShell hook flags stuff
updateProgInfo := 0

_menuHookStart:
    db $83
    cp a, 3
    jr nz, .return
    ld a, (ti.menuCurrent)
    cp a, ti.mProgramHome
    jr nz, .return
    ld a, (ti.menuCurrentSub)
    cp a, ti.mPrgm_Edit
    jr nz, .return
    ld hl, ti.progCurrent
    call ti.Mov9ToOP1
    ld hl, ti.appData
    ld (hl), 0
    push hl
    ld hl, -1
    ld (hl), 2
    pop hl
    jp edit_basic_program

.return:
    cp a, a
    ret

_getCSCHookStart: ; icons in [prgm] menu
    db $83
    push bc
    cp a, $1a
    jr nz, .keyPress
    pop bc
    push af
    ld a, (ti.menuCurrent)
    cp a, 3 ; check for program menu
    jr nz, .return
    ld a, (ti.menuCurrentSub)
    cp a, ti.mPrgm_Run ; check for run menu
    jr z, .update
    cp a, ti.mPrgm_Edit
    jr nz, .returnOther

.update:
    bit updateProgInfo, (iy + ti.asm_Flag2)
    jr nz, .return
    call ti.os.ClearStatusBarLow
    ld a, (ti.menuNumItems)
    cp a, 0
    jr z, .returnOther
    call _showIcons
    call _showDescription
    set updateProgInfo, (iy + ti.asm_Flag2)
    jr .return

.keyPress: ; keypress event
    ld a, ti.skPrgm
    cp a, b
    call z, _sortVAT
    res updateProgInfo, (iy + ti.asm_Flag2) ; reset flag on keypress

.return:
    pop bc
    or a, 1
    ld a, b
    ret

.returnOther: ; draw over artifact when switching to create menu
    call ti.os.ClearStatusBarLow
	bit updateProgInfo, (iy + ti.asm_Flag2)
    jr nz, .return
    ld hl, $ffff
	ld (ti.fillRectColor), hl
	ld hl, 152
	ld de, 168
	ld b, 58
	ld c, 234
	call ti.FillRect
    set updateProgInfo, (iy + ti.asm_Flag2)
	jr .return

_installMenuHook:
    ld iy, ti.flags
    ld hl, _menuHookStart
    jp ti.SetMenuHook

_removeMenuHook:
    ld iy, ti.flags
    jp ti.ClrMenuHook

_checkMenuHookInstalled:
    ld iy, ti.flags
    bit ti.menuHookActive, (iy + ti.hookflags4) ; check if a menu hook is installed
    ld a, 0
    ret z
    ld hl, (ti.menuHookPtr)
    ld de, _menuHookStart
    or a, a
    sbc hl, de
    ret nz
    inc a
    ret

_installGetCSCHook:
    ld hl, _getCSCHookStart
    ld iy, ti.flags
    jp ti.SetGetCSCHook

_removeGetCSCHook:
    ld iy, ti.flags
    jp ti.ClrGetKeyHook

_checkGetCSCHookInstalled:
    ld iy, ti.flags
    bit ti.getCSCHookActive, (iy + ti.hookflags2) ; check if a get csc hook is installed
    ld a, 0
    ret z
    ld hl, (ti.getKeyHookPtr)
    ld de, _getCSCHookStart
    or a, a
    sbc hl, de
    ret nz
    inc a
    ret
