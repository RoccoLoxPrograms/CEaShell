    assume adl=1

    section .text

include 'include/ti84pceg.inc'

    public _getDescBASIC
    
EOF := ti.appData

_getDescBASIC:
    push hl
    ld hl, -1
    ld (hl), 2
    pop hl
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
    pop de ; get char * pointer
    ld a, $3e ; colon token
    cp a, (hl)
    ld a, 0
    ret nz ; we can just return if there isn't a colon

    inc hl
    ld a, $2a
    cp a, (hl) ; check for a quotation mark
    ld a, 0
    ret nz ; return if there is no description

    inc hl
    ld a, 52
    push af

getDesc:
    pop bc
    xor a, a
    cp a, b ; check if bc is 0, meaning we've copied all 52 bytes
    jr z, return
    push hl ; save things to stack in this order
    push bc
    push de
    ld a, $3f
    cp a, (hl)
    jr z, returnPop
    push hl
    pop bc ; switch hl into bc
    call _checkEOF
    jr z, returnPop
    push bc
    pop hl
    call ti.Get_Tok_Strng
    ld hl, ti.OP3
    ld b, c
    ld c, $ff
    pop de
    
copyOP3:
    ldi
    pop af ; number of bytes uncopied
    dec a
    push af
    djnz copyOP3
    
    pop af
    pop hl
    push af
    ld a, (hl)
    inc hl
    call ti.Isa2ByteTok
    jr nz, getDesc
    inc hl
    jr getDesc

returnPop:
    pop de
    pop bc
    pop hl

return:
    ex de, hl
    ld (hl), 00 
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
    inc hl
    add hl, de
    ld (EOF), hl
    pop hl
    ret
