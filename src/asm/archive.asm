;--------------------------------------
;
; CEaShell Source Code - archive.asm
; By RoccoLox Programs and TIny_Hacker
; Copyright 2022
; License: GPL-3.0
;
;--------------------------------------

	assume adl=1

	section .text

include 'include/ti84pceg.inc'

	public _willGarbageCollect

_willGarbageCollect:
    push ix
    ld ix, 0
    add ix, sp
    ld hl, (ix + 6) ; get name
    ld a, (ix + 9)
    pop ix
    ld (ti.OP1), a ; move type to OP1
    ld de, ti.OP1 + 1
    ld bc, 8
    ldir ; move name to OP1
    call ti.ChkFindSym
	ret	c
	ex de, hl
	push hl
	add	hl, hl
	pop	hl
	ld a, 1
    ret nc
	ld hl, (hl)
	ld a, c
	add	a, 12
	ld c, a
	ld b, 0
	add.s hl, bc
	ld a, 0
    ret c
	push hl
	pop	bc
	call ti.FindFreeArcSpot
    ld a, 1
    ret nz
    dec a
    ret
