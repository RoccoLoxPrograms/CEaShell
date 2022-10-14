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

    assume adl=1

	section .text

include 'include/ti84pceg.inc'

	public hook_show_labels

edit_cursor := ti.cursorImage + 3
edit_tail := edit_cursor + 3
label_number := edit_tail + 3
label_number_of_pages := label_number + 3
label_page := label_number_of_pages + 3
label_name := label_page + 3

program_string := ti.cursorImage + 32
page_string := hook_show_labels.page_string - hook_show_labels.program_string + ti.cursorImage + 32
current_page_string := hook_show_labels.current_page_string - hook_show_labels.program_string + ti.cursorImage + 32
total_page_string := hook_show_labels.total_page_string - hook_show_labels.program_string + ti.cursorImage + 32
top_label := hook_show_labels.top_label - hook_show_labels.program_string + ti.cursorImage + 32
bottom_label := hook_show_labels.bottom_label - hook_show_labels.program_string + ti.cursorImage + 32

hook_show_labels:
	di
	ld	a,(ti.cxCurApp)
	cp	a,ti.cxPrgmEdit
	jp nz, hook_get_key_none

    ld hl, .program_string ; hacky relocate
    ld de, ti.cursorImage + 32
    ld bc, endRelocate
    ldir

	call	ti.CursorOff

	or	a,a
	sbc	hl,hl
	ld	(label_page),hl
	inc	l
	inc	l
	ld	(label_number),hl
	ld	hl,(ti.editTail)
	ld	(edit_tail),hl
	ld	hl,(ti.editCursor)
	ld	(edit_cursor),hl
	call	ti.ClrTxtShd
	call	ti.BufToTop

	call	.countlabels

	ld	bc,0
.getlabelloop:
	call 	ti.ClrScrn
	call	ti.BufToTop

	call	.drawlabels

	ld	hl,(label_page)
	ld	de,current_page_string
	inc	hl
	call	helper_num_convert

	ld	hl,page_string
	call	helper_vputs_toolbar

.getkey:
	di
	call	ti.DisableAPD
	call	ti.GetCSC
	or	a,a
	jr	z,.getkey
	ld	bc,1
	cp	a,ti.sk0
	jr	z,.movetolabel
	inc	c
	cp	a,ti.sk1
	jr	z,.movetolabel
	inc	c
	cp	a,ti.sk2
	jr	z,.movetolabel
	inc	c
	cp	a,ti.sk3
	jr	z,.movetolabel
	inc	c
	cp	a,ti.sk4
	jr	z,.movetolabel
	inc	c
	cp	a,ti.sk5
	jr	z,.movetolabel
	inc	c
	cp	a,ti.sk6
	jr	z,.movetolabel
	inc	c
	cp	a,ti.sk7
	jr	z,.movetolabel
	inc	c
	cp	a,ti.sk8
	jr	z,.movetolabel
	inc	c
	cp	a,ti.sk9
	jr	z,.movetolabel
	cp	a,ti.skLeft
	jp	z,.prevpage
	cp	a,ti.skRight
	jp	z,.nextpage
	cp	a,ti.skClear
	jp	z,.return
	cp	a,ti.skMode
	jp	z,.return
	jr	.getkey

.movetolabel:
	ld	a,(ti.curRow)
	cp	a,c
	jr	z,.okay
	jr	c,.getkey
.okay:
	ld	hl,(label_page)
        add	hl,de
	or	a,a
	sbc	hl,de
	jr	nz,.normallabel
	ld	a,c
	dec	c
	dec	c
	dec	a
	jr	nz,.nottop
	call	ti.BufToTop
	jr	.gotoeditor
.nottop:
	dec	a
	jr	nz,.normallabel
	call	ti.BufToBtm
	jr	.gotoeditor
.normallabel:
	call	.computepageoffsethl
	add	hl,bc
	push	hl
	call	ti.BufToTop
	pop	bc
	call	.skiplabels
	call	ti.BufLeft
.gotoeditor:
	call 	ti.ClrScrn
	xor	a,a
	ld	(ti.curCol),a
	ld	(ti.curRow),a
	ld	a,(ti.winTop)
	or	a,a
	jr	z,.incesiumeditor	; check if using cesium feature
	ld	hl,program_string
	call	ti.PutS
	ld	hl,ti.progToEdit
	call	ti.PutS
	call	ti.NewLine
.incesiumeditor:
	ld	a,':'
	call	ti.PutMap
	ld	a,1
	ld	(ti.curCol),a
.backup:
	call	ti.BufLeft
	jr	z,.done
	ld	a,d
	or	a,a
	jr	nz,.backup
	ld	a,e
	cp	a,ti.tEnter
	jr	nz,.backup
	call	ti.BufRight
.done:
	call	ti.DispEOW
	call	ti.CursorOn
	call	ti.DrawStatusBar
	jp hook_get_key_none

.return:
	ld	hl,(edit_tail)
	ld	(ti.editTail),hl
	ld	hl,(edit_cursor)
	ld	(ti.editCursor),hl
	jr	.gotoeditor

.nextpage:
	ld	hl,(label_page)
	ld	de,(label_number_of_pages)
	or	a,a
	sbc	hl,de
	add	hl,de
	jr	z,.firstpage
	inc	hl
	jr	.setpage
.firstpage:
	or	a,a
	sbc	hl,hl
.setpage:
	ld	(label_page),hl
	jp	.getlabelloop
.prevpage:
	ld	hl,(label_page)
	add	hl,de
	or	a,a
	sbc	hl,de
	jr	z,.lastpage
	dec	hl
	jr	.setpage
