;--------------------------------------
;
; CEaShell Source Code - getASMPrgmSize.asm
; By RoccoLox Programs and TIny_Hacker
; Copyright 2022 - 2023
; License: GPL-3.0
;
;--------------------------------------

	assume adl=1

	section .text

include 'include/ti84pceg.inc'

	public _getASMPrgmSize

_getASMPrgmSize:
    ld hl, (ti.asm_prgm_size)
    ret
