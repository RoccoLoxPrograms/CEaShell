;--------------------------------------
;
; CEaShell Source Code - hooks.asm
; By RoccoLox Programs and TIny_Hacker
; Copyright 2022 - 2024
; License: GPL-3.0
;
;--------------------------------------

    assume adl=1

    section .text

include 'include/equates.inc'

    public _asm_hooks_installStopHook
    public _asm_hooks_removeStopHook
    public _asm_hooks_triggerAPD
    public _asm_hooks_installHomescreenHook
    public _asm_hooks_removeHomescreenHook
    public _asm_hooks_installMenuHook
    public _asm_hooks_removeMenuHook
    public _asm_hooks_installGetCSCHook
    public _asm_hooks_removeGetCSCHook
    public _asm_hooks_installAppChangeHook
    public _asm_hooks_removeAppChangeHook
    public _asm_hooks_editorHook
    public _asm_hooks_installGetCSCHookCont
    public _asm_hooks_removeBasicKeyHook
    public _asm_hooks_installBasicKeyHook
    public _asm_hooks_basicPrgmHook

    extern _asm_editProgram_restoreAppVar
    extern _asm_editProgram_main
    extern _asm_fileSystem_sortVAT
    extern _asm_prgmMenuHook_icons
    extern _asm_prgmMenuHook_showAppInfo
    extern _asm_prgmMenuHook_showDescription
    extern _asm_runProgram_main
    extern _asm_runProgram_vectors
    extern _asm_runProgram_return.quit
    extern _asm_runProgram_returnOS.restoreHooks
    extern _asm_labelJumper_showLabels
    extern _asm_apps_reloadApp
    extern _asm_utils_arcUnarc
    extern _asm_utils_findVar
    extern _asm_utils_clrScrnAndUsedRAM
    extern _asm_utils_findCEaShellAppVar
    extern _asm_utils_deleteTempRunner
    extern _asm_spi_setupSPI
    extern _rodata_hashProg
    extern _rodata_appName

hooks_parserStopHook:
    db $83 ; hook signifier
    push af
    cp a, 2
    jr z, .maybeStop

.chain:
    ld a, (parserChainLoc)
    cp a, $7F
    jr nz, .noChain
    pop af
    ld ix, (parserChainLoc + 1)
    jp (ix)

.noChain:
    pop af
    xor a, a
    ret

.maybeStop:
    ld a, hookTokenStop ; check if stop token
    cp a, b
    jr nz, .chain

.stop:
    pop af
    xor a, a
    jp ti.JError

_asm_hooks_installStopHook:
    xor a, a
    ld (parserChainLoc), a
    bit ti.parserHookActive, (iy + ti.hookflags4)
    jr z, .noChain
    ld hl, (ti.parserHookPtr)
    ld de, hooks_parserStopHook
    or a, a
    sbc hl, de
    add hl, de
    jr z, .checkIfBadExit

.chainHooks:
    ld a, (hl)
    cp a, $83
    jr nz, .noChain
    ex de, hl
    inc de
    ld hl, parserChainLoc
    ld (hl), $7F
    inc hl
    ld (hl), de
    jr .noChain

.checkIfBadExit:
    ld hl, parserChainLoc
    ld a, (hl)
    cp a, $7F
    jr nz, .noChain
    inc hl
    ld hl, (hl)
    dec hl
    jr .chainHooks

.noChain:
    ld hl, hooks_parserStopHook
    jp ti.SetParserHook

_asm_hooks_removeStopHook:
    bit ti.parserHookActive, (iy + ti.hookflags4)
    jr z, .clearParser
    ld hl, (ti.parserHookPtr)
    ld de, hooks_parserStopHook
    or a, a
    sbc hl, de
    add hl, de
    ret nz
    ld hl, parserChainLoc
    ld a, (hl)
    cp a, $7F
    jr nz, .clearParser
    inc hl
    ld hl, (hl)
    dec hl
    xor a, a
    ld (parserChainLoc), a
    jp ti.SetParserHook

.clearParser:
    jp ti.ClrParserHook

hooks_getCSCHookStart: ; icons and on shortcuts
    db $83
    cp a, $1A
    jr z, hooks_iconHookStart + 1
    ld hl, $F0202C
    ld (hl), l
    ld l, h
    bit 0, (hl)
    jr z, hooks_iconHookStart + 1
    jp hooks_onHookStart + 1

