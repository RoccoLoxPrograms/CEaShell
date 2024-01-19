;--------------------------------------
;
; CEaShell Source Code - prgmMenuHook.asm
; By RoccoLox Programs and TIny_Hacker
; Copyright 2022 - 2024
; License: GPL-3.0
;
;--------------------------------------

    assume adl=1

    section .text

include 'include/equates.inc'

    public _asm_prgmMenuHook_showDescription
    public _asm_prgmMenuHook_icons
    public _asm_prgmMenuHook_showAppInfo

    extern _asm_fileOps_getPrgmType.check
    extern _asm_fileOps_getIconASM.varFound
    extern _asm_fileOps_getIconDCS.varFound
    extern _asm_utils_checkEOF
    extern _asm_utils_getEOF
    extern _asm_utils_findVar
    extern _asm_utils_dispTextToolbar
    extern _asm_apps_getAppIcon

_asm_prgmMenuHook_showDescription:
    ld hl, ti.progCurrent
    call ti.Mov9ToOP1
    call _asm_utils_findVar + 4
    push de
    call _asm_fileOps_getPrgmType.check
    pop de
    push af
    or a, a
    sbc hl, hl
    ld a, (de)
    ld l, a
    inc de
    ld a, (de)
    ld h, a
    inc de
    pop af
    call _asm_utils_getEOF
    ld hl, .clipDescription
    push hl
    ld hl, description
    or a, a
    jr z, .asmDesc
    dec a
    jr z, .asmDesc
    dec a
    jr z, .basicDesc
    dec a
    jr nz, .basicDesc

.asmDesc:
    inc de
    inc de
    ld a, (de)
    or a, a
    jr z, $ + 6
    cp a, $7F
    jr nz, $ + 3
    inc de
    ld a, (de)
    cp a, $C3
    pop bc
    ret nz
    push bc
    inc de
    inc de
    inc de
    inc de
    ld a, (de)
    inc de
    cp a, 1
    jr nz, .noIcon
    push hl
    ld hl, 258
    add hl, de
    ex de, hl
    pop hl
    jr .copyDescription

.noIcon:
    cp a, 2
    pop bc
    ret nz
    push bc

.copyDescription:
    ld a, (de)
    or a, a
    ret z
    push de
    pop bc
    push hl
    call _asm_utils_checkEOF
    pop hl
    ret z
    ld (hl), a
    inc hl
    inc de
    jr .copyDescription

.basicDesc:
    ld a, (de)
    cp a, ti.tColon
    ret nz
    inc de
    ld a, (de)
    cp a, ti.tString
    ret nz
    inc de

.copyLoop:
    push de
    pop bc
    push hl
    call _asm_utils_checkEOF
    pop hl
    ret z
    ld a, (de)
    cp a, ti.tEnter
    ret z
    cp a, ti.tStore
    ret z
    cp a, ti.tString
    ret z
    call ti.Isa2ByteTok
    jr nz, $ + 7
    inc de
    push de
    dec de
    jr $ + 3
    push de
    push hl
    ex de, hl
    call ti.Get_Tok_Strng
    ld b, c
    pop de
    ld hl, ti.OP3

.copyToken:
    ldi
    djnz .copyToken
    ex de, hl
    pop de
    inc de
    jr .copyLoop

.clipDescription:
    ld (hl), 0
    push hl
    ld hl, description
    call ti.FontGetWidth
    ld hl, 300
    or a, a
    sbc hl, bc
    pop hl
    jr nc, .drawDescription
    dec hl
    jr .clipDescription

.drawDescription:
    ld hl, description
    jp _asm_utils_dispTextToolbar

_asm_prgmMenuHook_icons:
    ld hl, .finishDrawing
    push hl
    xor a, a
    ld (itemSelected), a
    ld hl, ti.userMenuSA
    call ti.Mov9ToOP1
    call ti.ChkFindSym

.loopVAT:
    ld bc, (ti.pTemp)
    or a, a
    sbc hl, bc
    ret z
    ret c
    add hl, bc
    push hl
    ld bc, -7
    add hl, bc
    ld a, (hl)
    or a, a
    sbc hl, bc
    cp a, ti.tA
    jr c, .vatEnd
    ld a, (hl)
    and a, $1F
    cp a, ti.ProgObj
    jr z, .isProgram
    cp a, ti.ProtProgObj
    jr nz, .vatEnd
    ld a, (ti.menuCurrentSub)
    cp a, ti.mPrgm_Run
    jr nz, .vatEnd

