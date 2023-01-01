;--------------------------------------
;
; CEaShell Source Code - iconHook.asm
; By RoccoLox Programs and TIny_Hacker
; Copyright 2022 - 2023
; License: GPL-3.0
;
;--------------------------------------

    assume adl=1

    section .text

include 'include/ti84pceg.inc'

    public _showIcons

itemSelected := ti.pixelShadow2
EOF := ti.appData

_showIcons:
	ld a, 0
    ld (itemSelected), a
    ld hl, ti.userMenuSA
    ld a, (hl)
    ld (ti.OP1), a ; move type to OP1
    inc hl
    ld de, ti.OP1 + 1
    ld bc, 8
    ldir ; move name to OP1
    call ti.ChkFindSym
	ex de, hl ; move data pointer to hl
    ld a, (itemSelected)
    ld b, a
    add a, a
    add a, b ; multiply a by 3
    push de ; save vat pointer
    push bc ; name length
    push hl ; data pointer
    ld bc, 0
    ld c, a
    ld hl, iconLocations
    add hl, bc
    ld hl, (hl) ; location to start drawing in VRAM is in hl for now
	pop de ; data pointer
	pop bc
    push bc
    push hl
    call _getPrgmType
    ex de, hl
	pop de ; VRAM VRAM
    cp a, 4 ; check for Ice source
    jr z, .continue
    cp a, 2 ; check for basic
    call z, _getIconBasic
    jr z, .continue
    call _getIconASM

.continue:
	pop bc ; name length in c
	pop hl ; vat pointer in hl
    ld a, c
    ld bc, 0
    ld c, a
    or a, a
    sbc hl, bc
    ld bc, -7
    add hl, bc
    ld a, 1
    ld (itemSelected), a

.loop:
    ld a, (itemSelected)
    cp a, 9 ; are we finished drawing programs?
    jp z, .return
    ld de, (ti.pTemp)
    or a, a
    sbc hl, de
    add hl, de
    jp z, _vatEnd; we draw blank icons for the end of the VAT, this erases potential artifacts on the edit menu
    ld a, (hl)
    cp a, $05
    jr z, .isProgram
    ld a, (ti.menuCurrentSub)
    cp a, ti.mPrgm_Edit ; check if we're on the edit menu
    push hl
    jr z, .nextEntry
    ld a, (hl)
    cp a, $06
    jr nz, .nextEntry
    pop hl

.isProgram:
    ld bc, -7
    add hl, bc
    ld a, (hl) ; get first char of name
    ld bc, 7
    add hl, bc ; go back to type byte
	push hl
    cp a, 65
    jr c, .nextEntry
	pop hl
    ld bc, -6
    add hl, bc ; go to the location storing the data pointer
    ld a, (hl)
    ld c, a
    inc hl
    ld a, (hl)
    ld (ti.scrapMem + 2), a
    ld de, (ti.scrapMem)
    inc hl
    ld d, (hl)
    inc hl
    ld e, (hl) ; copy data pointer to de
    inc hl
    inc hl
    inc hl ; type byte
    push hl
    call _getPrgmType ; program type in a
    push de
	push af
    ld a, (itemSelected) ; get the vram location
    ld b, a
    add a, a
    add a, b ; multiply a by 3
	ld bc, 0
    ld c, a
    ld hl, iconLocations
    add hl, bc
    ld de, (hl) ; location to start drawing in VRAM is in de
	pop af
	pop hl ; data pointer
	cp a, 4 ; check for ice source
    jq z, .drawnIcon
    cp a, 2 ; check for basic
    call z, _getIconBasic
    jr z, .drawnIcon ; _getIconBasic will set the zero flag
    call _getIconASM ; we'll just assume it's an asm icon then

.drawnIcon:
    ld a, (itemSelected)
    inc a
    ld (itemSelected), a

.nextEntry:
	pop hl ; type byte pointer
	ld bc, -6
	add hl, bc
	ld bc, 0
	ld c, (hl) ; name length
	or a, a
	sbc hl, bc
	dec hl
	jp .loop

.return:
    cp a, a
    ret

_getPrgmType:
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

_getIconASM:
    cp a, 0
    jr z, .isAsm ; we skip an extra byte if the program is C or ICE
    inc hl

.isAsm:
    inc hl
    inc hl
    ld a, $c3
    cp a, (hl)
    jp nz, _noIcon
    ld bc, 4
    add hl, bc
    ld a, (hl)
    cp a, 1 ; check if the program has a valid sprite
    jp nz, _noIcon
    inc hl ; skip to the sprite data
    inc hl
    inc hl
    ld c, 16

.loopMain:
    push de
    ld b, 16

.loopInner:
    ld a, (hl)
    ld (de), a
    inc de
    ld (de), a
    inc hl
    inc de
    djnz .loopInner
    pop de
    dec c
    jr z, .return
    ex de, hl
    push bc
    ld bc, 640
    add hl, bc
    pop bc
    ex de, hl
    jr .loopMain

.return:
	cp a, a
	ret

_noIcon:
    ex de, hl
    ld c, 16

.loopMain:
    push hl
    ld b, 16

.loopInner:
    ld (hl), $FF
    inc hl
    ld (hl), $FF
    inc hl
    djnz .loopInner
    pop hl
    dec c
    jr z, _getIconASM.return
    push bc
    ld bc, 640
    add hl, bc
    pop bc
    jr .loopMain

