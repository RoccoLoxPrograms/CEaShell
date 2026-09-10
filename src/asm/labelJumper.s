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

    .assume adl=1

    .include "src/asm/include/equates.inc"

    .global _asm_labelJumper_showLabels
    .type   _asm_labelJumper_showLabels, @function
    .global _asm_labelJumper_convertNum
    .type   _asm_labelJumper_convertNum, @function

    .extern _asm_utils_dispTextToolbar

;--------------------------------------

    .section .text

_asm_labelJumper_showLabels:
    di
    ld a, (ti.cxCurApp)
    cp a, ti.cxPrgmEdit
    jr z, $ + 6
    xor a, a
    inc a
    dec a
    ret
    ld.sis hl, (ti.localLanguage & $FFFF)
    or a, a
    ld de, $010C ; check for French language
    sbc hl, de
    res frenchLanguage, (iy + shellFlags)
    ld hl, showLabels.pageString
    ld bc, stringRelocateSize
    jr nz, $ + 14
    set frenchLanguage, (iy + shellFlags)
    ld hl, showLabels.pageStringFR
    ld bc, stringRelocateSizeFR
    ld de, ti.cursorImage + 32
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
    call showLabels.countLabels
    ld bc, 0

showLabels.getLabelLoop:
    call ti.ClrScrn
    call ti.BufToTop
    call showLabels.drawLabels
    ld hl, (labelPage)
    ld de, ti.cursorImage + 32 + (showLabels.currentPageString - showLabels.pageString)
    bit frenchLanguage, (iy + shellFlags)
    jr z, $ + 6
    ld de, ti.cursorImage + 32 + (showLabels.currentPageStringFR - showLabels.pageStringFR)
    inc hl
    call convertNum.threeDigits
    ld hl, ti.cursorImage + 32
    call _asm_utils_dispTextToolbar

showLabels.getKey:
    di
    call ti.DisableAPD
    call ti.GetCSC
    or a, a
    jr z, showLabels.getKey
    ld bc, 1
    cp a, ti.sk0
    jr z, showLabels.moveToLabel
    inc c
    cp a, ti.sk1
    jr z, showLabels.moveToLabel
    inc c
    cp a, ti.sk2
    jr z, showLabels.moveToLabel
    inc c
    cp a, ti.sk3
    jr z, showLabels.moveToLabel
    inc c
    cp a, ti.sk4
    jr z, showLabels.moveToLabel
    inc c
    cp a, ti.sk5
    jr z, showLabels.moveToLabel
    inc c
    cp a, ti.sk6
    jr z, showLabels.moveToLabel
    inc c
    cp a, ti.sk7
    jr z, showLabels.moveToLabel
    inc c
    cp a, ti.sk8
    jr z, showLabels.moveToLabel
    inc c
    cp a, ti.sk9
    jr z, showLabels.moveToLabel
    cp a, ti.skLeft
    jp z, showLabels.prevPage
    cp a, ti.skRight
    jp z, showLabels.nextPage
    cp a, ti.skClear
    jp z, showLabels.return
    cp a, ti.skMode
    jp z, showLabels.return
    jr showLabels.getKey

showLabels.moveToLabel:
    ld a, (ti.curRow)
    cp a, c
    jr z, showLabels.okay
    jr c, showLabels.getKey

showLabels.okay:
    ld hl, (labelPage)
    add hl, de
    or a, a
    sbc hl, de
    jr nz, showLabels.normalLabel
    ld a, c
    dec c
    dec c
    dec a
    jr nz, showLabels.notTop
    call ti.BufToTop
    jr showLabels.gotoEditor

showLabels.notTop:
    dec a
    jr nz, showLabels.normalLabel
    call ti.BufToBtm
    jr showLabels.gotoEditor

showLabels.normalLabel:
    call showLabels.computePageOffsetHL
    add hl, bc
    push hl
    call ti.BufToTop
    pop bc
    call showLabels.skipLabels
    call ti.BufLeft

