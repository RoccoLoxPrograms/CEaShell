;--------------------------------------
;
; CEaShell Source Code - apps.asm
; By RoccoLox Programs and TIny_Hacker
; Copyright 2022
; License: GPL-3.0
;
;--------------------------------------

	assume adl=1

	section .text

include 'include/ti84pceg.inc'

    public _detectApp
    public _getAppSize
    public _getAppMinOSVersion
    public _getAppCopyrightInfo
    public _executeApp
    public _deleteApp
    public _exitDefrag

appNamePtr := ti.appData

_detectApp:
    push ix
    ld ix, 0
    add ix, sp
    ld hl, (ix + 6) ; name of app (NULL first time)
    ld de, (ix + 9) ; pointer to var to store pointer to app in
    pop ix
    push hl
    push de
    ld a, (hl)
    cp a, 0
    jr nz, .loadName

.noName:
    call ti.ZeroOP1
    ld a, ti.AppObj
    ld (ti.OP1), a
    jr .search

.loadName:
    ex de, hl
    ld hl, ti.OP1
    ld (hl), ti.AppObj
    inc hl
    ld bc, 8
    ex de, hl
    ldir ; load char * into OP1

.search:
    call ti.FindAppUp
    ld a, 0
    pop hl
    ld (hl), de
    pop hl
    ret c
    ex de, hl
    ld hl, ti.OP1 + 1
    ld bc, 9 ; get the zero at the end
    ldir ; copy in the name
    ld a, 1 ; there was an app
    ret

_getAppSize:
    push ix
    ld ix, 0
    add ix, sp
    ld hl, (ix + 6)
    pop ix
    call ti.FindAppStart
    push hl
	call ti.NextFieldFromType
	call ti.NextFieldFromType
	pop	de
	or a
	sbc	hl, de ; end of app - start of app
    inc	hl
	inc	hl
	inc	hl ; count app size bytes too
    ret

_getAppMinOSVersion:
    push ix
    ld ix, 0
    add ix, sp
    ld hl, (ix + 6)
    pop ix
    call ti.FindAppStart
    ld de, 13 ; let's hope this works
    add hl, de
    ret

_getAppCopyrightInfo:
    push ix
    ld ix, 0
    add ix, sp
    ld hl, (ix + 6)
    pop ix
    call ti.FindAppStart
    ld bc, $100
	add	hl, bc
	push hl
	pop	de
	ld bc, $24
	add	hl, bc
	ld hl, (hl)
	add	hl, de 
	or a, a 
	sbc hl, de
	ret	z
	add	hl, de
	ret

_executeApp: ; mostly Cesium's code
    ld de, ti.userMem
	ld hl, (ti.asm_prgm_size)
    ex de, hl
	call ti.DelMem
    ld hl, 0
    ld (ti.asm_prgm_size), hl
    ld iy, ti.flags
	call ti.ClrAppChangeHook
	res ti.useTokensInString, (iy + ti.clockFlags)
	res	ti.onInterrupt, (iy + ti.onFlags)
	set	ti.graphDraw, (iy + ti.graphFlags)
	call ti.ReloadAppEntryVecs
	call ti.AppSetup
	set	ti.appRunning, (iy + ti.APIFlg)
	set	6, (iy + $28)
	res	0, (iy + $2C)
	set	ti.appAllowContext, (iy + ti.APIFlg)
	ld hl, $d1787c
	ld bc, $fff
	call ti.MemClear
    ; get proper stuff for the app
    push ix
    ld ix, 0
    add ix, sp
    ld hl, (ix + 6)
    pop ix
    ld de, appNamePtr
    ex de, hl
    ld (hl), de ; save pointer
    ld sp, (ti.onSP) ; Don't bork the stack (safety first)
	call ti.ResetStacks
    ld hl, (appNamePtr)
    call ti.FindAppStart ; app pointer in hl
    ld de, $100
    add hl, de
	push hl
	ex de, hl ; code pointer in de
	ld hl, $18
	add	hl, de
	ld hl, (hl)
	add	hl, de
	or a, a
	sbc	hl, de
	jr z, .noBss
	push hl
	pop	bc
	ld hl, $15
	add	hl, de
	ld hl, (hl)
	add	hl, de
	ld de, $d1787c
	ldir

.noBss:
	pop	hl
	push hl
	pop	de
	ld bc, $1b
	add	hl, bc
	ld hl, (hl)
	add	hl, de
	jp (hl)

_deleteApp:
    push ix
    ld ix, 0
    add ix, sp
    ld hl, (ix + 6)
    pop ix
    call ti.FindAppStart
    call ti.DeleteApp
    set 3, (iy + $25) ; defrag thing?
    ret

_exitDefrag:
    bit 3, (iy + $25)
    ret z
    ld a, ti.cxErase
    jp ti.NewContext0
