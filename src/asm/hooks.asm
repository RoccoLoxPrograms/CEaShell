;--------------------------------------
;
; CEaShell Source Code - hooks.asm
; By RoccoLox Programs and TIny_Hacker
; Copyright 2022 - 2026
; License: GPL-3.0
;
;--------------------------------------

    assume adl=1

    section .text

include 'include/equates.inc'

    public _asm_hooks_installStopHook
    public _asm_hooks_removeStopHook
    public _asm_hooks_installGetCSCHook
    public _asm_hooks_installGetCSCHookCont
    public _asm_hooks_removeGetCSCHook
    public _asm_hooks_installAppChangeHook
    public _asm_hooks_removeAppChangeHook
    public _asm_hooks_editorHook
    public _asm_hooks_installHomescreenHook
    public _asm_hooks_removeHomescreenHook
    public _asm_hooks_installMenuHook
    public _asm_hooks_removeMenuHook
    public _asm_hooks_triggerAPD
    public _asm_hooks_removeBasicKeyHook
    public _asm_hooks_installBasicKeyHook
    public _asm_hooks_basicPrgmHook

    extern _asm_apps_reloadApp
    extern _asm_editProgram_main
    extern _asm_editProgram_restoreAppVar
    extern _asm_fileSystem_sortVAT
    extern _asm_labelJumper_showLabels
    extern _asm_prgmMenuHook_showDescription
    extern _asm_prgmMenuHook_showType
    extern _asm_prgmMenuHook_icons
    extern _asm_prgmMenuHook_showAppInfo
    extern _asm_runProgram_main
    extern _asm_runProgram_returnOS.restoreHooks
    extern _asm_spi_setupSPI
    extern _asm_utils_arcUnarc
    extern _asm_utils_clrScrnAndUsedRAM
    extern _asm_utils_findCEaShellAppVar
    extern _asm_utils_deleteTempRunner
    extern _asm_utils_cleanupForceCmd
    extern _asm_utils_checkSysVar
    extern _rodata_hashProg
    extern _rodata_appName
    extern _rodata_numberKeysLUT
    extern _exit.sp

hooks_parserStopHook:
    db $83
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
    cp a, a
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

hooks_getCSCHook:
    db $83
    push af
    ld a, (ti.cxCurApp)
    cp a, ti.cxextapps
    jr z, .return
    ld hl, (ti.catalog1HookPtr)
    call ti.ChkHLIs0
    jr z, .return
    pop af
    ld (getCSCvalA), a
    ld (getCSCvalBC), bc
    ld a, (ti.catalog1HookPtr)
    bit 0, a
    call nz, hooks_iconHook
    ld a, (ti.catalog1HookPtr)
    bit 1, a
    call nz, hooks_onHook
    ld a, (ti.catalog1HookPtr)
    bit 2, a
    call nz, hooks_fastAlphaScrolling
    ld a, (getCSCvalA)
    ld bc, (getCSCvalBC)
    push af

.return:
    pop af
    cp a, $1B
    ret nz
    or a, a
    ld a, b
    ret

hooks_iconHook:
    ld a, (ti.cxCurApp)
    cp a, ti.cxPrgmEdit
    ret z
    ld a, (getCSCvalA)
    cp a, $1A
    jr nz, .keyPress
    ld a, (ti.menuCurrent)
    cp a, ti.mApps
    jr z, .update
    cp a, ti.mProgramHome
    jr z, .continue
    bit updateProgInfo, (iy + shellFlags) ; if we just left a valid menu, clean up
    ret nz
    call ti.os.ClearStatusBarLow
    set updateProgInfo, (iy + shellFlags)
    ret

.continue:
    ld a, (ti.menuCurrentSub)
    cp a, ti.mPrgm_Run
    jr z, .update
    cp a, ti.mPrgm_Edit
    jr nz, .returnOther

.update:
    bit updateProgInfo, (iy + shellFlags)
    ret nz
    call ti.os.ClearStatusBarLow
    ld a, (ti.menuNumItems)
    or a, a
    jr z, .returnOther
    ld a, (ti.menuCurrent)
    cp a, ti.mApps
    jp z, _asm_prgmMenuHook_showAppInfo
    call _asm_prgmMenuHook_icons
    call _asm_prgmMenuHook_showDescription
    ld a, (description)
    or a, a
    call z, _asm_prgmMenuHook_showType
    set updateProgInfo, (iy + shellFlags)
    ret

.keyPress: ; keypress event
    ld bc, (getCSCvalBC)
    ld a, b
    cp a, ti.skMatrix ; apps key
    jr z, .return
    cp a, ti.skCos ; apps key on 83PCE
    jr z, .return
    cp a, ti.skPrgm
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

