;--------------------------------------
;
; CEaShell Source Code - hooks.asm
; By RoccoLox Programs and TIny_Hacker
; Copyright 2022
; License: GPL-3.0
;
;--------------------------------------

    assume adl=1

    section .text

include 'include/ti84pceg.inc'

	extern _sortVAT
    extern _showIcons
    extern _showDescription
    extern _checkPrgmType
    extern _continueRun
    extern edit_basic_program
    extern hook_show_labels
    extern _reloadApp
    public _triggerAPD
    public _installHomescreenHook
    public _removeHomescreenHook
    public _checkHomescreenHookInstalled
    public _installMenuHook
    public _removeMenuHook
    public _checkMenuHookInstalled
    public _installGetCSCHook
    public _removeGetCSCHook
    public _checkGetCSCHookInstalled
    public _isGetCSCHookInstalled
    public _installGetCSCHookCont
    public _openShellHook

; CEaShell hook flags stuff
updateProgInfo := 0
returnIsAsm := ti.appData + 9
returnEditLocked := returnIsAsm + 1
returnCEaShell := returnEditLocked + 1
tempAppVarSize := 1

_iconHookStart: ; the user only has the icon hook enabled
    db $83
    push bc
    cp a, $1a
    jr nz, .keyPress
    pop bc
    push af
    ld a, (ti.menuCurrent)
    cp a, 3 ; check for program menu
    jr nz, .return
    ld a, (ti.menuCurrentSub)
    cp a, ti.mPrgm_Run ; check for run menu
    jr z, .update
    cp a, ti.mPrgm_Edit
    jr nz, .returnOther

.update:
    bit updateProgInfo, (iy + ti.asm_Flag2)
    jr nz, .return
    call ti.os.ClearStatusBarLow
    ld a, (ti.menuNumItems)
    cp a, 0
    jr z, .returnOther
    call _showIcons
    call _showDescription
    set updateProgInfo, (iy + ti.asm_Flag2)
    jr .return

.keyPress: ; keypress event
    ld a, ti.skPrgm
    cp a, b
    call z, _sortVAT
    res updateProgInfo, (iy + ti.asm_Flag2) ; reset flag on keypress

.return:
    pop bc
    or a, 1
    ld a, b
    ret

.returnOther: ; draw over artifact when switching to create menu
    call ti.os.ClearStatusBarLow
	bit updateProgInfo, (iy + ti.asm_Flag2)
    jr nz, .return
    ld hl, $ffff
	ld (ti.fillRectColor), hl
	ld hl, 152
	ld de, 168
	ld b, 58
	ld c, 234
	call ti.FillRect
    set updateProgInfo, (iy + ti.asm_Flag2)
	jr .return

_onHookStart: ; the user only has the on shortcut hook enabled
    db $83
    push bc
    cp a, $1a
    jr z, .return
    ld	hl,$f0202c
	ld	(hl),l
	ld	l,h
	bit	0,(hl)
    jr z, .return

.onShortcut:
    ld a, ti.skGraph
    cp a, b
    pop ix
    jp z, hook_show_labels
    ld a, ti.skPrgm
    cp a, b
    jp z, _openShellHook
    ld a, ti.skStat
    cp a, b
    jp z, _apdHook
    ld a, ti.skStore
    cp a, b
    jp z, _invertOn
    ld a, ti.skLn
    cp a, b
    jp z, _invertOff
    push ix

.return:
    pop bc
    or a, 1
    ld a, b
    ret

_homescreenHookStart: ; handle OS programs using our code
    db $83
    cp a, 2
    jp nz, .return
    ld hl, appVarName
    call ti.Mov9ToOP1
    call ti.ChkFindSym
    ld a, 0
    jr c, .continue ; CEaShell appvar doesn't exits
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
    ld hl, 11 ; skip to the byte to check
    add hl, de
    ld a, (hl)

.continue:
    ld hl, returnEditLocked
    ld (hl), a
    ld hl, returnCEaShell
    ld (hl), 0 ; return to the OS
    ld hl, hashProg ; prgm#
    call ti.Mov9ToOP1
    call ti.ChkFindSym
    jr c, .return
    inc de
    inc de ; skip size
    ex de, hl
    ld a, $5f ; prgm token
    cp a, (hl)
    jr nz, .return
    dec hl
    dec hl ; get size
    ld bc, 0
    ld c, (hl)
    inc hl
    ld b, (hl)
    dec bc ; skip prgm token
    inc hl
    inc hl
    ex de, hl
    ld hl, ti.OP1
    ld (hl), $5
    inc hl
    ex de, hl
    ldir
    ex de, hl
    ld (hl), 0
    call ti.ChkFindSym
    jr c, .return
    call _checkPrgmType
    ld hl, returnIsAsm
    ld (hl), a
    ld bc, 0
    ld c, a
    push bc
    jp _continueRun

