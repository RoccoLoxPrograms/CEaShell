;--------------------------------------
;
; CEaShell Source Code - getVATPtrs.asm
; By RoccoLox Programs and TIny_Hacker
; Copyright 2022
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
	cp a, ti.ProgObj
	jr z, .isProgram
	cp a, ti.ProtProgObj
	jr nz, .skipEntry

.isProgram:
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
