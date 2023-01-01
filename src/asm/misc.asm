;--------------------------------------
;
; CEaShell Source Code - misc.asm
; By RoccoLox Programs and TIny_Hacker
; Copyright 2022 - 2023
; License: GPL-3.0
;
;--------------------------------------

	assume adl=1

	section .text

include 'include/ti84pceg.inc'

	public _willNotGarbageCollect
	public _invertPalette
	public _arcUnarc

_willNotGarbageCollect:
    push ix
    ld ix, 0
    add ix, sp
    ld hl, (ix + 6) ; get name
    ld a, (ix + 9)
    pop ix
    ld (ti.OP1), a ; move type to OP1
    ld de, ti.OP1 + 1
    ld bc, 8
    ldir ; move name to OP1
    call ti.ChkFindSym
	ret	c
	ex de, hl
	push hl
	add	hl, hl
	pop	hl
	ld a, 1
    ret nc
	ld hl, (hl)
	ld a, c
	add	a, 12
	ld c, a
	ld b, 0
	add.s hl, bc
	ld a, 0
    ret c
	push hl
	pop	bc
	call ti.FindFreeArcSpot
    ld a, 1
    ret nz
    dec a
    ret

_invertPalette:
    ld hl, $E30200 ; memory address of the palette
	ld c, 2

.loadLoop:
	ld b, 0

.loop:
	ld a, 255
	sub a, (hl)
	ld (hl), a
	inc hl
	djnz .loop
	dec c
	jr nz, .loadLoop
    ret

; -------------------------------------------------------------------------------

; Copyright 2015-2022 Matt "MateoConLechuga" Waltz
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;
; 1. Redistributions of source code must retain the above copyright notice,
;    this list of conditions and the following disclaimer.
;
; 2. Redistributions in binary form must reproduce the above copyright notice,
;    this list of conditions and the following disclaimer in the documentation
;    and/or other materials provided with the distribution.
;
; 3. Neither the name of the copyright holder nor the names of its contributors
;    may be used to endorse or promote products derived from this software
;    without specific prior written permission.
;
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
; AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
; IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
; ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
; LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
; CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
; SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
; INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
; CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
; ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
; POSSIBILITY OF SUCH DAMAGE.



; safely unarchive a variable and restore screen mode
; in the case of a garbage collect
; returns nz if okay, z if garbage collect
; derived from https://github.com/calc84maniac/tiboyce/blob/350e414dfc345d5e754eb87c1b87bc4e06131e71/tiboyce.asm#L468
_arcUnarc: ; previously cesium.Arc_Unarc
	call ti.ChkFindSym
	ret	c
	ex de, hl
	push hl
	add	hl, hl
	pop	hl
	jr nc, .no_garbage_collect
	ld hl, (hl)
	ld a, c
	add	a, 12
	ld c, a
	ld b, 0
	add.s hl, bc
	jr c, .garbage_collect
	push hl
	pop	bc
	call ti.FindFreeArcSpot
	jr nz, .no_garbage_collect
.garbage_collect:
	xor	a, a
	push af
	call ti.boot.ClearVRAM
	ld a, $2d
	ld (ti.mpLcdCtrl), a
	call ti.DrawStatusBar
	jr .archive_or_unarchive
.no_garbage_collect:
	xor	a, a
	inc	a
	push af
.archive_or_unarchive:
	ld hl, error_handler
	call ti.PushErrorHandler
	call ti.Arc_Unarc
	call ti.PopErrorHandler
error_handler:
    pop af
	ret
