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
    bit 0, a
    jr z, .turnOff
    set ti.lwrCaseActive, (iy + ti.appLwrCaseFlag)
    ret

.turnOff:
    res ti.lwrCaseActive, (iy + ti.appLwrCaseFlag)
    ret

_checkLowercase:
    ld iy, ti.flags
    xor a, a
    bit ti.lwrCaseActive, (iy + ti.appLwrCaseFlag)
    ret z
    inc a
    ret
