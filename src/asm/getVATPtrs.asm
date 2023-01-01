;--------------------------------------
;
; CEaShell Source Code - getVATPtrs.asm
; By RoccoLox Programs and TIny_Hacker
; Copyright 2022 - 2023
; License: GPL-3.0
;
;--------------------------------------

	assume adl=1

	section .text

include 'include/ti84pceg.inc'

    public _getProgramPtrs
    public _getAppVarPtrs

arrayPtr := ti.appData

_getProgramPtrs:
	ld iy, ti.flags
	res 0, (iy + ti.asm_Flag1)
    push ix
    ld ix, 0
    add ix, sp
    ld hl, (ix + 6) ; get array
	ld a, (ix + 9) ; ignore hidden programs?
    pop ix
	ld (arrayPtr), hl
	ld hl, (ti.progPtr)
	bit 0, a
	jr z, .loop
	set 0, (iy + ti.asm_Flag1)

.loop:
	ld de, (ti.pTemp)
	or a, a
	sbc hl, de
	ret z
	ret c
	add hl, de
	ld a, (hl)
	and a, $1F
	cp a, ti.ProgObj
	jr z, .isProgram
	cp a, ti.ProtProgObj
	jr nz, .skipEntry

.isProgram:
	ld de, 7
	or a, a
	sbc hl, de
	ld a, (hl)
	add hl, de
	push hl
	call _checkHasHiddenHeader
	pop hl
	bit 0, b
	jr nz, .skipEntry
	bit 0, (iy + ti.asm_Flag1)
	jr z, .loadAddress
	cp a, 65
	jr c, .skipEntry

.loadAddress:
	ld de, (arrayPtr)
	ex de, hl
	ld (hl), de
	inc hl
	inc hl
	inc hl
	ld (arrayPtr), hl
	ex de, hl

.skipEntry:
	ld de, 6
	or a, a
	sbc hl, de
	ld e, (hl)
	inc e
	or a, a
	sbc hl, de
	jr .loop

_checkHasHiddenHeader:
	ld b, 0
	ld de, 5
	or a, a
	sbc hl, de
	ld a, (hl)
	ld (ti.scrapMem + 2), a
	ld de, (ti.scrapMem)
	inc hl
	ld d, (hl)
	inc hl
	ld e, (hl)
	call ti.ChkInRam
	jr z, .inRam
    ld hl, 9
	add hl, de
	ex de, hl
	ld hl, 0
	ld a, (de)
	ld l, a
	add hl, de
	ex de, hl
	inc de

.inRam:
	ld hl, 0
    ld a, (de)
    ld l, a
    inc de
    ld a, (de)
    ld h, a ; get program size
    inc de
	call ti.ChkHLIs0
	ret z
    dec hl
	call ti.ChkHLIs0
	ret z
    ex de, hl
    ld a, $72 ; Ans token
    cp a, (hl)
    jr z, .maybeHidden
    ld a, $AB ; rand token
    cp a, (hl)
    jr z, .maybeHidden 
    ret

.maybeHidden:
    inc hl
    ld a, $3F ; newline
    cp a, (hl)
    jr z, .isHidden
    ld a, $3E ; colon
    cp a, (hl)
    jr z, .isHidden
	ret

.isHidden:
	ld b, 1
	ret

_getAppVarPtrs:
	push ix
    ld ix, 0
    add ix, sp
    ld hl, (ix + 6) ; get array
    pop ix
	ld (arrayPtr), hl
	ld hl, (ti.progPtr)

.loop:
	ld de, (ti.pTemp)
	or a, a
	sbc hl, de
	ret z
	ret c
	add hl, de
	ld a, (hl)
	and a, $1F
	cp a, ti.AppVarObj
	jr nz, .skipEntry
	ld de, (arrayPtr)
	ex de, hl
	ld (hl), de
	inc hl
	inc hl
	inc hl
	ld (arrayPtr), hl
	ex de, hl

.skipEntry:
	ld de, 6
	or a, a
	sbc hl, de
	ld e, (hl)
	inc e
	or a, a
	sbc hl, de
	jr .loop
