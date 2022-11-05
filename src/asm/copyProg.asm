;--------------------------------------
;
; CEaShell Source Code - copyProg.asm
; By RoccoLox Programs and TIny_Hacker
; Copyright 2022
; License: GPL-3.0
;
;--------------------------------------

	assume adl=1

	section .text

include 'include/ti84pceg.inc'

    public _copyProgram

_copyProgram:
    push ix
    ld ix, 0
    add ix, sp
    ld a, (ix + 12) ; get program type
    ld de, (ix + 9) ; get new name
    ld hl, (ix + 6) ; get name of prog to copy from
    pop ix
    push de
    ex de, hl
    ld hl, ti.OP1
    ld (hl), a
    inc hl
    ld bc, 8
    ex de, hl
    ldir
    push af ; preserve type
    call ti.ChkFindSym
    call ti.ChkInRam
    jr z, .inRam ; same as in getPrgmType
    ld hl, 10
    add hl, de
    ld a, c
    ld bc, 0
    ld c, a
    add hl, bc
    ex de, hl

.inRam:
    ld hl, 0
    ld a, (de)
    ld l, a
    inc de
    ld a, (de)
    ld h, a ; get program size
    inc de
    pop af
    pop bc ; pointer to new name
    push de ; data pointer of old program
    push hl
    push bc
    pop hl ; new name
    ld bc, 8
    ld de, ti.OP1 + 1
    ldir ; name in OP1
    pop hl
    push hl
    call ti.CreateVar
    pop bc ; size to copy in bc
    inc de
    inc de ; skip size bytes of new var
    pop hl

.loop:
    call ti.ChkBCIs0
    ret z
    ldi ; don't use an ldir in case the program is 0 or 1 bytes
    jr .loop