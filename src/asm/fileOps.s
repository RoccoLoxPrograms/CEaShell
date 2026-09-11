;--------------------------------------
;
; CEaShell Source Code - fileOps.s
; By RoccoLox Programs and TIny_Hacker
; Copyright 2022 - 2026
; License: GPL-3.0
;
;--------------------------------------

    .assume adl=1

    include "src/asm/include/equates.inc"

    .global _asm_fileOps_getPrgmSize
    .type   _asm_fileOps_getPrgmSize, @function
    .global _asm_fileOps_getPrgmType
    .type   _asm_fileOps_getPrgmType, @function
    .global _asm_fileOps_getPrgmType.check
    .type   _asm_fileOps_getPrgmType.check, @function
    .global _asm_fileOps_getAppVarType
    .type   _asm_fileOps_getAppVarType, @function
    .global _asm_fileOps_copyFile
    .type   _asm_fileOps_copyFile, @function
    .global _asm_fileOps_renameOnGC
    .type   _asm_fileOps_renameOnGC, @function
    .global _asm_fileOps_hidePrgm
    .type   _asm_fileOps_hidePrgm, @function
    .global _asm_fileOps_lockPrgm
    .type   _asm_fileOps_lockPrgm, @function
    .global _asm_fileOps_getDescASM
    .type   _asm_fileOps_getDescASM, @function
    .global _asm_fileOps_getDescBASIC
    .type   _asm_fileOps_getDescBASIC, @function
    .global _asm_fileOps_getIconASM
    .type   _asm_fileOps_getIconASM, @function
    .global _asm_fileOps_getIconASM.varFound
    .type   _asm_fileOps_getIconASM.varFound, @function
    .global _asm_fileOps_getIconDCS
    .type   _asm_fileOps_getIconDCS, @function
    .global _asm_fileOps_getIconDCS.varFound
    .type   _asm_fileOps_getIconDCS.varFound, @function
    .global _asm_fileOps_getLaunchKey
    .type   _asm_fileOps_getLaunchKey, @function

    .extern _asm_apps_reloadApp
    .extern _asm_runProgram_error
    .extern _asm_utils_checkEOF
    .extern _asm_utils_getEOF
    .extern _asm_utils_loadNameOP1
    .extern _asm_utils_findVarPtr
    .extern _asm_utils_findVar
    .extern _asm_utils_clrScrnAndUsedRAM
    .extern _rodata_celticAppVarHeader
    .extern _rodata_osColorToXlibC
    .extern _ti_RenameVar

;--------------------------------------

    .section .text

_asm_fileOps_getPrgmSize:
    pop de
    ex (sp), hl
    push de
    call _asm_utils_findVarPtr
    or a, a
    sbc hl, hl
    ld a, (de)
    ld l, a
    inc de
    ld a, (de)
    ld h, a
    ld a, c
    add a, 9
    ld bc, 0
    ld c, a
    add hl, bc
    ret

_asm_fileOps_getPrgmType:
    pop de
    ex (sp), hl
    push de
    call _asm_utils_findVarPtr

_asm_fileOps_getPrgmType.check:
    inc de
    inc de
    ld a, (de)
    cp a, ti.tii
    ld a, typeSrc
    ret z
    ld a, (de)
    cp a, ti.tExtTok
    ld a, typeBasic
    ret nz
    inc de
    ld a, (de)
    cp a, ti.tAsm84CeCmp
    ld a, typeBasic
    ret nz
    inc de
    ld a, (de)
    ld c, a
    or a, a ; check for C
    ld a, typeC
    ret z
    ld a, c
    cp a, $7F ; check for ICE
    ld a, typeICE
    ret z
    xor a, a ; plain ASM indicator
    ret

