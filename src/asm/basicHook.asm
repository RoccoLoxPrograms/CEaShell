; Copyright 2015-2021 Matt "MateoConLechuga" Waltz
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

; slightly modified from Cesium code

	assume adl=1

	section .text

include 'include/ti84pceg.inc'

    public _installStopHook
    public _removeStopHook

hookTokenStop := $d9 - $ce

_hookParser:
	db $83 ; hook signifier
	push af
	cp a, 2
	jq z, .maybeStop

.chain:
	ld a, (ti.appErr2)
	cp a, $7f
	jq nz, .noChain
	pop	af
	ld ix, (ti.appErr2 + 1)
	jp (ix)

.noChain:
	pop	af
	xor	a, a
	ret

.maybeStop:
	ld a, hookTokenStop ; check if stop token
	cp a, b
	jq nz, .chain

.stop:
    pop af
	ld a, ti.E_AppErr1
	jq ti.JError

_installStopHook: ; hook chaining and installation
    ; more cesium code
    xor	a, a
	ld (ti.appErr2), a
    ld iy, ti.flags ; just in case the toolchain messes stuff up
	bit	ti.parserHookActive, (iy + ti.hookflags4)
	jq z, .noChain
	ld	hl, (ti.parserHookPtr)
	ld de, _hookParser
	or a, a
	sbc	hl, de
	add	hl, de
	jq z, .checkIfBadExit

.chainHooks:
	ld a, (hl)
	cp a, $83
	jq nz, .noChain
	ex de, hl
	inc	de
	ld hl, ti.appErr2
	ld (hl), $7f
	inc	hl
	ld (hl), de
	jq .noChain

.checkIfBadExit:
	ld hl, ti.appErr2
	ld a, (hl)
	cp a, $7f
	jq nz, .noChain
	inc	hl
	ld hl, (hl)
	dec	hl
	jq .chainHooks

.noChain:
    ld hl, _hookParser
    jp ti.SetParserHook

_removeStopHook:
    bit	ti.parserHookActive, (iy + ti.hookflags4)
	jq z, .clearParser
	ld hl, (ti.parserHookPtr)
	ld de, _hookParser
	or a, a
	sbc	hl, de
	add	hl, de
	ret	nz
	ld hl, ti.appErr2
	ld a, (hl)
	cp a, $7f
	jq nz, .clearParser
	inc	hl
	ld hl, (hl)
	dec	hl
	xor	a, a
	ld (ti.appErr2), a
	jq ti.SetParserHook

.clearParser:
	jq	ti.ClrParserHook
