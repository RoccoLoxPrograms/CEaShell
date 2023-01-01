;--------------------------------------
;
; CEaShell Source Code - getPrgmType.asm
; By RoccoLox Programs and TIny_Hacker
; Copyright 2022 - 2023
; License: GPL-3.0
;
;--------------------------------------

	assume adl=1

	section .text

include 'include/ti84pceg.inc'

	public _getPrgmType

_getPrgmType:
    push ix
    ld ix, 0
    add ix, sp
    ld a, (ix + 9) ; get type
    ld hl, (ix + 6) ; get name
    pop ix
    ld (ti.OP1), a ; move type to OP1
    ld de, ti.OP1 + 1
    ld bc, 8
    ldir ; move name to OP1

    call ti.ChkFindSym
    call ti.ChkInRam
    jr z, inRam ; we can skip this next bit if it's in the RAM
    ; it's in archive, so we need to change the data offset
    ld hl, 10
    add hl, de
    ld a, c
    ld bc, 0
    ld c, a
    add hl, bc
    ex de, hl

inRam:
    inc de ; skip size bytes
    inc de

    ; the rest of the process is the same
    ld a, (de)
    cp a, $2c ; check if it's an ice source file
    ld a, 4 ; ice source identifier
    ret z ; return if it's ice source

    ; check for ASM
    ex de, hl
    ld de, (hl)
    ex de, hl
    xor a, a
    ld bc, $007BEF ; check for C
    sbc hl, bc
    ld a, 1 ; C identifier
    ret z ; return if it's C
    add hl, bc
    ld bc, $7F7BEF ; check for ICE
    xor a, a
    sbc hl, bc
    ld a, 3 ; ice identifier
    ret z ; return if it's ICE
    ld a, (de)
    ld hl, $00
    ld h, a
    inc de
    ld a, (de)
    ld l, a
    ld bc, $EF7B ; check for plain ASM
    xor a, a
    sbc hl, bc
    ld a, 0 ; ASM identifier
    ret z ; return if it's ASM

    ; assume it is BASIC

_checkIsHidden:
    dec de
    ex de, hl
    ld a, $72 ; Ans token
    cp a, (hl)
    jr z, .maybeHidden
    ld a, $ab ; rand token
    cp a, (hl)
    jr z, .maybeHidden
    ld a, 2 ; BASIC identifier
    ret

.maybeHidden:
    ld bc, 0
    dec hl
    dec hl
    ld c, (hl)
    inc hl
    ld b, (hl)
    inc hl
    inc hl
    ld a, $3f ; newline
    cp a, (hl)
    jr z, .isHidden
    ld a, $3e ; colon
    cp a, (hl)
    jr z, .isHidden
    dec bc
    call ti.ChkBCIs0
    jr z, .isHidden
    ld a, 2 ; BASIC identifier
    ret

.isHidden:
    ld a, 7 ; Hidden identifier
    ret