_asm_fileOps_getAppVarType:
    pop de
    ex (sp), hl
    push de
    call _asm_utils_findVarPtr
    or a, a
    sbc hl, hl
    ld a, (de)
    ld l, a
    inc de
    ld a, (de)
    ld h, a
    inc de
    ld bc, 5
    or a, a
    sbc hl, bc
    jr c, getAppVarType.notCeltic
    ex de, hl
    ld de, _rodata_celticAppVarHeader
    ld b, 5
    call ti.StrCmpre
    jr nz, getAppVarType.notCeltic
    ld a, typeCeltic
    ret

getAppVarType.notCeltic:
    ld a, typeAppVar
    ret

_asm_fileOps_copyFile:
    xor a, a
    ld (returnLoc), a
    ld iy, 0
    add iy, sp
    ld a, (iy + 9) ; get program type
    ld hl, (iy + 3) ; get name of prog to copy from
    push af
    call _asm_utils_findVar
    ld (ti.asm_data_ptr1), de
    or a, a
    sbc hl, hl
    ld a, (de)
    ld l, a
    inc de
    ld a, (de)
    ld h, a
    pop af
    push hl
    ld hl, (iy + 6) ; get new name
    call _asm_utils_loadNameOP1
    pop hl
    push hl
    call ti.EnoughMem
    ld a, ti.E_Memory
    pop hl
    jp c, _asm_runProgram_error
    push hl
    ld a, (iy + 9)
    call ti.CreateVar
    inc de
    inc de
    ld hl, (ti.asm_data_ptr1)
    inc hl
    inc hl
    pop bc
    ld a, b
    or a, c
    ret z
    ldir
    ret

_asm_fileOps_renameOnGC:
    ld iy, ti.flags
    call ti.GetCSC
    or a, a
    jr nz, _asm_fileOps_renameOnGC + 5 ; debouncing
    pop hl ; throw away return and keep these same arguments
    ld iy, 0
    add iy, sp
    ld hl, renameOnGC.return
    call ti.PushErrorHandler
    ld hl, (iy + 6)
    push hl
    ld hl, (iy + 3)
    ld de, newName
    push de
    ld bc, 9
    ldir
    ld hl, (iy)
    ld de, oldName
    push de
    ld bc, 9
    ldir
    ld iy, ti.flags
    call _asm_utils_clrScrnAndUsedRAM
    call ti.DrawStatusBar
    call _ti_RenameVar
    call ti.PopErrorHandler

renameOnGC.return:
    jp _asm_apps_reloadApp - 4

_asm_fileOps_hidePrgm:
    ld iy, 0
    add iy, sp
    ld a, (iy + 6) ; get type
    ld hl, (iy + 3) ; get name
    call _asm_utils_loadNameOP1
    call ti.ChkFindSym
    call ti.ChkInRam
    jr z, hidePrgm.inRam
    ld iy, ti.flags
    call ti.Arc_Unarc
    jr _asm_fileOps_hidePrgm

hidePrgm.inRam:
    ld de, -7
    add hl, de
    ld a, (hl)
    xor a, 64
    ld (hl), a
    ret

_asm_fileOps_lockPrgm:
    ld iy, 0
    add iy, sp
    ld a, (iy + 6) ; get type
    ld hl, (iy + 3) ; get name
    call _asm_utils_loadNameOP1
    call ti.ChkFindSym
    call ti.ChkInRam
    jr z, lockPrgm.inRam
    ld iy, ti.flags
    call ti.Arc_Unarc
    jr _asm_fileOps_lockPrgm

lockPrgm.inRam:
    ld a, (hl)
    xor a, 3
    ld (hl), a
    ret

_asm_fileOps_getDescASM:
    ld iy, 0
    add iy, sp
    ld c, (iy + 6) ; get asm status
    ld hl, (iy + 3) ; get VAT pointer
    ld de, (iy + 9) ; get char * to store into
    push de
    push bc
    xor a, a
    ld (de), a
    call _asm_utils_findVarPtr
    inc de ; skip size bytes
    inc de
    pop bc
    ld a, c
    or a, a
    jr z, getDescASM.isAsm ; we skip an extra byte if the program is C or ICE
    inc de