hooks_iconHookStart: ; the user only has the icon hook enabled
    db $83
    push bc
    cp a, $1A
    jr nz, .keyPress
    pop bc
    push af
    ld a, (ti.menuCurrent)
    cp a, ti.mApps
    jr z, .update
    cp a, ti.mProgramHome
    jr nz, .return
    ld a, (ti.menuCurrentSub)
    cp a, ti.mPrgm_Run ; check for run menu
    jr z, .update
    cp a, ti.mPrgm_Edit
    jr nz, .returnOther

.update:
    bit updateProgInfo, (iy + shellFlags)
    jr nz, .return
    call ti.os.ClearStatusBarLow
    ld a, (ti.menuNumItems)
    or a, a
    jr z, .returnOther
    ld a, (ti.menuCurrent)
    cp a, ti.mApps
    jp z, _asm_prgmMenuHook_showAppInfo
    call _asm_prgmMenuHook_icons
    call _asm_prgmMenuHook_showDescription
    set updateProgInfo, (iy + shellFlags)
    jr .return

.keyPress: ; keypress event
    ld a, ti.skPrgm
    cp a, b
    jr nz, .modified
    ld a, (iy + ti.shiftFlags)
    add a, a
    add a, a ; shiftALock
    jr c, .modified
    add a, a ; shiftLwrAlpha
    jr c, .modified
    add a, a ; shiftAlpha
    jr c, .modified
    add a, a ; shift2nd
    call nc, _asm_fileSystem_sortVAT

.modified:
    res updateProgInfo, (iy + shellFlags) ; reset flag on keypress

.return:
    pop bc
    or a, 1
    ld a, b
    ret

.returnOther: ; draw over artifact when switching to create menu
    call ti.os.ClearStatusBarLow
    bit updateProgInfo, (iy + shellFlags)
    jr nz, .return
    ld hl, 152
    ld de, 168
    ld b, 58
    ld c, 234
    call ti.ClearRect
    set updateProgInfo, (iy + shellFlags)
    jr .return

hooks_onHookStart: ; the user only has the on shortcut hook enabled
    db $83
    cp a, $1A
    jr z, .return
    ld hl, $F0202C
    ld (hl), l
    ld l, h
    bit 0, (hl)
    jr z, .return

.onShortcut:
    ld a, ti.skGraph
    cp a, b
    jp z, _asm_labelJumper_showLabels
    ld a, ti.skPrgm
    cp a, b
    jp z, hooks_openShellHook
    ld a, ti.skStat
    cp a, b
    jp z, hooks_turnCalcOff
    ld a, ti.skStore
    cp a, b
    jp z, hooks_invertOn
    ld a, ti.skLn
    cp a, b
    jp z, hooks_invertOff

.return:
    or a, 1
    ld a, b
    ret

hooks_homescreenHookStart: ; handle OS programs using our code
    db $83
    cp a, 2
    jr z, $ + 4
    cp a, a
    ret
    res disableBusyIndicator, (iy + shellFlags)
    ld a, returnOS
    ld (returnLoc), a
    call _asm_utils_findCEaShellAppVar
    jr c, .notFound
    ld hl, 18 ; skip to the byte to check
    add hl, de
    ld de, (hl)
    ld (editArcProgs), de
    inc hl
    inc hl
    inc hl
    inc hl
    ld a, (hl)
    or a, a
    jr z, .continue
    set disableBusyIndicator, (iy + shellFlags)
    jr .continue

.notFound:
    ld hl, $10101
    ld (editArcProgs), hl

.continue:
    res useASMToken, (iy + shellFlags)
    ld hl, _rodata_hashProg ; prgm#
    call ti.Mov9ToOP1
    call ti.ChkFindSym
    jr c, .return
    ex de, hl
    ld bc, 0
    ld c, (hl)
    inc hl
    ld b, (hl)
    inc hl
    ld a, ti.tProg
    cp a, (hl)
    jr z, .isProg
    cp a, ti.t2ByteTok
    inc hl
    jr nz, .return
    ld a, (hl)
    cp a, ti.tasm
    jr nz, .return
    inc hl
    ld a, (hl)
    cp a, ti.tProg
    jr nz, .return
    dec bc
    dec bc
    set useASMToken, (iy + shellFlags)

.isProg:
    inc hl
    dec bc
    ex de, hl
    ld hl, ti.OP1
    ld (hl), ti.ProgObj
    inc hl
    ex de, hl

.load:
    ld a, b
    or a, c
    jr z, .loaded
    ldi
    jr .load