.isProgram:
    pop hl
    push hl
    ld bc, -6
    add hl, bc
    ld c, (hl)
    inc hl
    ld a, (hl)
    ld (ti.scrapMem + 2), a
    ld de, (ti.scrapMem)
    inc hl
    ld d, (hl)
    inc hl
    ld e, (hl)
    call ti.ChkInRam
    jr z, .inRam
    ld hl, 10
    add hl, de
    ld a, c
    ld bc, 0
    ld c, a
    add hl, bc
    ex de, hl

.inRam:
    ld a, (itemSelected)
    inc a
    ld (itemSelected), a
    push de
    call _asm_fileOps_getPrgmType.check
    pop de
    ld bc, .iconReturn
    push bc
    ld bc, iconData - 2 ; subtract 2 since the routines add 2 to the address passed
    push bc
    or a, a
    jr z, .asmIcon
    cp a, 1
    jr z, .asmIcon
    cp a, 2
    jr z, .basicIcon
    cp a, 3
    jr nz, .basicIcon

.asmIcon:
    ld c, a
    push bc
    jp _asm_fileOps_getIconASM.varFound

.basicIcon:
    jp _asm_fileOps_getIconDCS.varFound

.iconReturn:
    call prgmMenuHook_drawIcon

.vatEnd:
    pop hl
    ld a, (itemSelected)
    cp a, 9
    ret z
    ld bc, -6
    add hl, bc
    ld a, (hl)
    inc a
    neg
    ld c, a
    add hl, bc
    jp .loopVAT

.finishDrawing:
    ld a, (itemSelected)
    cp a, 9
    ret z
    ld a, $FF
    ld hl, iconData
    ld bc, 256
    call ti.MemSet

.blankIconsLoop:
    ld a, (itemSelected)
    inc a
    ld (itemSelected), a
    call prgmMenuHook_drawIcon
    ld a, (itemSelected)
    cp a, 9
    ret z
    jr .blankIconsLoop

_asm_prgmMenuHook_showAppInfo:
    xor a, a
    ld (itemSelected), a
    ld hl, ti.userMenuSA
    push hl
    call ti.Mov9ToOP1
    pop hl
    inc hl
    call ti.FindAppStart
    ex de, hl
    jr nc, .drawIcon
    ld a, 1
    ld (itemSelected), a

.loopApps:
    ld a, $0D
    call ti.FindAppCustom
    jr c, .foundAll
    ld a, (itemSelected)
    inc a
    ld (itemSelected), a
    cp a, 9
    jr z, .drawCopyright

.drawIcon:
    ld bc, iconData - 2
    push bc
    push de
    call _asm_apps_getAppIcon
    pop hl
    pop hl
    call prgmMenuHook_drawIcon
    jr .loopApps

.foundAll:
    ld hl, .drawCopyright
    push hl
    jr _asm_prgmMenuHook_icons.finishDrawing

.drawCopyright:
    ld hl, ti.progCurrent + 1
    call ti.FindAppStart
    jr c, .return
    ld bc, $100
    add hl, bc
    push hl
    pop de
    ld bc, $24
    add hl, bc
    ld hl, (hl)
    add hl, de
    or a, a 
    sbc hl, de
    jr z, .return
    add hl, de
    call _asm_utils_dispTextToolbar

.return:
    set updateProgInfo, (iy + shellFlags)
    pop bc
    or a, 1
    ld a, b
    ret

prgmMenuHook_drawIcon:
    ld a, (itemSelected)
    dec a
    or a, a
    sbc hl, hl
    ld l, a
    add a, a
    add a, l
    ld l, a
    ld de, prgmMenuHook_iconLocations
    add hl, de
    ld hl, (hl)
    ld de, iconData
    ld c, 16

.loopVert:
    ld b, 16
    push hl

.loopHoriz:
    ld a, (de)
    ld (hl), a
    inc hl
    ld (hl), a
    inc hl
    inc de
    djnz .loopHoriz
    pop hl
    dec c
    ret z
    push de
    ld de, ti.lcdWidth * 2
    add hl, de
    pop de
    jr .loopVert

prgmMenuHook_iconLocations:
    dl $D49230
    dl $D4C430
    dl $D4F630
    dl $D52830
    dl $D55A30
    dl $D58C30
    dl $D5BE30
    dl $D5F030
    dl $D62230
