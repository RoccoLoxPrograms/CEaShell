	assume adl=1

	section .text

include 'include/ti84pceg.inc'

	public _getIconASM
_getIconASM:
    ;push hl
    ;ld hl, -1
    ;ld (hl), 2
    ;pop hl
    push ix
    ld ix, 0
    add ix, sp
    ld bc, (ix + 12) ; get asm status
    ld a, (ix + 9) ; get type
    ld hl, (ix + 6) ; get name
    ld de, (ix + 15) ; get sprite to store into
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
    jr z, isAsm
    inc de

isAsm:
    ex de, hl
    ld de, 6
    add hl, de

    ld a, (hl)
    pop de
    cp a, 1
    ld a, 0
    ret nz
    inc hl
    inc hl
    inc hl

    inc de
    inc de

    ld bc, 256

    ldir

    ld a, 1
    ret