showLabels.gotoEditor:
    call ti.ClrScrn
    xor a, a
    ld (ti.curCol), a
    ld (ti.curRow), a
    ld a, (ti.winTop)
    or a, a
    jr z, showLabels.inCEashellEditor
    ld hl, showLabels.programString
    call ti.PutS
    ld hl, ti.progToEdit
    call ti.PutS
    call ti.NewLine

showLabels.inCEashellEditor:
    ld a, ':'
    call ti.PutMap
    ld hl, 1
    ld.sis (ti.curCol & $FFFF), hl

showLabels.backup:
    call ti.BufLeft
    jr z, showLabels.done
    ld a, d
    or a, a
    jr nz, showLabels.backup
    ld a, e
    cp a, ti.tEnter
    jr nz, showLabels.backup
    call ti.BufRight

showLabels.done:
    call ti.DispEOW
    call ti.CursorOn
    call ti.DrawStatusBar
    xor a, a
    inc a
    dec a
    ret

showLabels.return:
    ld hl, (editTail)
    ld (ti.editTail), hl
    ld hl, (editCursor)
    ld (ti.editCursor), hl
    jr showLabels.gotoEditor

showLabels.nextPage:
    ld hl, (labelPage)
    ld de, (labelNumberOfPages)
    or a, a
    sbc hl, de
    add hl, de
    jr z, showLabels.firstPage
    inc hl
    jr showLabels.setPage

showLabels.firstPage:
    or a, a
    sbc hl, hl

showLabels.setPage:
    ld (labelPage), hl
    jp showLabels.getLabelLoop

showLabels.prevPage:
    ld hl, (labelPage)
    add hl, de
    or a, a
    sbc hl, de
    jr z, showLabels.lastPage
    dec hl
    jr showLabels.setPage

showLabels.lastPage:
    ld hl, (labelNumberOfPages)
    jr showLabels.setPage

showLabels.countLabels:
    call ti.BufToTop

showLabels.loop:
    call ti.BufRight
    jr z, showLabels.getNumPages
    ld a, d
    cp a, ti.t2ByteTok
    jr z, showLabels.loop
    ld a, e
    cp a, ti.tLbl
    jr nz, showLabels.loop
    ld hl, (labelNumber)
    inc hl
    ld (labelNumber), hl
    jr showLabels.loop

showLabels.getNumPages:
    ld hl, (labelNumber)
    dec hl
    ld a, 10
    call ti.DivHLByA
    ld (labelNumberOfPages), hl
    inc hl
    ld de, ti.cursorImage + 32 + (showLabels.totalPageString - showLabels.pageString)
    bit frenchLanguage, (iy + shellFlags)
    jr z, $ + 6
    ld de, ti.cursorImage + 32 + (showLabels.totalPageStringFR - showLabels.pageStringFR)
    jp convertNum.threeDigits

showLabels.skipLabelsLoop:
    push bc
    call ti.BufRight
    pop bc
    ret z
    ld a, d
    cp a, ti.t2ByteTok
    jr z, showLabels.skipLabelsLoop
    ld a, e
    cp a, ti.tLbl
    jr nz, showLabels.skipLabelsLoop
    dec bc

showLabels.skipLabels:
    sbc hl, hl
    adc hl, bc
    jr nz, showLabels.skipLabelsLoop
    ret

showLabels.drawLabels:
    xor a, a
    ld (ti.curCol), a
    ld (ti.curRow), a
    ld hl, (labelPage)
    add hl, de
    or a ,a
    sbc hl, de
    jr nz, showLabels.normalPage
    ld hl, showLabels.topLabel
    bit frenchLanguage, (iy + shellFlags)
    jr z, $ + 6
    ld hl, showLabels.topLabelFR
    call ti.PutS
    xor a, a
    ld (ti.curCol), a
    inc a
    ld (ti.curRow), a
    ld hl, showLabels.bottomLabel
    bit frenchLanguage, (iy + shellFlags)
    jr z, $ + 6
    ld hl, showLabels.bottomLabelFR
    call ti.PutS
    call ti.BufToTop
    xor a, a
    ld (ti.curCol), a
    ld a, 2
    ld (ti.curRow), a

