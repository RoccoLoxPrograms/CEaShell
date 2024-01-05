; Copyright 2015-2024 Matt "MateoConLechuga" Waltz
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

include 'include/equates.inc'

    public _asm_labelJumper_showLabels

    extern _asm_utils_dispTextToolbar

_asm_labelJumper_showLabels:
    di
    ld a, (ti.cxCurApp)
    cp a, ti.cxPrgmEdit
    jr z, $ + 6
    xor a, a
    inc a
    dec a
    ret
    ld hl, .pageString
    ld de, ti.cursorImage + 32
    ld bc, stringRelocateSize
    ldir
    call ti.CursorOff
    or a, a
    sbc hl, hl
    ld (labelPage), hl
    inc l
    inc l
    ld (labelNumber), hl
    ld hl, (ti.editTail)
    ld (editTail), hl
    ld hl, (ti.editCursor)
    ld (editCursor), hl
    call ti.ClrTxtShd
    call ti.BufToTop
    call .countLabels
    ld bc, 0

.getLabelLoop:
    call ti.ClrScrn
    call ti.BufToTop
    call .drawLabels
    ld hl, (labelPage)
    ld de, ti.cursorImage + 32 + (.currentPageString - .pageString)
    inc hl
    call labelJumper_convertNum
    ld hl, ti.cursorImage + 32
    call _asm_utils_dispTextToolbar

.getKey:
    di
    call ti.DisableAPD
    call ti.GetCSC
    or a, a
    jr z, .getKey
    ld bc, 1
    cp a, ti.sk0
    jr z, .moveToLabel
    inc c
    cp a, ti.sk1
    jr z, .moveToLabel
    inc c
    cp a, ti.sk2
    jr z, .moveToLabel
    inc c
    cp a, ti.sk3
    jr z, .moveToLabel
    inc c
    cp a, ti.sk4
    jr z, .moveToLabel
    inc c
    cp a, ti.sk5
    jr z, .moveToLabel
    inc c
    cp a, ti.sk6
    jr z, .moveToLabel
    inc c
    cp a, ti.sk7
    jr z, .moveToLabel
    inc c
    cp a, ti.sk8
    jr z, .moveToLabel
    inc c
    cp a, ti.sk9
    jr z, .moveToLabel
    cp a, ti.skLeft
    jp z, .prevPage
    cp a, ti.skRight
    jp z, .nextPage
    cp a, ti.skClear
    jp z, .return
    cp a, ti.skMode
    jp z, .return
    jr .getKey

.moveToLabel:
    ld a, (ti.curRow)
    cp a, c
    jr z, .okay
    jr c, .getKey

.okay:
    ld hl, (labelPage)
    add hl, de
    or a, a
    sbc hl, de
    jr nz, .normalLabel
    ld a, c
    dec c
    dec c
    dec a
    jr nz, .notTop
    call ti.BufToTop
    jr .gotoEditor

.notTop:
    dec a
    jr nz, .normalLabel
    call ti.BufToBtm
    jr .gotoEditor

.normalLabel:
    call .computePageOffsetHL
    add hl, bc
    push hl
    call ti.BufToTop
    pop bc
    call .skipLabels
    call ti.BufLeft

.gotoEditor:
    call ti.ClrScrn
    xor a, a
    ld (ti.curCol), a
    ld (ti.curRow), a
    ld a, (ti.winTop)
    or a, a
    jr z, .inCEashellEditor
    ld hl, .programString
    call ti.PutS
    ld hl, ti.progToEdit
    call ti.PutS
    call ti.NewLine

.inCEashellEditor:
    ld a, ':'
    call ti.PutMap
    ld a, 1
    ld (ti.curCol), a

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
    call ti.DrawStatusBar
    xor a, a
    inc a
    dec a
    ret

.return:
    ld hl, (editTail)
    ld (ti.editTail), hl
    ld hl, (editCursor)
    ld (ti.editCursor), hl
    jr .gotoEditor

.nextPage:
    ld hl, (labelPage)
    ld de, (labelNumberOfPages)
    or a, a
    sbc hl, de
    add hl, de
    jr z, .firstPage
    inc hl
    jr .setPage

.firstPage:
    or a, a
    sbc hl, hl

.setPage:
    ld (labelPage), hl
    jp .getLabelLoop

.prevPage:
    ld hl, (labelPage)
    add hl, de
    or a, a
    sbc hl, de
    jr z, .lastPage
    dec hl
    jr .setPage

.lastPage:
    ld hl, (labelNumberOfPages)
    jr .setPage

