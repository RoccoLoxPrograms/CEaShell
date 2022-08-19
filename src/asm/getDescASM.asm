	assume adl=1

	section .text

include 'include/ti84pceg.inc'

	public _getDescASM

_getDescASM:
    push ix
    ld ix, 0
    add ix, sp
    ld bc, (ix + 12) ; get asm status
    ld a, (ix + 9) ; get type
    ld hl, (ix + 6) ; get name
    ld de, (ix + 15) ; get char * to store into
    pop ix
    push de
    push bc
    ld (ti.OP1), a ; move type to OP1
    ld de, ti.OP1 + 1
    ld bc, 8
    ldir ; move name to OP1

    call ti.ChkFindSym
    call ti.ChkInRam
    jr z, inRam ; same as in getPrgmType
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

    pop bc
    xor a, a
    cp a, c
    jr z, isAsm ; we skip an extra byte if the program is C or ICE
    inc de

isAsm:
    ex de, hl
    inc hl
    inc hl
    ld a, $c3
    cp a, (hl)
    ld a, 0
    ret nz
    ld de, 4
    add hl, de

    ld a, (hl)
    pop de ; pointer to the char *
    cp a, 1 ; check if the program has a valid sprite
    inc hl
    jr nz, noIcon

    ld bc, 258
    add hl, bc
    jr description

noIcon:
    cp a, 2 ; check for only a description
    ld a, 0
    ret nz

description:
    ld b, 52 ; max number of bytes to copy

getDesc:
    ldi ; copy description byte to char *
    inc bc ; restore bc
    xor a, a
    cp a, (hl)
    ld a, 1 ; return true since there was a description
    ret z
    djnz getDesc
    ;push hl
    ;ld hl, -1
    ;ld (hl), 2
    ;pop hl
    ret
