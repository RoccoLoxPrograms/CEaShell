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

; entry points
; required: OP1 = name of program to edit

	assume adl=1

	section .text

include 'include/ti84pceg.inc'

	public edit_basic_program_goto
	public edit_basic_program
	public _editBasicProg
	extern _reloadApp

returnCEaShell := ti.pixelShadow2
edit_status := returnCEaShell + 1
edit_mode := edit_status + 1
edit_goto := $ee
edit_archived := $ff
string_temp := ti.mpLcdCrsrImage
backup_prgm_name := edit_mode + 1

errorOffset := (edit_helper.error_offset - edit_helper) + ti.cursorImage + 253

_editBasicProg:
	ld hl, returnCEaShell
	ld (hl), 1
	push ix
    ld ix, 0
    add ix, sp
    ld a, (ix + 9) ; get type
    ld hl, (ix + 6) ; get name
    pop ix
    ld (ti.OP1), a ; move type to OP1
    ld de, ti.OP1 + 1
    ld bc, 8
    ldir ; move name to OP1
	jp edit_basic_program

edit_basic_program_goto:
	ld hl, returnCEaShell
	ld (hl), 1
	call	compute_error_offset
	ld	a,edit_goto
	ld	(edit_mode),a
	jr	edit_basic_program.entry
edit_basic_program:
	xor	a,a
	sbc	hl,hl
	ld	(errorOffset),hl		; perhaps in future restore location?
	ld	(edit_mode),a
.entry:
	xor	a,a
	ld	(edit_status),a
	call	util_backup_prgm_name
	call	util_op1_to_temp

.no_external_editor:
	call	ti.PushOP1			; for restoring in hook
	call	ti.ChkFindSym
	call	ti.ChkInRam
	jr	z,.not_archived
	ld	a,edit_archived
	ld	(edit_status),a
	call	ti.Arc_Unarc
.not_archived:
	ld	hl,hook_app_change
	ld iy, ti.flags	; C toolchain messes up this
	call	ti.SetAppChangeHook
	xor	a,a
	ld	(ti.menuCurrent),a
	call	ti.CursorOff
	call	ti.RunIndicOff
	call ti.DrawStatusBar
	ld	hl,string_temp			; contains OP1
	push	hl
	ld	de,ti.progToEdit
	call	ti.Mov9b
	pop	hl
	dec	hl
	ld	de,ti.basic_prog
	call	ti.Mov9b
	push hl
	push de
	push bc
	ld hl, edit_helper
	ld de, ti.cursorImage + 256
	ld bc, edit_end - edit_helper
	ldir
	pop bc
	pop de
	pop hl
	jp ti.cursorImage + 256

edit_helper:
	ld	a,ti.cxPrgmEdit
	call	ti.NewContext
	xor	a,a
	ld	(ti.winTop),a
	call	ti.ScrollUp
	call	ti.HomeUp
	ld	a,':'
	call	ti.PutC
	ld	a,(edit_mode)
	or	a
	jr	z,.goto_none

	ld	hl,(ti.editTop)
	ld	de,(ti.editCursor)
	or	a,a
	sbc	hl,de
	add	hl,de
	jr	nz,.goto_end

	ld	bc,0
.error_offset:
	call	ti.ChkBCIs0
	jr	z,.goto_end
	ld	hl,(ti.editTail)
	ldir
	ld	(ti.editTail),hl
	ld	(ti.editCursor),de
	call	.goto_new_line
.goto_end:
	call	ti.DispEOW
	ld	hl,$100
	ld.sis	(ti.curRow and $ffff),hl
	jr	.skip

.goto_none:
	call	ti.DispEOW
	ld	hl,$100
	ld.sis	(ti.curRow and $ffff),hl
	call	ti.BufToTop
.skip:
	xor	a,a
	ld	(ti.menuCurrent),a
	set	7,(iy + $28)
	jp	ti.Mon

.goto_new_line:
	ld	hl,(ti.editCursor)
	ld	a,(hl)
	cp	a,ti.tEnter
	jr	z,.goto_new_line_back
.loop:
	ld	a,(hl)
	ld	de,(ti.editTop)
	or	a,a
	sbc	hl,de
	ret	z
	add	hl,de
	dec	hl
	push	af
	ld	a,(hl)
	call	ti.Isa2ByteTok
	pop	de
	jr	z,.goto_new_line_back
	ld	a,d
	cp	a,ti.tEnter
	jr	z,.goto_new_line_next
.goto_new_line_back:
	call	ti.BufLeft
	ld	hl,(ti.editCursor)
	jr	.loop
.goto_new_line_next:
	jp	ti.BufRight
edit_end:

compute_error_offset:
	ld	hl,(ti.curPC)
	ld	bc,(ti.begPC)
	or	a,a
	sbc	hl,bc
	ld	(errorOffset),hl
	ret

util_op1_to_temp:
	ld	de,string_temp
	push	de
	call	ti.ZeroOP
	ld	hl,ti.OP1 + 1
	pop	de
.handle:
	push	de
	call	ti.Mov8b
	pop	hl
	ret

util_backup_prgm_name:
	ld	hl,ti.OP1
.entry:
	ld	de,backup_prgm_name
	jp	ti.Mov9b

hook_app_change:
	db	$83
	ld	c,a
	ld	a,b
	cp	a,ti.cxPrgmEdit		; only allow when editing
	ld	b,a
	ret	nz
	ld	a,c
	or	a,a
	ld iy, ti.flags
	call ti.ClrAppChangeHook
	jr	z,.close_editor
	cp	a,ti.cxMode
	ret	z
	cp	a,ti.cxFormat
	ret	z
	cp	a,ti.cxTableSet
	ret	z
	call	.close_editor
	push af
	ld hl, returnCEaShell
	ld a, 0
	cp a, (hl)
	jp nz, .exitApp
	pop af
	pop de
	pop ix
	pop ix
	push de ; sort the stack for proper return?
	ret
.close_editor:
	push	af, bc, hl
	call	ti.CursorOff
	call	ti.CloseEditEqu
	ld hl, backup_prgm_name
	call ti.Mov9ToOP1
	call	ti.ChkFindSym
	jr	c,.noarc
	ld	a,(edit_status)
	or	a,a
	call	nz,ti.Arc_Unarc
.noarc:
	pop	hl, bc, af
	ret
.exitApp:
	pop af
	jp _reloadApp
