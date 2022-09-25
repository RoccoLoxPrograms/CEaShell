;--------------------------------------
;
; CEaShell Source Code - invert.asm
; By RoccoLox Programs and TIny_Hacker
; Copyright 2022
; License: GPL-3.0
;
;--------------------------------------

    assume adl=1

    section .text

    public _invertPalette

_invertPalette:
    ld hl, $e30200 ; memory address of the palette
    ld b, 255
    ld d, 0

_loop:
    call _invertHex
    djnz _loop
    call _invertHex
    inc d
    ld b, 255
    ld a, d
    cp a, 2
    jr nz, _loop
    ret

_invertHex:
    ld a, 255
    ld c, (hl)
    sub a, c
    ld (hl), a
    inc hl
    ret