.lastpage:
	ld	hl,(label_number_of_pages)
	jr	.setpage

.countlabels:
	call	ti.BufToTop
.loop:
	call	ti.BufRight
	jr	z,.getnumpages
	ld	a,d
	cp	a,ti.t2ByteTok
	jr	z,.loop
	ld	a,e
	cp	a,ti.tLbl
	jr	nz,.loop
	ld	hl,(label_number)
	inc	hl
	ld	(label_number),hl
	jr	.loop
.getnumpages:
	ld	hl,(label_number)
	dec	hl
	ld	a,10
	call	ti.DivHLByA
	ld	(label_number_of_pages),hl
	inc	hl
	ld	de,total_page_string
	jp	helper_num_convert

.skiplabelsloop:
	push	bc
	call	ti.BufRight
	pop	bc
	ret	z
	ld	a,d
	cp	a,ti.t2ByteTok
	jr	z,.skiplabelsloop
	ld	a,e
	cp	a,ti.tLbl
	jr	nz,.skiplabelsloop
	dec	bc
.skiplabels:
	sbc	hl,hl
	adc	hl,bc
	jr	nz,.skiplabelsloop
	ret

.drawlabels:
	xor	a,a
	ld	(ti.curCol),a
	ld	(ti.curRow),a
	ld	hl,(label_page)
        add	hl,de
	or	a,a
	sbc	hl,de
	jr	nz,.normalpage
	ld	hl,top_label
	call	ti.PutS
	xor	a,a
	ld	(ti.curCol),a
	inc	a
	ld	(ti.curRow),a
	ld	hl,bottom_label
	call	ti.PutS
	call	ti.BufToTop
	xor	a,a
	ld	(ti.curCol),a
	ld	a,2
	ld	(ti.curRow),a
.normalpage:
	call	.computepageoffset
	call	.skiplabels
	ld	hl,label_name
.parse_labels:
	push	hl
	call	ti.BufRight
	pop	hl
	ret	z
	ld	a,d
	or	a,a
	jr	nz,.parse_labels
	ld	a,e
	cp	a,ti.tLbl
	jr	nz,.parse_labels
.add_label:
	push	hl
	call	ti.BufRight
	pop	hl
	jr	z,.added_label
	ld	a,e
	cp	a,ti.tColon
	jr	z,.added_label
	cp	a,ti.tEnter
	jr	z,.added_label
	ld	a,d
	or	a,a
	jr	z,.single
	ld	(hl),a
	inc	hl
.single:
	ld	(hl),e
	inc	hl
	jr	.add_label
.added_label:
	xor	a,a
	ld	(hl),a
	ld	(ti.curCol),a
	ld	a,(ti.curRow)
	add	a,'0'
	call	ti.PutC
	ld	a,':'
	call	ti.PutC
	ld	hl,label_name
	push	hl
.displayline:
	ld	a,(hl)
	or	a,a
	jr	z,.leftedge
	inc	hl
	call	ti.Isa2ByteTok
	ld	d,0
	jr	nz,.singlebyte
.multibyte:
	ld	d,a
	ld	e,(hl)
	inc	hl
	jr	.getstring
.singlebyte:
	ld	e,a
.getstring:
	push	hl
	call	ti.GetTokString
	ld	b,(hl)
	inc	hl
.loopdisplay:
	ld	a,(ti.curCol)
	cp	a,$19
	jr	z,.leftedgepop
	ld	a,(hl)
	inc	hl
	call	ti.PutC
	djnz	.loopdisplay
	pop	hl
	jr	.displayline
.leftedgepop:
	pop	hl
.leftedge:
	ld	a,(ti.curRow)
	inc	a
	ld	(ti.curRow),a
	cp	a,10
	pop	hl
	jp	nz,.parse_labels
	ret

.computepageoffsethl:
	push	bc
	call	.computepageoffset
	pop	bc
	ret
.computepageoffset:
	ld	hl,(label_page)
	add	hl,de
	or	a,a
	sbc	hl,de
	push	hl
	pop	bc
	ret	z
	ld	bc,10
	call	ti._imulu
	dec	hl
	dec	hl
	push	hl
	pop	bc
	ret

.program_string:
	db	"PROGRAM:",0
.page_string:
	db	"Use <> to switch page:     <"
.current_page_string:
	db	"000"
	db	" of "
.total_page_string:
	db	"000"
	db	">",0
.top_label:
	db	"0:PRGM TOP",0
.bottom_label:
	db	"1:PRGM BOTTOM",0

endRelocate := $-.program_string

helper_num_convert:
	ld	a,4
.entry:
	ld	bc,-100
	call	.aqu
	ld	c,-10
	call	.aqu
	ld	c,b
.aqu:
	ld	a,'0' - 1
.under:
	inc	a
	add	hl,bc
	jr	c,.under
	sbc	hl,bc
	ld	(de),a
	inc	de
	ret

helper_vputs_toolbar:
	di
	ld	a,$d0
	ld	mb,a
	ld	de,$e71c
	ld.sis	(ti.drawFGColor and $ffff),de
	ld.sis	de,(ti.statusBarBGColor and $ffff)
	ld.sis	(ti.drawBGColor and $ffff),de
	ld	a,14
	ld	(ti.penRow),a
	ld	de,2
	ld.sis	(ti.penCol and $ffff), de
	jp	ti.VPutS

hook_get_key_none:
	xor	a,a
	inc	a
	dec	a
	ret
