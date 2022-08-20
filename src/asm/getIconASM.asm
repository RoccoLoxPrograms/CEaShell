	assume adl=1

	section .text

include 'include/ti84pceg.inc'

	public _getIconASM
_getIconASM:
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
    jr z, isAsm ; we skip an extra byte if the program is C or ICE
    inc de

isAsm:
    ex de, hl
    inc hl
    inc hl
    ld a, $c3
    cp a, (hl)
    ld a, 0
    pop de
    ret nz
    push de
    ld de, 4
    add hl, de

    ld a, (hl)
    pop de ; pointer to the gfx_sprite_t
    cp a, 1 ; check if the program has a valid sprite
    ld a, 0 ; we'll return false if the program does not have a sprite
    ret nz
    inc hl ; skip to the sprite data
    inc hl
    inc hl

    inc de ; skip height and width in the gfx_sprite_t
    inc de

    ld bc, 256 ; number of bytes to copy

    ldir

    ld a, 1 ; the program has a sprite, so return true
    ret
