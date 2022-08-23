	assume adl=1

	section .text

include 'include/ti84pceg.inc'

	public _getIconDCS

EOF := ti.appData

_getIconDCS:
    push ix
    ld ix, 0
    add ix, sp
    ld a, (ix + 9) ; get type
    ld hl, (ix + 6) ; get name
    ld de, (ix + 12) ; get char * to store into
    pop ix
    push de
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
    ld hl, 0
    ld a, (de)
    ld l, a
    inc de
    ld a, (de)
    ld h, a ; get program size
    inc de
    call _getEOF
    ex de, hl

    ld a, $3e ; check for colon
    cp a, (hl)
    ld a, 0
    pop de
    ret nz
    inc hl
    ld a, $44
    cp a, (hl)
    ld a, 0
    ret nz
    inc hl
    ld a, $43
    cp a, (hl)
    ld a, 0
    ret nz
    inc hl
    ld a, $53
    ld a, 0
    ret nz
    inc hl
    push de

    ; program has DCS Icon
    ld a, $36 ; check for DCS6 icon
    cp a, (hl)
    jp z, _mono16x16

_getIconLength:
    inc hl ; skip newline
    ld a, $2a ; check for quotation mark just to be safe
    cp a, (hl)
    ld a, 0
    pop de
    ret nz
    push de

    inc hl ; first byte of icon
    push hl
    xor a, a
    push af

.loop:
    ld a, $2a ; check for end quotation mark
    cp a, (hl)
    jr z, getDCSType
    ld a, $3f ; check for newline
    cp a, (hl)
    jr z, getDCSType
    push hl
    pop bc
    call _checkEOF
    push bc
    pop hl
    jr z, getDCSType

    pop af
    inc a
    push af
    inc hl
    jr .loop

getDCSType:
    pop af
    cp a, 16
    jr z, _mono8x8
    ld de, 256
    or a, a
    pop hl

_color:
    ld b, 0 ; underflows to 255 (256 bytes)
    pop de
    inc de
    inc de

.loop:
    ld a, (hl)
    sub a, $30
    push bc
    ld bc, osColorToXlibC
    push hl
    ld hl, 0
    ld l, a
    add hl, bc ; get xlibc color address
    ld a, (hl)
    ld (de), a ; load color into gfx_sprite_t *
    pop hl
    pop bc
    inc hl
    inc de
    djnz .loop
    jq return

_mono8x8:
    pop hl ; brings us back to the beginning of the program sprite data
    pop de ; gfx_sprite_t *
    inc de
    inc de
    push de
    push hl
    ex de, hl ; fill the sprite with white
    ld bc, 256
    ld a, $ff ; white color
    call ti.MemSet
    pop hl
    pop de
    ld b, 8 ; number of bytes to copy
    ld a, (hl) ; get token
    sub a, $30
    cp a, $10
    call nc, _subHex

.writeSprite:
    push bc
    ld b, 2

.loop: ; this is probably a bad way to do this
    bit 3, a
    call _storeMono
    bit 2, a
    call _storeMono
    bit 1, a
    call _storeMono
    bit 0, a
    call _storeMono
    inc hl
    ld a, (hl) ; get next token
    sub a, $30
    cp a, $10
    call nc, _subHex
    bit 3, a
    call _storeMono
    bit 2, a
    call _storeMono
    bit 1, a
    call _storeMono
    bit 0, a
    call _storeMono
    dec hl
    ld a, (hl)
    sub a, $30
    cp a, $10
    call nc, _subHex
    djnz .loop

    pop bc
    inc hl
    inc hl
    ld a, (hl)
    sub a, $30
    cp a, $10
    call nc, _subHex

    djnz .writeSprite
    jq return

_mono16x16:
    inc hl
    inc hl ; skip the newline
    ld a, $2a ; check for quotation mark just to be safe
    cp a, (hl)
    ld a, 0
    pop de ; gfx_sprite_t *
    ret nz
    inc hl
    inc de
    inc de
    push de
    push hl
    ex de, hl ; fill the sprite with white
    ld bc, 256
    ld a, $ff ; white color
    call ti.MemSet
    pop hl
    pop de
    ld b, 64

.loop: ; this is probably a bad way to do this
    ld a, (hl)
    sub a, $30
    cp a, $10
    call nc, _subHex
    bit 3, a
    call _storeMono16x16
    bit 2, a
    call _storeMono16x16
    bit 1, a
    call _storeMono16x16
    bit 0, a
    call _storeMono16x16
    inc hl
    djnz .loop

return:
    ld a, 1
    ret

_checkEOF: ; bc = current address being read; destroys hl and a
    ld hl, (EOF)
    inc hl
    or a, a
    sbc hl, bc
    ret nc
    dec de
    cp a, a
    ret

_getEOF: ; args: hl = size of var; de = start of variable; preserves both registers
    push hl
    dec hl
    add hl, de
    ld (EOF), hl
    pop hl
    ret

_subHex: ; ensures that A is A, B is B, C is C, etc. (Since TI's tokens don't match normally)
    sub a, 7
    ret

_storeMono:
    jr z, .incDeReturn
    push af
    ld a, 107
    ld (de), a
    inc de
    ld (de), a
    inc de
    pop af
    ret

.incDeReturn:
    inc de
    inc de
    ret

_storeMono16x16:
    jr z, .skip
    push af
    ld a, 107
    ld (de), a
    pop af

.skip:
    inc de
    ret

osColorToXlibC: ; lines up with xlibc color picker
    db 255 ; white (transparent but not)
    db 25 ; blue
    db 224 ; red
    db 0 ; black
    db 249 ; magenta
    db 5 ; green
    db 227 ; orange
    db 160 ; brown
    db 16 ; navy
    db 28 ; light blue
    db 0 ; skip these tokens
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 231 ; yellow
    db 255 ; white
    db 222 ; light gray
    db 181 ; medium gray
    db 148 ; gray
    db 74 ; dark gray