.countLabels:
    call ti.BufToTop

.loop:
    call ti.BufRight
    jr z, .getNumPages
    ld a, d
    cp a, ti.t2ByteTok
    jr z, .loop
    ld a, e
    cp a, ti.tLbl
    jr nz, .loop
    ld hl, (labelNumber)
    inc hl
    ld (labelNumber), hl
    jr .loop

.getNumPages:
    ld hl, (labelNumber)
    dec hl
    ld a, 10
    call ti.DivHLByA
    ld (labelNumberOfPages), hl
    inc hl
    ld de, ti.cursorImage + 32 + (.totalPageString - .pageString)
    jp labelJumper_convertNum

.skipLabelsLoop:
    push bc
    call ti.BufRight
    pop bc
    ret z
    ld a, d
    cp a, ti.t2ByteTok
    jr z, .skipLabelsLoop
    ld a, e
    cp a, ti.tLbl
    jr nz, .skipLabelsLoop
    dec bc

.skipLabels:
    sbc hl, hl
    adc hl, bc
    jr nz, .skipLabelsLoop
    ret

.drawLabels:
    xor a, a
    ld (ti.curCol), a
    ld (ti.curRow), a
    ld hl, (labelPage)
    add hl, de
    or a ,a
    sbc hl, de
    jr nz, .normalPage
    ld hl, .topLabel
    call ti.PutS
    xor a, a
    ld (ti.curCol), a
    inc a
    ld (ti.curRow), a
    ld hl, .bottomLabel
    call ti.PutS
    call ti.BufToTop
    xor a, a
    ld (ti.curCol), a
    ld a, 2
    ld (ti.curRow), a

.normalPage:
    call .computePageOffset
    call .skipLabels
    ld hl, labelName

.parseLabels:
    push hl
    call ti.BufRight
    pop hl
    ret z
    ld a, d
    or a, a
    jr nz, .parseLabels
    ld a, e
    cp a, ti.tLbl
    jr nz, .parseLabels

.addLabel:
    push hl
    call ti.BufRight
    pop hl
    jr z, .addedLabel
    ld a, e
    cp a, ti.tColon
    jr z, .addedLabel
    cp a, ti.tEnter
    jr z, .addedLabel
    ld a, d
    or a, a
    jr z, .single
    ld (hl), a
    inc hl

.single:
    ld (hl), e
    inc hl
    jr .addLabel

.addedLabel:
    xor a, a
    ld (hl), a
    ld (ti.curCol), a
    ld a, (ti.curRow)
    add a, '0'
    call ti.PutC
    ld a, ':'
    call ti.PutC
    ld hl, labelName
    push hl

.displayLine:
    ld a, (hl)
    or a, a
    jr z, .leftEdge
    inc hl
    call ti.Isa2ByteTok
    ld d, 0
    jr nz, .singleByte

.multiByte:
    ld d, a
    ld e, (hl)
    inc hl
    jr .getString

.singleByte:
    ld e, a

.getString:
    push hl
    call ti.GetTokString
    ld b, (hl)
    inc hl

.loopDisplay:
    ld a, (ti.curCol)
    cp a, $19
    jr z, .leftEdgePop
    ld a, (hl)
    inc hl
    call ti.PutC
    djnz .loopDisplay
    pop hl
    jr .displayLine

.leftEdgePop:
    pop hl

.leftEdge:
    ld a, (ti.curRow)
    inc a
    ld (ti.curRow), a
    cp a, 10
    pop hl
    jp nz, .parseLabels
    ret

.computePageOffsetHL:
    push bc
    call .computePageOffset
    pop bc
    ret

.computePageOffset:
    ld hl, (labelPage)
    add hl, de
    or a, a
    sbc hl, de
    push hl
    pop bc
    ret z
    ld bc, 10
    call ti._imulu
    dec hl
    dec hl
    push hl
    pop bc
    ret

.programString:
    db "PROGRAM:", 0

.pageString:
    db "Use <> to switch page:     <"

.currentPageString:
    db "000"
    db " of "

.totalPageString:
    db "000"
    db ">", 0
stringRelocateSize := $ - .pageString

.topLabel:
    db "0:PRGM TOP", 0

.bottomLabel:
    db "1:PRGM BOTTOM", 0

labelJumper_convertNum:
    ld a, 4

.entry:
    ld bc, -100
    call .aqu
    ld c, -10
    call .aqu
    ld c, b

.aqu:
    ld a, '0' - 1

.under:
    inc a
    add hl, bc
    jr c, .under
    sbc hl, bc
    ld (de), a
    inc de
    ret
