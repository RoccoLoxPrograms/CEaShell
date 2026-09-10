;--------------------------------------
;
; CEaShell Source Code - prgmMenuHook.asm
; By RoccoLox Programs and TIny_Hacker
; Copyright 2022 - 2026
; License: GPL-3.0
;
;--------------------------------------

    .assume adl=1

    .include "src/asm/include/equates.inc"

    .global _asm_prgmMenuHook_showDescription
    .type   _asm_prgmMenuHook_showDescription, @function
    .global _asm_prgmMenuHook_showType
    .type   _asm_prgmMenuHook_showType, @function
    .global _asm_prgmMenuHook_icons
    .type   _asm_prgmMenuHook_icons, @function
    .global _asm_prgmMenuHook_showAppInfo
    .type   _asm_prgmMenuHook_showAppInfo, @function

    .extern _asm_apps_getAppIcon
    .extern _asm_apps_getAppSize
    .extern _asm_fileOps_getPrgmSize
    .extern _asm_fileOps_getPrgmType.check
    .extern _asm_fileOps_getIconASM.varFound
    .extern _asm_fileOps_getIconDCS.varFound
    .extern _asm_labelJumper_convertNum
    .extern _asm_utils_checkEOF
    .extern _asm_utils_getEOF
    .extern _asm_utils_findVar
    .extern _asm_utils_dispTextToolbar
    .extern _rodata_fileTypes

;--------------------------------------

    .section .text

_asm_prgmMenuHook_showDescription:
    xor a, a
    ld (description), a
    ld hl, ti.progCurrent
    call ti.Mov9ToOP1
    call prgmMenuHook_eraseRect
    call _asm_utils_findVar + 4
    push de
    call _asm_fileOps_getPrgmSize + 7
    ld de, ti.cursorImage + 32
    push de
    call _asm_labelJumper_convertNum + 8
    xor a, a
    ld (de), a
    pop hl
    ld.sis de, (ti.curCol & $FFFF)
    push de
    ld de, 15
    ld.sis (ti.curCol & $FFFF), de
    call ti.PutS
    pop hl
    ld.sis (ti.curCol & $FFFF), hl
    pop de
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
    ld hl, showDescription.clipDescription
    push hl
    ld hl, description
    or a, a
    jr z, showDescription.asmDesc ; assembly program
    dec a
    jr z, showDescription.asmDesc ; C program
    dec a
    jr z, showDescription.basicDesc ; BASIC program
    dec a
    jr nz, showDescription.basicDesc ; ICE program

showDescription.asmDesc:
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
    jr nz, showDescription.noIcon
    push hl
    ld hl, 258
    add hl, de
    ex de, hl
    pop hl
    jr showDescription.copyDescription

showDescription.noIcon:
    cp a, 2
    pop bc
    ret nz
    push bc

showDescription.copyDescription:
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
    jr showDescription.copyDescription

showDescription.basicDesc:
    ld a, (de)
    cp a, ti.tColon
    ret nz
    inc de
    ld a, (de)
    cp a, ti.tString
    ret nz
    inc de

showDescription.copyLoop:
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

showDescription.copyToken:
    ldi
    djnz showDescription.copyToken
    ex de, hl
    pop de
    inc de
    jr showDescription.copyLoop

showDescription.clipDescription:
    ld de, 0
    ld (hl), de
    push hl
    ld hl, description
    call ti.FontGetWidth
    ld hl, 300 ; max description length (in pixels)
    or a, a
    sbc hl, bc
    pop hl
    jr nc, showDescription.drawDescription
    dec hl
    ld de, $CE
    jr showDescription.clipDescription + 4

showDescription.drawDescription:
    ld hl, description
    jp _asm_utils_dispTextToolbar

_asm_prgmMenuHook_showType:
    ld hl, ti.progCurrent
    call ti.Mov9ToOP1
    call _asm_utils_findVar + 4
    call _asm_fileOps_getPrgmType.check
    ld e, a
    ld d, 11
    mlt de
    ld hl, _rodata_fileTypes
    add hl, de
    jp _asm_utils_dispTextToolbar

_asm_prgmMenuHook_icons:
    ld hl, icons.finishDrawing
    push hl
    xor a, a
    ld (itemSelected), a
    ld hl, ti.userMenuSA
    call ti.Mov9ToOP1
    call ti.ChkFindSym

icons.loopVAT:
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
    jr c, icons.vatEnd
    ld a, (hl)
    and a, $1F
    cp a, ti.ProgObj
    jr z, icons.isProgram
    cp a, ti.ProtProgObj
    jr nz, icons.vatEnd
    ld a, (ti.menuCurrentSub)
    cp a, ti.mPrgm_Run
    jr nz, icons.vatEnd

