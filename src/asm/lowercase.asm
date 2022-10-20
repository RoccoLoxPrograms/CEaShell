;--------------------------------------
;
; CEaShell Source Code - lowercase.asm
; By RoccoLox Programs and TIny_Hacker
; Copyright 2022
; License: GPL-3.0
;
;--------------------------------------

	assume adl=1

	section .text

include 'include/ti84pceg.inc'

	public _toggleLowercase
    public _checkLowercase

_toggleLowercase:
    push ix
    ld ix, 0
    add ix, sp
    ld a, (ix + 6) ; turn on or off lowercase
    pop ix
    ld iy, ti.flags
    cp a, 0
    jr z, .turnOff
    set ti.lwrCaseActive, (iy + ti.appLwrCaseFlag)
    ret

.turnOff:
    res ti.lwrCaseActive, (iy + ti.appLwrCaseFlag)
    ret

_checkLowercase:
    ld iy, ti.flags
    bit ti.lwrCaseActive, (iy + ti.appLwrCaseFlag)
    ld a, 0
    ret z
    ld a, 1
    ret