getDescASM.isAsm:
    ex de, hl
    inc hl
    inc hl
    ld a, (hl)
    cp a, $C3
    pop de
    ret nz
    inc hl
    inc hl
    inc hl
    inc hl
    ld a, (hl)
    cp a, 1 ; check if the program has a valid sprite
    inc hl
    jr nz, getDescASM.noIcon
    ld bc, 258
    add hl, bc
    jr getDescASM.description

getDescASM.noIcon:
    cp a, 2 ; check for only a description
    ret nz

getDescASM.description:
    ld b, 52 ; max number of bytes to copy
    ld a, (hl)
    or a, a
    ret z

getDescASM.getDesc:
    ldi ; copy description byte to char *
    inc bc ; restore bc
    ld a, (hl)
    or a, a
    jr z, getDescASM.return
    djnz getDescASM.getDesc

getDescASM.return:
    xor a, a
    ld (de), a
    ret

_asm_fileOps_getDescBASIC:
    ld iy, 0
    add iy, sp
    ld hl, (iy + 3) ; get VAT pointer
    ld de, (iy + 6) ; get char * to store into
    ld iy, ti.flags
    push de
    xor a, a
    ld (de), a
    call _asm_utils_findVarPtr
    or a, a
    sbc hl, hl
    ld a, (de)
    ld l, a
    inc de
    ld a, (de)
    ld h, a ; get program size
    inc de
    call _asm_utils_getEOF
    ex de, hl
    pop de ; get char * pointer
    ld a, (hl)
    cp a, ti.tColon
    ret nz ; we can just return if there isn't a colon
    inc hl
    ld a, (hl)
    cp a, ti.tString
    ret nz ; return if there is no description
    inc hl
    ld a, 52
    push af

getDescBASIC.getDesc:
    pop bc
    ld a, b
    or a, a
    jr z, getDescBASIC.return
    push hl ; save things to stack in this order
    push bc
    push de
    ld a, (hl)
    cp a, ti.tEnter
    jr z, getDescBASIC.returnPop
    cp a, ti.tString
    jr z, getDescBASIC.returnPop
    cp a, ti.tStore
    jr z, getDescBASIC.returnPop
    push hl
    push hl
    pop bc ; switch hl into bc
    call _asm_utils_checkEOF
    pop hl
    jr z, getDescBASIC.returnPop
    call ti.Get_Tok_Strng
    ld hl, ti.OP3
    ld b, c
    pop de
    pop af

getDescBASIC.copyOP3:
    ldi
    dec a
    jr z, getDescBASIC.returnPop + 2
    djnz getDescBASIC.copyOP3
    pop hl
    push af
    ld a, (hl)
    inc hl
    call ti.Isa2ByteTok
    jr nz, getDescBASIC.getDesc
    inc hl
    jr getDescBASIC.getDesc

getDescBASIC.returnPop:
    pop de
    pop bc
    pop hl

getDescBASIC.return:
    xor a, a
    ld (de), a
    ret

_asm_fileOps_getIconASM:
    ld iy, 0
    add iy, sp
    ld c, (iy + 6) ; get asm status
    ld hl, (iy + 3) ; get VAT pointer
    ld de, (iy + 9) ; get sprite to store into
    push de
    push bc
    call _asm_utils_findVarPtr

_asm_fileOps_getIconASM.varFound:
    ex de, hl
    pop bc
    pop de
    push de
    push bc
    inc de
    inc de
    ex de, hl
    push de
    ld a, $FF
    ld bc, 256
    call ti.MemSet
    pop de
    inc de ; skip size bytes
    inc de
    pop bc
    xor a, a
    or a, c
    jr z, getIconASM.isAsm ; we skip an extra byte if the program is C or ICE
    inc de