icons.isProgram:
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
    jr z, icons.inRam
    ld hl, 10
    add hl, de
    ld a, c
    ld bc, 0
    ld c, a
    add hl, bc
    ex de, hl

icons.inRam:
    ld a, (itemSelected)
    inc a
    ld (itemSelected), a
    push de
    call _asm_fileOps_getPrgmType.check
    pop de
    ld bc, icons.iconReturn
    push bc
    ld bc, iconData - 2 ; subtract 2 since the routines add 2 to the address passed
    push bc
    or a, a ; typeASM
    jr z, icons.asmIcon
    cp a, typeC
    jr z, icons.asmIcon
    cp a, typeBasic
    jr z, icons.basicIcon
    cp a, typeICE
    jr nz, icons.basicIcon

icons.asmIcon:
    ld c, a
    push bc
    jp _asm_fileOps_getIconASM.varFound

icons.basicIcon:
    jp _asm_fileOps_getIconDCS.varFound

icons.iconReturn:
    call prgmMenuHook_drawIcon

icons.vatEnd:
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
    jp icons.loopVAT

icons.finishDrawing:
    ld a, (itemSelected)
    cp a, 9
    ret z
    ld a, $FF
    ld hl, iconData
    ld bc, 256
    call ti.MemSet

icons.blankIconsLoop:
    ld a, (itemSelected)
    inc a
    ld (itemSelected), a
    call prgmMenuHook_drawIcon
    ld a, (itemSelected)
    cp a, 9
    ret z
    jr icons.blankIconsLoop

_asm_prgmMenuHook_showAppInfo:
    ld a, 1
    ld (itemSelected), a
    ld hl, ti.userMenuSA
    push hl
    call ti.Mov9ToOP1
    pop hl
    inc hl
    call ti.FindAppStart
    ex de, hl
    jr nc, showAppInfo.drawIcon

showAppInfo.loopApps:
    ld a, $0D
    call ti.FindAppCustom
    jr c, showAppInfo.foundAll
    ld a, (itemSelected)
    inc a
    ld (itemSelected), a
    cp a, 10
    jr z, showAppInfo.drawCopyright

showAppInfo.drawIcon:
    ld bc, iconData - 2
    push bc
    push de
    call _asm_apps_getAppIcon
    pop hl
    pop hl
    call prgmMenuHook_drawIcon
    jr showAppInfo.loopApps

showAppInfo.foundAll:
    ld hl, showAppInfo.drawCopyright
    push hl
    jr icons.finishDrawing

showAppInfo.drawCopyright:
    call prgmMenuHook_eraseRect
    xor a, a
    ld (ti.progCurrent + 9), a ; zero terminate name just in case
    ld hl, ti.progCurrent + 1
    call ti.FindAppStart
    jr c, showAppInfo.return
    push hl
    call _asm_apps_getAppSize + 3
    ld de, ti.cursorImage + 32
    push de
    call _asm_labelJumper_convertNum
    xor a, a
    ld (de), a
    pop hl
    ld.sis de, (ti.curCol & $FFFF)
    push de
    ld de, 15
    ld.sis (ti.curCol & $FFFF), de
    call ti.PutS
    pop hl
    ld.sis (ti.curCol & $FFFF), hl
    pop de
    push de
    pop hl
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
    jr z, showAppInfo.return
    add hl, de
    call _asm_utils_dispTextToolbar

showAppInfo.return:
    set updateProgInfo, (iy + shellFlags)
    ret

prgmMenuHook_eraseRect:
    ld hl, ti.vRam + (182 * 59) * 2
    ld b, 200

eraseRect.loop:
    ld c, b
    ld b, 74

eraseRect.erase:
    ld (hl), $FF
    inc hl
    ld (hl), $FF
    inc hl
    djnz eraseRect.erase
    ld b, c
    ld de, (ti.lcdWidth - 74) * 2
    add hl, de
    djnz eraseRect.loop
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

drawIcon.loopVert:
    ld b, 16
    push hl

drawIcon.loopHoriz:
    ld a, (de)
    ld (hl), a
    inc hl
    ld (hl), a
    inc hl
    inc de
    djnz drawIcon.loopHoriz
    pop hl
    dec c
    ret z
    push de
    ld de, ti.lcdWidth * 2
    add hl, de
    pop de
    jr drawIcon.loopVert

prgmMenuHook_iconLocations:
    .d24 $D49230
    .d24 $D4C430
    .d24 $D4F630
    .d24 $D52830
    .d24 $D55A30
    .d24 $D58C30
    .d24 $D5BE30
    .d24 $D5F030
    .d24 $D62230
