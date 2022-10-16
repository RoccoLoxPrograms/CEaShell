;--------------------------------------
;
; CEaShell Source Code - programDescriptions.asm
; By RoccoLox Programs and TIny_Hacker
; Copyright 2022
; License: GPL-3.0
;
;--------------------------------------

    assume adl=1

    section .text

include 'include/ti84pceg.inc'

    public _showDescription
    public _checkPrgmType

EOF := ti.appData
description := ti.pixelShadow2

_showDescription:
    ld hl, description
    ld bc, 52
    call ti.MemClear ; zero out for our description
    ld hl, ti.progCurrent
    call ti.Mov9ToOP1
    call ti.ChkFindSym
    call _checkPrgmType ; get type and conveniently fix the data pointer for archive also
    cp a, 4 ; check for ice source
    ret z
    cp a, 2
    jr z, _basicDesc
    ; Assume it's ASM

_asmDesc:
    cp a, 0 ; check for ASM
    jr z, .skipIdentifier
    inc de

.skipIdentifier:
    ex de, hl
    inc hl
    inc hl
    ld a, (hl)
    cp a, $c3
    jp nz, _return ; no description
    ld de, 4
    add hl, de
    ld a, (hl)
    inc hl
    cp a, 1 ; check if the program has a valid sprite
    jr nz, .noIcon
    ld bc, 258
    add hl, bc
    jr .description

.noIcon:
    cp a, 2 ; check for only a description
    jp nz, _return

.description:
    xor a, a
    cp a, (hl)
    ret z
    ld de, description

.copy:
    ld a, (hl)
    ld (de), a
    inc de
    inc hl ; copy description byte to pixelShadow2
    inc bc ; restore bc
    xor a, a
    cp a, (hl)
    jr z, .continue
    push hl
    ld hl, description
    call ti.FontGetWidth
    ld hl, 290
    or a, a
    sbc hl, bc
    pop hl
    jr c, .continue
    jr .copy

.continue:
    ex de, hl
    ld (hl), 0
    jp _drawDescription

_basicDesc:
    dec de
    dec de
    ld hl, 0
    ld a, (de)
    ld l, a
    inc de
    ld a, (de)
    ld h, a ; get program size
    inc de
    call _getEOF
    ex de, hl
    ld de, description
    ld a, $3e ; colon token
    cp a, (hl)
    ret nz ; we can just return if there isn't a colon

    inc hl
    ld a, $2a
    cp a, (hl) ; check for a quotation mark
    ret nz ; return if there is no description

    inc hl

.getDesc:
    push hl
    ld hl, description
    call ti.FontGetWidth
    ld hl, 290
    or a, a
    sbc hl, bc
    pop hl
    push hl ; push things to the stack in this order
    push de
    jr c, .returnPop
    ld a, $3f
    cp a, (hl)
    jr z, .returnPop
    ld a, $2a
    cp a, (hl) ; check for a quotation mark
    jr z, .returnPop
    ld a, $04
    cp a, (hl) ; check for store arrow
    jr z, .returnPop
    push hl
    pop bc ; switch hl into bc
    call _checkEOF
    jr z, .returnPop
    push bc
    pop hl
    call ti.Get_Tok_Strng
    ld hl, ti.OP3
    ld b, c
    ld c, $ff
    pop de

.copyOP3:
    ldi
    push hl
    push bc
    ld hl, description
    call ti.FontGetWidth
    ld hl, 290
    or a, a
    sbc hl, bc
    pop bc
    pop hl
    jr c, .zeroTerminate
    djnz .copyOP3
    
    pop hl
    ld a, (hl)
    inc hl
    call ti.Isa2ByteTok
    jr nz, .getDesc
    inc hl
    jr .getDesc

.zeroTerminate:
    ex de, hl
    ld (hl), 0
    jr _drawDescription

.returnPop: ; clear stack
    ex de, hl
    ld (hl), 0
    pop hl
    pop hl

_drawDescription:
    ld hl, description
    ld de, $E71C
    ld.sis (ti.drawFGColor and $FFFF), de
    ld.sis de, (ti.statusBarBGColor and $FFFF)
    ld.sis (ti.drawBGColor and $FFFF), de
    ld a, 14
    ld (ti.penRow), a
    ld de, 2
    ld.sis (ti.penCol and $FFFF), de
    call ti.VPutS
    ld de, $FFFF
    ld.sis (ti.drawBGColor and $FFFF), de

_return:
    cp a, a
    ret

_checkPrgmType:
    call ti.ChkInRam
    jr z, .inRam ; we can skip this next bit if it's in the RAM
    ; it's in archive, so we need to change the data offset
    ld hl, 10
    add hl, de
    ld a, c
    ld bc, 0
    ld c, a
    add hl, bc
    ex de, hl

.inRam:
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
    dec de
    ld l, a
    ld bc, $EF7B ; check for plain ASM
    xor a, a
    sbc hl, bc
    ld a, 0 ; ASM identifier
    ret z ; return if it's ASM

    ld a, 2 ; BASIC identifier
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