_getIconBasic:
    push de
	ex de, hl
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

    ld a, $3e ; check for colon
    cp a, (hl)
    ld a, 0
    pop de
    jr nz, _noIcon
    inc hl
    ld a, $2a ; check for Cesium format (quotation mark token)
    cp a, (hl)
    jp z, _description
    ld a, $44 ; D
    cp a, (hl)
    jr nz, _noIcon
    inc hl
    ld a, $43 ; C
    cp a, (hl)
    jr nz, _noIcon
    inc hl
    ld a, $53 ; S
    cp a, (hl)
    jr nz, _noIcon
    inc hl
    push de

    ; program has DCS Icon
    ld a, $36 ; check for DCS6 icon (6 token)
    cp a, (hl)
    jp z, _mono16x16

_getIconLength:
    inc hl ; skip newline
    ld a, $2a ; check for quotation mark just to be safe
    cp a, (hl)
    pop de
    jp nz, _noIcon
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
    cp a, 64
    pop hl
    dec hl
    dec hl
    dec hl
    jp z, _mono16x16
    inc hl
    inc hl
    inc hl

_color:
    pop de
    ld c, 16

.loopMain:
    push de
    ld b, 16

.loopInner:
    ld a, (hl)
    sub a, $30
    cp a, 10
    jr c, .next
    sub a, 7

.next:
    ex de, hl
    push de
    call ti.GetColorValue
    ld (hl), e
    inc hl
    ld (hl), d
    inc hl
    ex de, hl
    pop hl
    inc hl
    djnz .loopInner
    pop de
    dec c
    jp z, _getIconASM.return
    ex de, hl
    push bc
    ld bc, 640
    add hl, bc
    pop bc
    ex de, hl
    jr .loopMain

_mono8x8:
    pop hl ; brings us back to the beginning of the program sprite data
    pop de ; VRAM VRAM
    ld b, 8 ; number of bytes to copy
    ld a, (hl) ; get token
    sub a, $30
    cp a, $10
    call nc, _subHex

.writeSprite:
    push bc
    ld b, 2

.loop: ; this is probably a bad way to do this
    push de
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
    pop de
    push hl
    ld hl, 640
    add hl, de
    ex de, hl
    pop hl
    djnz .loop

    pop bc
    inc hl
    inc hl
    ld a, (hl)
    sub a, $30
    cp a, $10
    call nc, _subHex

    djnz .writeSprite
    jp _getIconASM.return

_mono16x16:
    inc hl
    inc hl
    ld a, $2a ; check for quotation mark just to be safe
    cp a, (hl)
    ld a, 0
    pop de ; vram pointer
    jp nz, _noIcon
    inc hl
    ld c, 16

.loopMain:
    push de
    ld b, 4

.loopInner:
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
    djnz .loopInner
    pop de
    dec c
    jp z, _getIconASM.return
    ex de, hl
    push bc
    ld bc, 640
    add hl, bc
    pop bc
    ex de, hl
    jr .loopMain

_description:
    inc hl
    ld a, $3f ; check for newline
    cp a, (hl)
    jr z, .getIcon
    push hl
    pop bc ; swap hl and bc
    call _checkEOF ; check for end of file
    ld a, 0
    ret z
    push bc
    pop hl ; swap bc and hl
    jr _description

.getIcon:
    inc hl ; skip newline
    ld a, $3e ; check for second colon
    cp a, (hl)
    ld a, 0
    ret nz
    inc hl
    ld a, $2a ; check for quotation mark
    cp a, (hl)
    jp z, .next
    ld a, $44 ; D
    cp a, (hl)
    ld a, 0
    ret nz
    inc hl
    ld a, $43 ; C
    cp a, (hl)
    ld a, 0
    ret nz
    inc hl
    ld a, $53 ; S
    cp a, (hl)
    ld a, 0
    ret nz
    inc hl
    ld a, $3f ; newline
    cp a, (hl)
    ld a, 0
    ret nz
    inc hl ; skip to the quotation mark
    ld a, $2a
    cp a, (hl)
    ld a, 0
    ret nz

.next:
    push de
    dec hl ; switch back so _getIconLength works
    jp _getIconLength

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
    push af
    ld a, $FF
    jr z, .fill
    ld a, 107

.fill:
    ld (de), a
    inc de
    ld (de), a
    inc de
    ld (de), a
    inc de
    ld (de), a
    inc de
    pop af
    ret

_storeMono16x16:
    push af
    ld a, $ff
    jr z, .skip
    ld a, 107

.skip:
    ld (de), a
    inc de
    ld (de), a
    pop af
    inc de
    ret

_vatEnd:
    ld a, (itemSelected)
    cp a, 9
    jp z, _showIcons.return
    ld b, a
    add a, a
    add a, b ; multiply a by 3
	ld bc, 0
    ld c, a
    ld hl, iconLocations
    add hl, bc
    ld de, (hl) ; location to start drawing in VRAM is in de
    call _noIcon
    ld a, (itemSelected)
    inc a
    ld (itemSelected), a
    jr _vatEnd

iconLocations:
    dl $d49230
    dl $d4c430
    dl $d4f630
    dl $d52830
    dl $d55a30
    dl $d58c30
    dl $d5be30
    dl $d5f030
    dl $d62230