.loaded:
    ex de, hl
    ld (hl), 0
    call ti.ChkFindSym
    jr c, .return
    call ti.ChkInRam
    jr z, .inRam2
    ld hl, 10
    add hl, de
    ld a, c
    ld bc, 0
    ld c, a
    add hl, bc
    ex de, hl

.inRam2:
    inc de
    inc de
    ld a, (de)
    cp a, ti.tExtTok
    jr nz, .notASM
    inc de
    ld a, (de)
    cp a, ti.tAsm84CeCmp
    jr nz, .notASM
    xor a, a
    jr .run

.notASM:
    bit useASMToken, (iy + shellFlags)
    jr nz, .return
    ld a, typeBasic

.run:
    jp _asm_runProgram_main

.return:
    cp a, a
    ret

hooks_hideProgrammingOptions:
    db $83
    cp a, 4
    jr nz, .return
    ld a, $5B ; programming options menu equate
    cp a, b
    ld a, 4
    jr nz, .return
    ld b, ti.mProgramHome

.return:
    cp a, a
    ret

hooks_menuHookStart:
    db $83
    cp a, 4
    jr nz, hooks_editArchivedProgs + 1
    jr hooks_hideProgrammingOptions + 1

hooks_editArchivedProgs:
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
    call _asm_utils_findCEaShellAppVar
    jr c, .notFound
    ld hl, 14 ; skip to the byte to check
    add hl, de
    ld de, (hl)
    ld (editArcProgs), de
    jr .continue

.notFound:
    ld hl, $10101
    ld (editArcProgs), hl

.continue:
    ld a, returnOS
    ld (returnLoc), a
    xor a, a
    ld (isCelticVar), a
    sbc hl, hl
    ld (errorOffset), hl
    ld (editMode), a
    ld hl, ti.progCurrent
    call ti.Mov9ToOP1
    jp _asm_editProgram_main

.return:
    cp a, a
    ret

hooks_openShellHook: ; Launches CEaShell, not actually a hook but called in the on shortcut
    ld iy, ti.flags
    call ti.CursorOff
    call ti.RunIndicOff
    di
    ld a, (ti.menuCurrent)
    cp a, ti.kWindow
    jr nz, .notInWindow
    ld a, ti.kClear
    call ti.PullDownChk ; exit from alpha + function menus