.return:
    cp a, a
    ret

_menuHookStart:
    db $83
    cp a, 3
    jr nz, .return
    ld a, (ti.kbdLGSC)
    cp a, ti.skEnter
    jr nz, .return
    ld a, (ti.menuNumItems)
    or a, a
    jr z, .return
    ld a, (ti.menuCurrent)
    cp a, ti.mProgramHome
    jr nz, .return
    ld a, (ti.menuCurrentSub)
    cp a, ti.mPrgm_Edit
    jr nz, .return
    call ti.ReleaseBuffer
    call ti.PPutAway
    ld hl, ti.progCurrent
    call ti.Mov9ToOP1
    ld hl, returnCEaShell
    ld (hl), 0
    jp edit_basic_program

.return:
    cp a, a
    ret

_getCSCHookStart: ; icons and on shortcuts
    db $83
    push bc
    cp a, $1a
    jr nz, .keyPress
    pop bc
    push af
    ld a, (ti.menuCurrent)
    cp a, 3 ; check for program menu
    jr nz, .return
    ld a, (ti.menuCurrentSub)
    cp a, ti.mPrgm_Run ; check for run menu
    jr z, .update
    cp a, ti.mPrgm_Edit
    jr nz, .returnOther

.update:
    bit updateProgInfo, (iy + ti.asm_Flag2)
    jr nz, .return
    call ti.os.ClearStatusBarLow
    ld a, (ti.menuNumItems)
    cp a, 0
    jr z, .returnOther
    call _showIcons
    call _showDescription
    set updateProgInfo, (iy + ti.asm_Flag2)
    jr .return

.keyPress: ; keypress event
    ld	hl,$f0202c
	ld	(hl),l
	ld	l,h
	bit	0,(hl)
    jr nz, .onShortcut
    ld a, ti.skPrgm
    cp a, b
    call z, _sortVAT
    res updateProgInfo, (iy + ti.asm_Flag2) ; reset flag on keypress
    jr .return

.onShortcut:
    ld a, ti.skGraph
    cp a, b
    pop ix
    jp z, hook_show_labels
    ld a, ti.skPrgm
    cp a, b
    jp z, _openShellHook
    ld a, ti.skStat
    cp a, b
    jp z, _apdHook
    ld a, ti.skStore
    cp a, b
    jp z, _invertOn
    ld a, ti.skLn
    cp a, b
    jp z, _invertOff
    push ix

.return:
    pop bc
    or a, 1
    ld a, b
    ret

.returnOther: ; draw over artifact when switching to create menu
    call ti.os.ClearStatusBarLow
	bit updateProgInfo, (iy + ti.asm_Flag2)
    jr nz, .return
    ld hl, $ffff
	ld (ti.fillRectColor), hl
	ld hl, 152
	ld de, 168
	ld b, 58
	ld c, 234
	call ti.FillRect
    set updateProgInfo, (iy + ti.asm_Flag2)
	jr .return

_openShellHook: ; From Cesium
	ld iy, ti.flags
	call ti.CursorOff
	call ti.RunIndicOff
	di
	call ti.ClrGetKeyHook
	ld a, (ti.menuCurrent)
	cp a, ti.kWindow
	jr nz, .notinwindow
	ld a, ti.kClear
	call ti.PullDownChk	; exit from alpha + function menus
