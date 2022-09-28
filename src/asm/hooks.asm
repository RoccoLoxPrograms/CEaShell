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
    public _installGetCSCHook

sort_flag := $21
sort_first_item_found := 0
sort_first_hidden := 1
sort_second_hidden := 2
sort_first_item_found_ptr := ti.mpLcdCrsrImage
sort_end_of_part_ptr := ti.mpLcdCrsrImage + 3
sort_vat_entry_size := ti.mpLcdCrsrImage + 6
sort_vat_entry_new_loc := ti.mpLcdCrsrImage + 9
sort_vat_entry_temp_end := ti.mpLcdCrsrImage + 12 + 15

_getCSCHookStart:
    db $83
    push bc
    cp a, $1a
    jr nz, .sort
    pop bc
    push af
    ld a, (ti.menuCurrent)
    cp a, 3 ; check for program menu
    jr nz, .return
    ld a, (ti.menuCurrentSub)
    cp a, ti.mPrgm_Run ; check for run menu
    jr z, .icons
    cp a, ti.mPrgm_Edit
    jr nz, .returnOther

.icons:
    call _showIcons
    jr .return

.sort:
    ld a, ti.skPrgm
    cp a, b
    jr nz, .return
    call _sortVAT

.return:
    pop bc
    or a, 1
    ld a, b
    ret

.returnOther: ; draw over icons when switching menu
	ld hl, $ffff
	ld (ti.fillRectColor), hl
	ld hl, 144
	ld de, 152
	ld b, 58
	ld c, 234
	call ti.FillRect
	pop bc
	or a, 1
	ld a, b
	ret

_installGetCSCHook:
    ld hl, _getCSCHookStart
    jp ti.SetGetCSCHook