.return:
    res updateProgInfo, (iy + shellFlags)
    ret

.modified:
    res updateProgInfo, (iy + shellFlags)
    ld a, (ti.menuCurrent)
    cp a, ti.mApps
    ret z
    cp a, ti.mProgramHome
    ret z
    set updateProgInfo, (iy + shellFlags)
    ret

.returnOther: ; draw over artifact when switching to create menu
    call ti.os.ClearStatusBarLow
    bit updateProgInfo, (iy + shellFlags)
    ret nz
    ld hl, 152
    ld de, 252
    ld b, 58
    ld c, 234
    call ti.ClearRect
    set updateProgInfo, (iy + shellFlags)
    ret

hooks_onHook:
    ld a, (getCSCvalA)
    cp a, $1B
    ret nz
    ld hl, $F0202C
    ld (hl), l
    ld l, h
    bit 0, (hl)
    ret z
    pop hl
    ld bc, (getCSCvalBC)
    ld a, b
    cp a, ti.skGraph
    jp z, _asm_labelJumper_showLabels
    cp a, ti.skPrgm
    jp z, hooks_openShellHook
    cp a, ti.skStat
    jp z, hooks_turnCalcOff
    cp a, ti.skStore
    jp z, hooks_invertOn
    cp a, ti.skLn
    jp z, hooks_invertOff
    push hl
    ld bc, 10
    ld hl, _rodata_numberKeysLUT
    cpir
    ret nz
    ld a, 9
    sub a, c
    ld h, 10
    ld l, a
    mlt hl
    push hl
    call _asm_utils_findCEaShellAppVar
    pop hl
    ret c
    push hl
    ld hl, 24
    add hl, de
    pop de
    add hl, de
    ld a, (hl)
    or a, a
    ret z
    cp a, ti.AppObj
    jr nz, .runProgram
    inc hl
    push hl
    call ti.FindAppStart
    pop hl
    ret c
    jp hooks_launchAppHook

.runProgram:
    push hl
    call ti.Mov9ToOP1
    call ti.ChkFindSym
    pop hl
    ret c
    push hl
    push de
    push bc
    call _asm_utils_cleanupForceCmd
    call ti.BufClear
    pop bc
    pop de
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
    inc de
    inc de
    ld a, (de)
    cp a, ti.tExtTok
    jr nz, .notAsm
    inc de
    ld a, (de)
    cp a, ti.tAsm84CeCmp
    jr z, .isAsm
    cp a, ti.tAsm84CePrgm
    jr nz, .notAsm

.isAsm:
    ld de, (ti.t2ByteTok shl 8) or ti.tasm
    call ti.BufInsert
    pop hl
    ret z
    push hl

.notAsm:
    ld de, ti.tProg
    call ti.BufInsert
    pop hl
    ret z

.insertName:
    inc hl
    ld de, 0
    ld a, (hl)
    or a, a
    jr z, .doneInserting
    ld e, a
    push hl
    call ti.BufInsert
    pop hl
    jr nz, .insertName

.doneInserting:
    res ti.onInterrupt, (iy + ti.onFlags)
    ld a, ti.kEnter
    jp ti.JForceCmd

hooks_fastAlphaScrolling:
    ld a, (getCSCvalA)
    cp a, $1B
    ret nz
    ld a, (ti.cxCurApp)
    cp a, ti.cxPrgmEdit
    ret nz
    ld a, (ti.menuCurrent)
    or a, a
    ret nz
    bit ti.shiftAlpha, (iy + ti.shiftFlags)
    ret z
    xor a, a
    ld (newlineCount), a
    ld bc, (getCSCvalBC)
    ld a, b
    cp a, ti.skUp
    jr z, .popUp
    cp a, ti.skDown
    ret nz
    pop hl

.down:
    call ti.BufRight
    jr z, .returnDown
    ld a, d
    or a, a
    jr nz, .down
    ld a, e
    cp a, ti.tEnter
    jr nz, .down
    ld a, (newlineCount)
    inc a
    ld (newlineCount), a
    cp a, 7
    jr c, .down
    jr .return

.returnDown:
    call ti.BufToBtm
    jr .return

.popUp:
    pop hl

.up:
    call ti.BufLeft
    jr z, .returnUp
    ld a, d
    or a, a
    jr nz, .up
    ld a, e
    cp a, ti.tEnter
    jr nz, .up
    ld a, (newlineCount)
    inc a
    ld (newlineCount), a
    cp a, 7
    jr c, .up
    jr .return

.returnUp:
    call ti.BufToTop

