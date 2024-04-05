;--------------------------------------
;
; CEaShell Source Code - apps.asm
; By RoccoLox Programs and TIny_Hacker
; Copyright 2022 - 2024
; License: GPL-3.0
;
;--------------------------------------

    assume adl=1

    section .text

include 'include/equates.inc'

    public _asm_apps_getAppPtrs
    public _asm_apps_getAppName
    public _asm_apps_findAllApps
    public _asm_apps_getAppSize
    public _asm_apps_getAppMinOSVersion
    public _asm_apps_getAppCopyrightInfo
    public _asm_apps_getAppIcon
    public _asm_apps_reloadApp
    public _asm_apps_executeApp
    public _asm_apps_deleteApp
    public _asm_apps_exitDefrag

    extern _asm_utils_clrScrnAndUsedRAM
    extern _rodata_appName
    extern _exit.sp

_asm_apps_getAppPtrs:
    pop hl
    pop de
    pop bc
    push bc
    push de
    ex (sp), iy
    push hl
    call ti.ZeroOP1
    ld a, ti.AppObj
    ld (ti.OP1), a

.findApp:
    push bc
    ld a, $0D
    call ti.FindAppCustom
    pop bc
    ret c
    bit 0, c
    jr nz, .loop
    push bc
    push de
    ld b, 8
    ld hl, ti.OP1 + 1
    ld de, _rodata_appName
    call ti.StrCmpre
    pop de
    pop bc
    jr z, .findApp

.loop:
    ld (iy), de
    lea iy, iy + 3
    jr .findApp

_asm_apps_getAppName:
    pop bc
    pop de
    ex (sp), hl
    push de
    push bc
    ex de, hl
    ld bc, $103
    add hl, bc
    ld bc, 8
    ldir
    xor a, a
    ld (de), a
    ret

_asm_apps_findAllApps:
    call ti.ZeroOP1
    ld a, ti.AppObj
    ld (ti.OP1), a
    ld c, 0

.findAllLoop:
    push bc
    call ti.FindAppUp
    pop bc
    ld a, c
    ret c
    inc c
    jr .findAllLoop

_asm_apps_getAppSize:
    pop de
    ex (sp), hl
    push de
    push hl
    call ti.NextFieldFromType
    call ti.NextFieldFromType
    pop de
    or a, a
    sbc hl, de ; end of app - start of app
    inc hl
    inc hl
    inc hl ; count app size bytes too
    ret

_asm_apps_getAppMinOSVersion:
    pop de
    ex (sp), hl
    push de
    ld de, 13
    add hl, de
    ret

_asm_apps_getAppCopyrightInfo:
    pop bc
    pop de
    ex (sp), hl
    push de
    push bc
    ld (hl), 0
    ex de, hl
    push de
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
    pop bc
    ret z
    add hl, de
    push bc
    pop de
    ld bc, 52
    ldir
    xor a, a
    ld (de), a
    ret

_asm_apps_getAppIcon:
    pop bc
    pop de
    pop hl
    push hl
    push de
    push bc
    inc hl
    inc hl
    ld a, $FF
    ld bc, 256
    call ti.MemSet
    pop bc
    pop de
    ex (sp), hl
    push de
    push bc
    ex de, hl
    push de
    ld de, $100
    add hl, de
    push hl
    ld de, $12
    add hl, de
    ld hl, (hl)
    pop de
    add hl, de
    ld a, (hl)
    cp a, $01
    ld a, 0
    pop de
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

    ; call _asm_apps_reloadApp - 4 if the app needs to properly close first
    call _exit.sp + 3

_asm_apps_reloadApp:
    ld hl, _rodata_appName
    call ti.FindAppStart
    push hl
    push hl
    jr $ + 6

_asm_apps_executeApp:
    call _exit.sp + 3
    call ti.GetCSC
    or a, a
    jr nz, _asm_apps_executeApp + 4 ; debounce
    ld a, $25
    ld ($D02687), a
    xor a, a
    ld (ti.menuCurrent), a
    ld (ti.appErr1), a
    ld (ti.kbdGetKy), a
    ld de, ti.userMem
    ld hl, (ti.asm_prgm_size)
    ex de, hl
    call ti.DelMem
    or a, a
    sbc hl, hl
    ld (ti.asm_prgm_size), hl
    res ti.useTokensInString, (iy + ti.clockFlags)
    res ti.onInterrupt, (iy + ti.onFlags)
    set ti.graphDraw, (iy + ti.graphFlags)
    call ti.ReloadAppEntryVecs
    call ti.AppSetup
    set ti.appRunning, (iy + ti.APIFlg)
    set 6, (iy + $28)
    res 0, (iy + $2C)
    set ti.appAllowContext, (iy + ti.APIFlg)
    ld hl, $D1787C 
    ld bc, $FFF
    call ti.MemClear
    ; get proper stuff for the app
    call _asm_utils_clrScrnAndUsedRAM
    call ti.DrawStatusBar
    pop de
    ex (sp), hl
    push de
    ld (appPtr), hl ; save pointer
    ld sp, (ti.onSP) ; Don't bork the stack (safety first)
    call ti.ResetStacks
    ld hl, (appPtr)
    ld de, $100
    add hl, de
    push hl
    ex de, hl ; code pointer in de
    ld hl, $18
    add hl, de
    ld hl, (hl)
    add hl, de
    or a, a
    sbc hl, de
    jr z, .noBss
    push hl
    pop bc
    ld hl, $15
    add hl, de
    ld hl, (hl)
    add hl, de
    ld de, $D1787C
    ldir

.noBss:
    pop hl
    push hl
    pop de
    ld bc, $1B
    add hl, bc
    ld hl, (hl)
    add hl, de
    jp (hl)

_asm_apps_deleteApp:
    pop de
    ex (sp), hl
    push de
    call ti.DeleteApp
    set 3, (iy + $25) ; defrag thing?
    ret

_asm_apps_exitDefrag:
    ld iy, ti.flags
    bit 3, (iy + $25)
    ret z
    ld a, ti.cxErase
    jp ti.NewContext0