showLabels.normalPage:
    call showLabels.computePageOffset
    call showLabels.skipLabels
    ld hl, labelName

showLabels.parseLabels:
    push hl
    call ti.BufRight
    pop hl
    ret z
    ld a, d
    or a, a
    jr nz, showLabels.parseLabels
    ld a, e
    cp a, ti.tLbl
    jr nz, showLabels.parseLabels

showLabels.addLabel:
    push hl
    call ti.BufRight
    pop hl
    jr z, showLabels.addedLabel
    ld a, e
    cp a, ti.tColon
    jr z, showLabels.addedLabel
    cp a, ti.tEnter
    jr z, showLabels.addedLabel
    ld a, d
    or a, a
    jr z, showLabels.single
    ld (hl), a
    inc hl

showLabels.single:
    ld (hl), e
    inc hl
    jr showLabels.addLabel

showLabels.addedLabel:
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

showLabels.displayLine:
    ld a, (hl)
    or a, a
    jr z, showLabels.leftEdge
    inc hl
    call ti.Isa2ByteTok
    ld d, 0
    jr nz, showLabels.singleByte

showLabels.multiByte:
    ld d, a
    ld e, (hl)
    inc hl
    jr showLabels.getString

showLabels.singleByte:
    ld e, a

showLabels.getString:
    push hl
    call ti.GetTokString
    ld b, (hl)
    inc hl

showLabels.loopDisplay:
    ld a, (ti.curCol)
    cp a, $19
    jr z, showLabels.leftEdgePop
    ld a, (hl)
    inc hl
    call ti.PutC
    djnz showLabels.loopDisplay
    pop hl
    jr showLabels.displayLine

showLabels.leftEdgePop:
    pop hl

showLabels.leftEdge:
    ld a, (ti.curRow)
    inc a
    ld (ti.curRow), a
    cp a, 10
    pop hl
    jp nz, showLabels.parseLabels
    ret

showLabels.computePageOffsetHL:
    push bc
    call showLabels.computePageOffset
    pop bc
    ret

showLabels.computePageOffset:
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

showLabels.programString:
    db "PROGRAM:", 0

showLabels.pageString:
    db "Use <> to switch page:     <"

showLabels.currentPageString:
    db "000"
    db " of "

showLabels.totalPageString:
    db "000"
    db ">", 0

.equ stringRelocateSize, $ - showLabels.pageString

showLabels.topLabel:
    db "0:PRGM TOP", 0

showLabels.bottomLabel:
    db "1:PRGM BOTTOM", 0

showLabels.pageStringFR:
    db "Utiliser <> pour changer pages : <"

showLabels.currentPageStringFR:
    db "000"
    db " / "

showLabels.totalPageStringFR:
    db "000"
    db ">", 0

.equ stringRelocateSizeFR, $ - showLabels.pageStringFR

showLabels.topLabelFR:
    db "0:PRGM DESSUS", 0

showLabels.bottomLabelFR:
    db "1:PRGM DESSOUS", 0

_asm_labelJumper_convertNum:
    ld bc, -100000
    call convertNum.aqu
    ld bc, -10000
    call convertNum.aqu
    ld bc, -1000
    call convertNum.aqu

convertNum.threeDigits:
    ld bc, -100
    call convertNum.aqu
    ld c, -10
    call convertNum.aqu
    ld c, b

convertNum.aqu:
    ld a, '0' - 1

convertNum.under:
    inc a
    add hl, bc
    jr c, convertNum.under
    sbc hl, bc
    ld (de), a
    inc de
    ret