getIconASM.isAsm:
    ex de, hl
    inc hl
    inc hl
    ld a, (hl)
    cp a, $C3
    pop de
    ld a, 0
    ret nz
    inc hl
    inc hl
    inc hl
    inc hl
    ld a, (hl)
    dec a
    ld a, 0
    ret nz
    inc hl ; skip to the sprite data
    inc hl
    inc hl
    inc de ; skip height and width in the gfx_sprite_t
    inc de
    ld bc, 256
    ldir
    ld a, 1
    ret

_asm_fileOps_getIconDCS:
    ld iy, 0
    add iy, sp
    ld hl, (iy + 3) ; get VAT pointer
    ld de, (iy + 6) ; get char * to store into
    push de
    call _asm_utils_findVarPtr

_asm_fileOps_getIconDCS.varFound:
    or a, a
    sbc hl, hl
    ld a, (de)
    ld l, a
    inc de
    ld a, (de)
    ld h, a ; get program size
    inc de
    call _asm_utils_getEOF
    pop hl
    push hl
    push de
    inc hl
    inc hl
    ld bc, 256
    ld a, $FF
    call ti.MemSet
    pop de
    ld a, (de)
    cp a, ti.tColon
    jr nz, getIconDCS.return
    inc de
    push de
    pop hl
    call getIconDCS.checkDCSHeader
    jr z, getIconDCS.isIcon - 1
    push hl
    pop de
    call getIconDCS.checkDCS6Header
    jp z, getIconDCS.mono16x16
    ex de, hl
    ld a, (de)
    cp a, ti.tString
    jr nz, getIconDCS.return
    push de
    pop bc

getIconDCS.loopFindLine:
    call _asm_utils_checkEOF
    jr z, getIconDCS.return
    ld a, (bc)
    inc bc
    call ti.Isa2ByteTok
    jr nz, $ + 3
    inc bc
    cp a, ti.tEnter
    jr nz, getIconDCS.loopFindLine
    push bc
    pop de
    ld a, (de)
    cp a, ti.tColon
    jr nz, getIconDCS.return
    inc de
    push de
    pop hl
    call getIconDCS.checkDCSHeader
    jr z, getIconDCS.isIcon - 1
    push hl
    pop de
    call getIconDCS.checkDCS6Header
    jr z, getIconDCS.mono16x16
    ex de, hl
    ld a, (de)
    cp a, ti.tString
    jr nz, getIconDCS.return
    push de
    pop bc
    call _asm_utils_checkEOF
    jr z, getIconDCS.return
    jr $ + 3
    dec de

getIconDCS.isIcon:
    push de
    call getIconDCS.findIconLength
    push de
    pop bc
    pop de
    inc de
    ld a, b
    or a, c
    jr z, getIconDCS.return
    ld hl, 64
    or a, a
    sbc hl, bc
    jr z, getIconDCS.mono16x16
    ld hl, 16
    or a, a
    sbc hl, bc
    jr z, getIconDCS.mono8x8
    jr getIconDCS.colorIcon

getIconDCS.return:
    pop de
    xor a, a
    ret

getIconDCS.colorIcon:
    pop hl
    ex de, hl
    inc de
    inc de
    ld b, 0

getIconDCS.loopColor:
    push hl
    push bc
    ld a, (hl)
    sub a, '0'
    cp a, ti.tH - ti.t0 + 1
    jr c, $ + 4
    ld a, 255
    ld bc, 0
    ld c, a
    ld hl, _rodata_osColorToXlibC
    add hl, bc
    ldi
    pop bc
    pop hl
    inc hl
    djnz getIconDCS.loopColor
    jr getIconDCS.returnIcon

getIconDCS.mono16x16:
    pop hl
    inc hl
    inc hl
    ld b, 64