.notinwindow:
	ld a, ti.kQuit
	call ti.PullDownChk	; exit from randInt( and related menus
	ld a, ti.kQuit
	call ti.NewContext0	; just attempt a cleanup now
	call ti.CursorOff
	call ti.RunIndicOff
	xor	a, a
	ld (ti.menuCurrent), a ; make sure we aren't on a menu
	ld hl, appName ; execute app
	ld de, $d0082e ; I have absolutely no idea what this is
	push de
	ld bc, 8
	push bc
	ldir
	pop	bc
	pop	hl
	ld de, ti.progToEdit ; copy it here just to be safe
	ldir
	ld a, ti.kExtApps
	call ti.NewContext0
	ld a, ti.kClear
	jp ti.JForceCmd

_apdHook: ; trigger APD (Turn off and save where you were!)
    di
	call ti.EnableAPD
	ld a, 1
	ld hl, ti.apdSubTimer
	ld (hl), a
	inc	hl
	ld (hl), a
	set	ti.apdRunning, (iy + ti.apdFlags)
    ei
    xor a, a
    inc a
    dec a
    ret

_invertOn:
    call ti.boot.InitializeHardware ; cesium code
	ld hl, $F80818
	ld (hl), h
	ld (hl), $44
	ld (hl), $21
	ld l, h
	ld (hl), $01
    xor a, a
    inc a
	ret

_invertOff:
    call ti.boot.InitializeHardware
    ld hl, $F80818
    ld (hl), h
    ld (hl), $44
    ld (hl), $20
    ld l, h
    ld (hl), $01
    xor a, a
    inc a
    ret

_installHomescreenHook:
    ld hl, _homescreenHookStart
    ld iy, ti.flags
    jp ti.SetHomescreenHook

_removeHomescreenHook:
    ld iy, ti.flags
    jp ti.ClrHomescreenHook

_checkHomescreenHookInstalled:
    ld iy, ti.flags
    bit ti.homescreenHookActive, (iy + ti.hookflags2) ; check if a menu hook is installed
    ld a, 0
    ret z
    ld hl, (ti.homescreenHookPtr)
    ld de, _homescreenHookStart
    or a, a
    sbc hl, de
    ret nz
    inc a
    ret

_installMenuHook:
    ld iy, ti.flags
    ld hl, _menuHookStart
    jp ti.SetMenuHook

_removeMenuHook:
    ld iy, ti.flags
    jp ti.ClrMenuHook

_checkMenuHookInstalled:
    ld iy, ti.flags
    bit ti.menuHookActive, (iy + ti.hookflags4) ; check if a menu hook is installed
    ld a, 0
    ret z
    ld hl, (ti.menuHookPtr)
    ld de, _menuHookStart
    or a, a
    sbc hl, de
    ret nz
    inc a
    ret

_installGetCSCHook:
    push ix
    ld ix, 0
    add ix, sp
    ld a, (ix + 6) ; which hook to install
    pop ix

_installGetCSCHookCont:
    cp a, 1
    jr z, .both
    cp a, 2
    jr z, .icon
    cp a, 3
    jr z, .onShorts
    ret

.icon:
    ld hl, _iconHookStart
    jr .install

.onShorts:
    ld hl, _onHookStart
    jr .install

.both:
    ld hl, _getCSCHookStart

.install:
    ld iy, ti.flags
    jp ti.SetGetCSCHook

_removeGetCSCHook:
    ld iy, ti.flags
    jp ti.ClrGetKeyHook

_checkGetCSCHookInstalled:
    ld iy, ti.flags
    bit ti.getCSCHookActive, (iy + ti.hookflags2) ; check if a get csc hook is installed
    ld a, 0
    ret z
    push ix
    ld ix, 0
    add ix, sp
    ld a, (ix + 6) ; which hook to install
    pop ix
    cp a, 1
    jr z, .both
    cp a, 2
    jr z, .icon
    cp a, 3
    jr z, .onShorts
    ret

.icon:
    ld de, _iconHookStart
    jr .check

.onShorts:
    ld de, _onHookStart
    jr .check

.both:
    ld de, _getCSCHookStart

.check:
    ld hl, (ti.getKeyHookPtr)
    or a, a
    sbc hl, de
    ret nz
    inc a
    ret

_isGetCSCHookInstalled: ; this is different, yes
    ld a, 1
    ld hl, (ti.getKeyHookPtr)
    ld de, _iconHookStart
    or a, a
    sbc hl, de
    ret z
    add hl, de
    ld de, _onHookStart
    or a, a
    sbc hl, de
    ret z
    add hl, de
    ld de, _getCSCHookStart
    or a, a
    sbc hl, de
    ret z
    ld a, 0
    ret

_triggerAPD:
    ld de, (ti.asm_prgm_size)
	ld hl, ti.userMem
	call ti.DelMem
	or a, a
	sbc	hl, hl
	ld (ti.asm_prgm_size), hl
    ld hl, _reloadHook
    call ti.SetGetCSCHook
    ld a, ti.kClear
    jp ti.JForceCmd

_reloadHook:
    db $83
    call _apdHook
    call ti.ClrGetKeyHook
    jp _openShellHook

hashProg:
    db $05, '#', 0

appName:
    db 'CEaShell', 0

appVarName:
    db ti.AppVarObj, 'CEaShell', 0