.return:
    call ti.CursorOff
    xor a, a
    ld (ti.curCol), a
    ld (ti.curRow), a
    ld a, (ti.winTop)
    or a, a
    call nz, ti.NewLine
    ld a, ':'
    call ti.PutMap
    ld hl, 1
    ld.sis (ti.curCol and $FFFF), hl

.backup:
    call ti.BufLeft
    jr z, .done
    ld a, d
    or a, a
    jr nz, .backup
    ld a, e
    cp a, ti.tEnter
    jr nz, .backup
    call ti.BufRight

.done:
    call ti.DispEOW
    call ti.CursorOn
    bit ti.shiftALock, (iy + ti.shiftFlags)
    jr nz, $ + 6
    res ti.shiftAlpha, (iy + ti.shiftFlags)
    ; make sure these flags are these values, for Celtic compatibility
    res keyPressed, (iy + celticFlags2)
    set showLineNum, (iy + celticFlags1)
    call ti.os.ClearStatusBarLow
    xor a, a
    inc a
    dec a
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
    bit 0, a
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
    push hl
    ld hl, 1
    or a, a
    sbc hl, bc
    pop hl
    jr z, .return
    ld a, (hl)
    cp a, ti.tProg
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
    ld a, b
    or a, c
    jr z, .loaded
    ldir

.loaded:
    ex de, hl
    ld (hl), 0
    call _asm_utils_checkSysVar
    jr z, .return + 11
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
    dec de
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
    ld a, (de)
    cp a, ti.tExtTok
    jr nz, $ + 8
    inc de
    ld a, (de)
    cp a, ti.tAsm84CePrgm
    jr z, .notASM + 6
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
    ld a, (ti.menuCurrent)
    cp a, ti.mProgramHome
    jr nz, .return
    ld a, (ti.menuCurrentSub)
    cp a, ti.mPrgm_Edit
    jr nz, .return
    ld a, b
    cp a, ti.cxPrgmEdit
    jr nz, .return
    call _asm_utils_cleanupForceCmd + 7
    call ti.ReleaseBuffer
    call ti.PPutAway
    call _asm_utils_findCEaShellAppVar
    jr c, .notFound
    ld hl, 18 ; skip to the byte to check
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

hooks_openShellHook:
    ld hl, _rodata_appName

hooks_launchAppHook:
    push hl
    ld iy, ti.flags
    call _asm_utils_cleanupForceCmd
    call ti.RunIndicOff
    xor a, a
    ld (ti.menuCurrent), a ; make sure we aren't on a menu
    pop hl
    ld de, ti.progCurrent + 1
    push de
    ld bc, 9
    push bc
    ldir
    ld a, ti.AppObj
    ld (ti.progCurrent), a
    pop bc
    dec bc
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
    push af
    call _asm_spi_setupSPI
    pop af
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
    ld a, l

_asm_hooks_installGetCSCHookCont:
    ld iy, ti.flags
    or a, a
    ret z
    sbc hl, hl
    ld l, a
    ld (ti.catalog1HookPtr), hl
    call ti.ClrCatalog1Hook
    ld hl, hooks_getCSCHook
    jp ti.SetGetCSCHook

_asm_hooks_removeGetCSCHook:
    ld iy, ti.flags
    bit ti.getCSCHookActive, (iy + ti.hookflags2)
    ret z
    ld de, (ti.getKeyHookPtr)
    ld hl, hooks_getCSCHook
    or a, a
    sbc hl, de
    ret nz
    or a, a
    sbc hl, hl
    ld (ti.catalog1HookPtr), hl
    call ti.ClrCatalog1Hook
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
    cp a, ti.cxPrgmEdit
    ld b, a
    ld a, c
    ret nz
    cp a, ti.cxMode
    ret z
    cp a, ti.cxFormat
    ret z
    cp a, ti.cxTableSet
    ret z
    push bc
    push hl
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
    bit ti.monAbandon, (iy + ti.monFlags)
    jr nz, .returnOS
    ld a, (returnLoc)
    or a, a
    jp z, _asm_apps_reloadApp

.returnOS:
    ld a, (editMode)
    or a, a
    call nz, ShowResult
    bit ti.monAbandon, (iy + ti.monFlags)
    jr nz, .turnOff
    ld a, (exitLaunchHook)
    or a, a
    jr nz, .turnOff
    sbc hl, hl
    add hl, sp
    ld de, stackBackup
    ld bc, 3 * 6 ; copy the top 6 stack entries
    ldir
    ld hl, (ti.onSP)
    ld bc, 3 * 6
    or a, a
    sbc hl, bc
    ld sp, hl
    ex de, hl
    ld hl, stackBackup
    ldir

.turnOff:
    pop hl
    pop bc
    ld a, c
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
    call _exit.sp + 3
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
    jp z, ti.ClrRawKeyHook
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