getIconDCS.loop16x16:
    ld a, (de)
    inc de
    sub a, '0'
    cp a, 10
    jr c, $ + 4
    sub a, 7
    ld c, 107
    add a, a
    add a, a
    add a, a
    add a, a
    add a, a
    jr nc, $ + 3
    ld (hl), c
    inc hl
    add a, a
    jr nc, $ + 3
    ld (hl), c
    inc hl
    add a, a
    jr nc, $ + 3
    ld (hl), c
    inc hl
    add a, a
    jr nc, $ + 3
    ld (hl), c
    inc hl
    djnz getIconDCS.loop16x16

getIconDCS.returnIcon:
    ld a, 1
    ret

getIconDCS.mono8x8:
    pop hl
    inc hl
    inc hl
    ld b, 8
    ld a, 2

getIconDCS.loop8x8:
    push af
    ld a, (de)
    sub a, '0'
    cp a, 10
    jr c, $ + 4
    sub a, 7
    ld c, 107
    add a, a
    add a, a
    add a, a
    add a, a
    add a, a
    jr nc, $ + 3
    ld (hl), c
    inc hl
    jr nc, $ + 3
    ld (hl), c
    inc hl
    add a, a
    jr nc, $ + 3
    ld (hl), c
    inc hl
    jr nc, $ + 3
    ld (hl), c
    inc hl
    add a, a
    jr nc, $ + 3
    ld (hl), c
    inc hl
    jr nc, $ + 3
    ld (hl), c
    inc hl
    add a, a
    jr nc, $ + 3
    ld (hl), c
    inc hl
    jr nc, $ + 3
    ld (hl), c
    inc hl
    inc de
    pop af
    dec a
    jr nz, getIconDCS.loop8x8
    ex de, hl
    push hl ; pointer to program
    push bc
    push de
    pop hl
    ld bc, -16
    add hl, bc
    ld bc, 16
    ldir
    pop bc
    pop hl
    ex de, hl
    ld a, 2
    djnz getIconDCS.loop8x8
    jr getIconDCS.returnIcon

getIconDCS.checkDCSHeader:
    ld a, (de)
    cp a, ti.tD
    ret nz
    inc de
    ld a, (de)
    cp a, ti.tC
    ret nz
    inc de
    ld a, (de)
    cp a, ti.tS
    ret nz

getIconDCS.checkNewLine:
    inc de
    ld a, (de)
    cp a, ti.tEnter
    ret nz
    inc de
    ld a, (de)
    cp a, ti.tString
    inc de
    ret

getIconDCS.checkDCS6Header:
    ld a, (de)
    cp a, ti.tD
    ret nz
    inc de
    ld a, (de)
    cp a, ti.tC
    ret nz
    inc de
    ld a, (de)
    cp a, ti.tS
    ret nz
    inc de
    ld a, (de)
    cp a, ti.t6
    jr z, getIconDCS.checkNewLine
    ret

getIconDCS.findIconLength:
    push de
    pop bc
    ld de, 0
    inc bc

getIconDCS.loopIconLen:
    call _asm_utils_checkEOF
    ret z
    ld a, (bc)
    call ti.Isa2ByteTok
    jr nz, $ + 4
    inc de
    inc bc
    cp a, ti.tEnter
    ret z
    cp a, ti.tStore
    ret z
    cp a, ti.tString
    ret z
    inc bc
    inc de
    jr getIconDCS.loopIconLen

_asm_fileOps_getLaunchKey:
    pop bc
    pop de
    ex (sp), hl
    push de
    push bc
    ld c, 0

getLaunchKey.loop:
    push de
    push hl

getLaunchKey.compare:
    ld a, (de)
    or a, a
    jr nz, getLaunchKey.check
    ld a, (hl)
    or a, a
    jr z, getLaunchKey.return
    ld a, (de)

getLaunchKey.check:
    cp a, (hl)
    inc hl
    inc de
    jr z, getLaunchKey.compare
    inc c
    ld a, c
    cp a, 10

getLaunchKey.return:
    pop hl
    ld de, 10
    add hl, de
    pop de
    jr nz, getLaunchKey.loop
    ld a, c
    ret