.notInWindow:
    ld a, ti.kQuit
    call ti.PullDownChk ; exit from randInt( and related menus
    ld a, ti.kQuit
    call ti.NewContext0 ; just attempt a cleanup now
    call ti.CursorOff
    call ti.RunIndicOff
    xor a, a
    ld (ti.menuCurrent), a ; make sure we aren't on a menu
    ld hl, _rodata_appName ; execute app
    ld de, $D0082E
    push de
    ld bc, 8
    push bc
    ldir
    pop bc
    pop hl
    ld de, ti.progToEdit ; copy it here just to be safe
    ldir
    ld a, ti.kExtApps
    call ti.NewContext0
    ld a, ti.kClear
    jp ti.JForceCmd

hooks_turnCalcOff:
    di
    call ti.EnableAPD
    ld a, 1
    ld hl, ti.apdSubTimer
    ld (hl), a
    inc hl
    ld (hl), a
    set ti.apdRunning, (iy + ti.apdFlags)
    ei
    xor a, a
    inc a
    dec a
    ret

hooks_invertOff:
    ld a, $20
    jr hooks_invertOn + 2

hooks_invertOn:
    ld a, $21
    call _asm_spi_setupSPI
    ld hl, $F80818
    ld (hl), h
    ld (hl), $44
    ld (hl), a
    ld l, h
    ld (hl), $01
    xor a, a
    inc a
    dec a
    ret

_asm_hooks_installGetCSCHook:
    pop de
    ex (sp), hl
    push de

_asm_hooks_installGetCSCHookCont:
    dec l
    jr z, .both
    dec l
    jr z, .icon
    dec l
    jr z, .onShorts
    ret

.icon:
    ld hl, hooks_iconHookStart
    jr .install

.onShorts:
    ld hl, hooks_onHookStart
    jr .install

.both:
    ld hl, hooks_getCSCHookStart

.install:
    ld iy, ti.flags
    jp ti.SetGetCSCHook

_asm_hooks_removeGetCSCHook:
    ld iy, ti.flags
    bit ti.getCSCHookActive, (iy + ti.hookflags2)
    ret z
    ld de, (ti.getKeyHookPtr)
    ld hl, hooks_getCSCHookStart
    or a, a
    sbc hl, de
    jr z, .clearHook
    ld hl, hooks_iconHookStart
    or a, a
    sbc hl, de
    jr z, .clearHook
    ld hl, hooks_onHookStart
    or a, a
    sbc hl, de
    ret nz

.clearHook:
    jp ti.ClrGetKeyHook

_asm_hooks_installAppChangeHook:
    xor a, a
    ld (appChangeHookLoc), a
    bit ti.appChangeHookActive, (iy + ti.hookflags4)
    jr z, .install
    inc a
    ld (appChangeHookLoc), a
    ld hl, (ti.appChangeHookPtr)
    ld (appChangeHookLoc + 1), hl

.install:
    ex de, hl
    jp ti.SetAppChangeHook

_asm_hooks_removeAppChangeHook:
    bit ti.appChangeHookActive, (iy + ti.hookflags4)
    ret z
    ld hl, (ti.appChangeHookPtr)
    or a, a
    sbc hl, de
    ret nz
    ld a, (appChangeHookLoc)
    or a, a
    jp z, ti.ClrAppChangeHook
    ld hl, (appChangeHookLoc + 1)
    jp ti.SetAppChangeHook

_asm_hooks_editorHook:
    db $83
    ld c, a
    ld a, b
    cp a, ti.cxPrgmEdit ; only allow when editing
    ld b, a
    ret nz
    ld a, (iy + ti.monFlags)
    ld (monFlagBackup), a
    ld (backupAppChangeRet), hl
    res exitF5Menu, (iy + shellFlags)
    ld a, c
    or a, a ; "Quit Editor" in [alpha] + [f5] (Exiting an OS app context)
    jr nz, .notQuitF5
    set exitF5Menu, (iy + shellFlags)

.notQuitF5:
    cp a, $05 ; "Execute Program" in [alpha] + [f5] (Undocumented context?)
    jr nz, .notQuitExecute
    set exitF5Menu, (iy + shellFlags)

.notQuitExecute:
    cp a, ti.cxMode
    ret z
    cp a, ti.cxFormat
    ret z
    cp a, ti.cxTableSet
    ret z

.exitEditor:
    ld de, _asm_hooks_editorHook
    call _asm_hooks_removeAppChangeHook
    call ti.CursorOff
    call ti.CloseEditEqu
    ld hl, backupPrgmName
    call ti.Mov9ToOP1
    call ti.ChkFindSym
    ld a, (lockOnExit)
    or a, a
    jr z, .noLock
    ld (hl), ti.ProtProgObj

.noLock:
    ld a, (arcOnExit)
    or a, a
    jr z, .noArchive
    ld a, (isCelticVar)
    or a, a
    jr nz, .noArchive
    call _asm_utils_arcUnarc

.noArchive:
    ld a, (isCelticVar)
    or a, a
    call nz, _asm_editProgram_restoreAppVar
    ld a, (monFlagBackup)
    bit ti.monAbandon, a
    jr nz, .fromGoto
    ld a, (returnLoc)
    or a, a
    jp z, _asm_apps_reloadApp
    ld a, (editMode)
    or a, a
    jr nz, .fromGoto
    set exitF5Menu, (iy + shellFlags)

.fromGoto:
    ld a, (editArcProgs)
    bit 0, a
    call nz, _asm_hooks_installHomescreenHook
    ld a, (getCSCHooks)
    ld l, a
    call _asm_hooks_installGetCSCHookCont
    call ti.ClrTxtShd
    ld hl, ti.textShadow
    ld de, ti.cmdShadow
    ld bc, $104
    ldir
    ld a, (monFlagBackup)
    bit ti.monAbandon, a
    jr nz, .exitTurnOff
    ld a, ti.cxCmd
    call ti.NewContext0
    call ti.PPutAway
    bit exitF5Menu, (iy + shellFlags)
    jr nz, .exitF5
    or a, 1
    ld sp, (backupSP)
    ret

.exitF5:
    ld a, ti.kClear
    jp ti.JForceCmd

.exitTurnOff:
    xor a, a
    ld hl, (backupAppChangeRet)
    ret

_asm_hooks_installHomescreenHook:
    ld iy, ti.flags
    ld hl, hooks_homescreenHookStart
    jp ti.SetHomescreenHook

_asm_hooks_removeHomescreenHook:
    ld iy, ti.flags
    bit ti.homescreenHookActive, (iy + ti.hookflags2)
    ret z
    ld hl, (ti.homescreenHookPtr)
    ld de, hooks_homescreenHookStart
    or a, a
    sbc hl, de
    ret nz
    jp ti.ClrHomescreenHook

_asm_hooks_installMenuHook:
    pop de
    ex (sp), hl
    push de
    dec l
    jr z, .both
    dec l
    jr z, .hideProgrammingOptions
    dec l
    jr z, .editArchivedProgs
    ret

.editArchivedProgs:
    ld hl, hooks_editArchivedProgs
    jr .install

.hideProgrammingOptions:
    ld hl, hooks_hideProgrammingOptions
    jr .install

.both:
    ld hl, hooks_menuHookStart

.install:
    ld iy, ti.flags
    jp ti.SetMenuHook

_asm_hooks_removeMenuHook:
    ld iy, ti.flags
    bit ti.menuHookActive, (iy + ti.hookflags4)
    ret z
    ld de, (ti.menuHookPtr)
    ld hl, hooks_menuHookStart
    or a, a
    sbc hl, de
    jr z, .clearHook
    ld hl, hooks_editArchivedProgs
    or a, a
    sbc hl, de
    jr z, .clearHook
    ld hl, hooks_hideProgrammingOptions
    or a, a
    sbc hl, de
    ret nz

.clearHook:
    jp ti.ClrMenuHook

_asm_hooks_triggerAPD:
    ld iy, ti.flags
    ld de, (ti.asm_prgm_size)
    ld hl, ti.userMem
    call ti.DelMem
    or a, a
    sbc hl, hl
    ld (ti.asm_prgm_size), hl
    call _asm_utils_clrScrnAndUsedRAM + 12
    xor a, a
    ld (getcscHookLoc), a
    bit ti.getCSCHookActive, (iy + ti.hookflags2)
    jr z, .install
    inc a
    ld (getcscHookLoc), a
    ld hl, (ti.getKeyHookPtr)
    ld (getcscHookLoc + 1), hl

.install:
    ld hl, hooks_reloadHook
    call ti.SetGetCSCHook
    ld a, ti.kClear
    jp ti.JForceCmd

hooks_reloadHook:
    db $83
    call hooks_turnCalcOff
    ld a, (getcscHookLoc)
    or a, a
    jr z, .clearHook
    ld hl, (getcscHookLoc + 1)
    call ti.SetGetCSCHook
    jr $ + 6

.clearHook:
    call ti.ClrGetKeyHook
    jp hooks_openShellHook

_asm_hooks_removeBasicKeyHook:
    bit ti.rawKeyHookActive, (iy + ti.hookflags2)
    ret z
    ld hl, (ti.rawKeyHookPtr)
    ld de, hooks_basicKeyHook
    or a, a
    sbc hl, de
    ret nz
    ld a, (rawKeyHookLoc)
    or a, a
    jp z, ti.ClrGetKeyHook
    ld hl, (rawKeyHookLoc + 1)
    jp ti.SetGetKeyHook

hooks_basicKeyHook:
    db $83
    push af
    ld a, (ti.cxCurApp)
    cp a, ti.kPrgmInput
    jr nz, .returnAlt
    pop af
    push af
    cp a, ti.kQuit
    jr nz, .returnAlt

.returnCancel:
    pop af
    xor a, a
    ret

.returnAlt:
    pop af
    ld b, a
    inc b
    cp a, b
    ret

_asm_hooks_installBasicKeyHook:
    xor a, a
    ld (rawKeyHookLoc), a
    bit ti.rawKeyHookActive, (iy + ti.hookflags2)
    jr z, .install
    inc a
    ld (rawKeyHookLoc), a
    ld hl, (ti.rawKeyHookPtr)
    ld (rawKeyHookLoc + 1), hl

.install:
    ld hl, hooks_basicKeyHook
    jp ti.SetGetKeyHook

_asm_hooks_basicPrgmHook:
    db $83
    push hl
    ld hl, ti.mpLcdCtrl + 1
    bit 3, (hl)
    pop hl
    jr z, .exit
    ld e, a
    ld a, b
    cp a, ti.kError
    ld a, e
    ret nz
    cp a, ti.kQuit
    ret nz

.exit:
    push af
    push hl
    push bc
    ld de, _asm_hooks_basicPrgmHook
    call _asm_hooks_removeAppChangeHook
    call _asm_hooks_removeStopHook
    call _asm_hooks_removeBasicKeyHook
    call _asm_runProgram_returnOS.restoreHooks
    call _asm_utils_deleteTempRunner
    pop bc
    pop hl
    pop af
    ret
