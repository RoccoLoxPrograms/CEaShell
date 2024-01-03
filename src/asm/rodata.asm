;--------------------------------------
;
; CEaShell Source Code - rodata.asm
; By RoccoLox Programs and TIny_Hacker
; Copyright 2022 - 2023
; License: GPL-3.0
;
;--------------------------------------

    assume adl=1

    section .rodata

include 'include/equates.inc'

    public _rodata_celticAppVarHeader
    public _rodata_hashProg
    public _rodata_appVarName
    public _rodata_appName
    public _rodata_tempAppVarPrgm
    public _rodata_errorQuit
    public _rodata_errorGoto
    public _rodata_basicPrgmName
    public _rodata_hexaEditHeader
    public _rodata_osColorToXlibC
    public _rodata_characters
    public _rodata_sizeOfCharsLUT

_rodata_celticAppVarHeader:
    db ti.tColon, ti.tC, ti.tE, ti.tL, ti.tEnter

_rodata_hashProg:
    db ti.ProgObj, '#', 0

_rodata_appVarName:
_rodata_appName := $ + 1
    db ti.AppVarObj, 'CEaShell', 0

_rodata_tempAppVarPrgm:
    db ti.ProgObj, "appvar", 0

_rodata_errorQuit:
    db '1:', 0, 'Quit', 0

_rodata_errorGoto:
    db '2:', 0, 'Goto', 0

_rodata_basicPrgmName:
    db ti.ProtProgObj, "CEaShTmp", 0

_rodata_hexaEditHeader:
    db "HexaEdit", 1 shl 2

_rodata_osColorToXlibC: ; lines up with xlibc color picker
    db 255 ; white (transparent but not)
    db 25 ; blue
    db 224 ; red
    db 0 ; black
    db 249 ; magenta
    db 5 ; green
    db 227 ; orange
    db 160 ; brown
    db 16 ; navy
    db 28 ; light blue
    db 0 ; skip these tokens
    db 0
    db 0
    db 0
    db 0
    db 0
    db 0
    db 231 ; yellow
    db 255 ; white
    db 222 ; light gray
    db 181 ; medium gray
    db 148 ; gray
    db 74 ; dark gray

_rodata_characters:
    ; numbers
    db 0, 0, 0, 0, 0, 0, 0     ; + - × ÷ ^ undef
    db 0, '369', 0, 0, 0, 0    ; (-) 3 6 9 ) TAN VARS undef
    db 0, '258', 0, 0, 0, 0    ; . 2 5 8 ( COS PRGM STAT
    db '0147', 0, 0, 0, 'X', 0 ; 0 1 4 7 , SIN APPS XT?n undef
    db 0, 0, 0, 0, 0, 0        ; STO LN LOG x2 x-1 MATH
_rodata_sizeOfCharsLUT := $ - _rodata_characters

    ; uppercase letters
    db 0, 'WRMH', 0, 0         ; + - × ÷ ^ undef
    db 0, 'Z'+ 1, 'VQLG', 0, 0 ; (-) 3 6 9 ) TAN VARS undef
    db 0, 'ZUPKFC', 0          ; . 2 5 8 ( COS PRGM STAT
    db 0, 'YTOJEBX', 0         ; 0 1 4 7 , SIN APPS XT?n undef
    db 'XSNIDA'                ; STO LN LOG x2 x-1 MATH

    ; lowercase letters
    db 0, 'wrmh', 0, 0         ; + - × ÷ ^ undef
    db 0, 'Z'+ 1, 'vqlg', 0, 0 ; (-) 3 6 9 ) TAN VARS undef
    db 0, 'zupkfc', 0          ; . 2 5 8 ( COS PRGM STAT
    db 0, 'ytojebX', 0         ; 0 1 4 7 , SIN APPS XT?n undef
    db 'xsnida'                ; STO LN LOG x2 x-1 MATH
