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

; made by martin warmer, mmartin@xs4all.nl
; modified for ez80 architecture and hidden programs by matt waltz
;
; uses insertion sort to sort the vat alphabetically

    .assume adl=1

    .include "src/asm/include/equates.inc"

    .global _asm_fileSystem_sortVAT
    .type   _asm_fileSystem_sortVAT, @function
    .global _asm_fileSystem_initPtrArrays
    .type   _asm_fileSystem_initPtrArrays, @function
    .global _asm_fileSystem_error
    .type   _asm_fileSystem_error, @function
    .global _asm_fileSystem_getProgramPtrs
    .type   _asm_fileSystem_getProgramPtrs, @function
    .global _asm_fileSystem_getAppVarPtrs
    .type   _asm_fileSystem_getAppVarPtrs, @function
    .global _asm_fileSystem_findAllVars
    .type   _asm_fileSystem_findAllVars, @function
    .global _asm_fileSystem_findArrayOffset
    .type   _asm_fileSystem_findArrayOffset, @function

    .extern _asm_utils_clrScrnAndUsedRAM
    .extern _asm_utils_checkHiddenHeader
    .extern _asm_utils_dispQuitErr
    .extern _exit_sp
    .extern _gfx_End

;--------------------------------------

    .section .text

_asm_fileSystem_sortVAT:
    ld iy, ti.flags
    or a, a
    sbc hl, hl
    ld (sortFirstItemFoundPtr), hl
    ld hl, (ti.progPtr)

sortVAT.sortNext:
    call sortVAT.findNextItem
    ret nc

sortVAT.foundItem:
    push hl
    ld hl, (sortFirstItemFoundPtr)
    add hl, de
    or a, a
    sbc hl, de
    pop hl
    jr nz, sortVAT.notFirst
    ld (sortFirstItemFoundPtr), hl ; to make it only execute once
    call sortVAT.skipName
    ld (sortEndOfPartPtr), hl
    jr sortVAT.sortNext

sortVAT.notFirst:
    push hl
    call sortVAT.skipName
    pop de
    push hl ; to continue from later on
    ld hl, (sortFirstItemFoundPtr)
    jr sortVAT.searchNextStart ; could speed up sorted list by first checking if it's the last item (not neccessary)

sortVAT.searchNext:
    call sortVAT.skipName
    ld bc, (sortEndOfPartPtr)
    or a, a ; reset carry flag
    push hl
    sbc hl, bc
    pop hl
    jr z, sortVAT.locationFound
    ld bc, -6
    add hl, bc

sortVAT.searchNextStart:
    push hl
    push de
    call sortVAT.compareNames
    pop de
    pop hl
    jr nc, sortVAT.searchNext

sortVAT.searchNextEnd:
    ld bc, 6
    add hl, bc ; goto start of entry

sortVAT.locationFound:
    ex de, hl
    ld a, (hl)
    add a, 7
    ld bc, 6 ; rewind six bytes
    add hl, bc ; a = number of bytes to move
    ld c, a ; hl -> bytes to move
    ld (sortVatEntrySize), bc ; de -> move to location
    ld (sortVatEntryNewLoc), de
    push de
    push hl
    or a, a
    sbc hl, de
    pop hl
    pop de
    jr z, sortVAT.noMoveNeeded
    push hl
    ld de, sortVatEntryTempEnd
    lddr ; copy entry to move to sortVatEntryTempEnd
    ld hl, (sortVatEntryNewLoc)
    pop bc
    push bc
    or a, a
    sbc hl, bc
    push hl
    pop bc
    pop hl
    inc hl
    push hl
    ld de, (sortVatEntrySize)
    or a, a
    sbc hl, de
    ex de, hl
    pop hl
    ldir
    ld hl, sortVatEntryTempEnd
    ld bc, (sortVatEntrySize)
    ld de, (sortVatEntryNewLoc)
    lddr
    ld hl, (sortEndOfPartPtr)
    ld bc, (sortVatEntrySize)
    or a, a
    sbc hl, bc
    ld (sortEndOfPartPtr), hl
    pop hl ; pointer to continue from
    jp sortVAT.sortNext ; to skip name and rest of entry

sortVAT.noMoveNeeded:
    pop hl
    ld (sortEndOfPartPtr), hl
    jp sortVAT.sortNext

sortVAT.skipToNext:
    ld bc, -6
    add hl, bc
    call sortVAT.skipName
    jr sortVAT.findNextItem ; look for next item

sortVAT.skipName:
    ld bc, 0
    ld c, (hl) ; number of bytes in name
    inc c ; to get pointer to data type byte of next entry
    or a, a ; reset carry flag
    sbc hl, bc
    ret

sortVAT.compareNames: ; hl and de pointers to strings output=carry if de is first
    ld b, (hl)
    ld a, (de)
    ld c, 0
    cp a, b ; check if same length
    jr z, sortVAT.hlLonger
    jr nc, sortVAT.hlLonger ; b = smaller than a
    inc c ; to remember that b was larger
    ld b, a ; b was larger than a

sortVAT.hlLonger:
    push bc
    ld b, 64
    dec hl
    dec de
    ld a, (hl)
    cp a, b
    jr nc, sortVAT.firstNotHidden ; check if files are hidden
    add a, b

sortVAT.firstNotHidden:
    ld c, a
    ld a, (de)
    cp a, b
    jr nc, sortVAT.secondNotHidden
    add a, b

sortVAT.secondNotHidden:
    cp a, c
    pop bc
    jr sortVAT.start

sortVAT.loop:
    dec hl
    dec de
    ld a, (de)
    cp a, (hl)

sortVAT.start:
    ret nz
    djnz sortVAT.loop
    dec c
    ret nz
    ccf
    ret

sortVAT.findNextItem: ; carry = found, nc = notfound
    ex de, hl
    ld hl, (ti.pTemp)
    or a, a ; reset carry flag
    sbc hl, de
    ret z
    ex de, hl ; load progptr into hl
    ld a, (hl)
    and a, $1F ; mask out state bytes
    push hl
    ld hl, fileSystem_sortTypes
    ld bc, fileSystem_sortTypes.length
    cpir
    pop hl
    jr nz, sortVAT.skipToNext ; skip to next entry
    dec hl ; add check for folders here if needed
    dec hl
    dec hl ; to pointer
    ld e, (hl)
    dec hl
    ld d, (hl) ; pointer now in de
    dec hl
    ld a, (hl) ; high byte now in a
    dec hl ; add check: do I need to sort this program (not necessary)
    scf
    ret

fileSystem_sortTypes:
    db ti.ProgObj, ti.ProtProgObj, ti.AppVarObj
.equ fileSystem_sortTypes.length, $ - fileSystem_sortTypes

;------------------------------------------------

_asm_fileSystem_initPtrArrays:
    pop hl
    pop bc
    pop de
    ex (sp), iy
    push de
    push bc
    push hl
    lea hl, iy + 0
    add hl, de
    add hl, bc
    ld de, ti.pixelShadow2 - ti.pixelShadow + 1
    or a, a
    sbc hl, de
    jr nc, initPtrArrays.error
    ld iy, 0
    add iy, sp
    push bc
    pop hl
    add hl, hl
    add hl, bc
    ld de, ti.pixelShadow
    add hl, de
    ex de, hl
    ld hl, (iy + 12)
    ld (hl), de
    ld hl, (iy + 6)
    push hl
    pop bc
    add hl, hl
    add hl, bc
    add hl, de
    ex de, hl
    ld hl, (iy + 15)
    ld (hl), de
    ret

initPtrArrays.error:
    ld a, ti.E_Memory
    ld (ti.errNo), a
    call _gfx_End

_asm_fileSystem_error:
    call _asm_utils_clrScrnAndUsedRAM
    ld iy, ti.flags
    call ti.DrawStatusBar
    call _asm_utils_dispQuitErr

error.waitLoop:
    call ti.GetCSC
    cp a, ti.sk1
    jr z, $ + 4
    cp a, ti.skEnter
    jp z, _exit_sp - 4 ; exit the app
    jr error.waitLoop

_asm_fileSystem_getProgramPtrs:
    ld iy, 0
    add iy, sp
    ld b, (iy + 6) ; ignore hidden programs?
    ld hl, (iy + 9) ; search string pointer
    ld iy, (iy + 3) ; get array
    res 1, b
    xor a, a
    or a, (hl)
    jr z, $ + 4
    set 1, b ; z to not search, nz if we do search
    push hl
    ld hl, (ti.progPtr)

getProgramPtrs.loop:
    ld de, (ti.pTemp)
    or a, a
    sbc hl, de
    jr z, getProgramPtrs.return
    jr c, getProgramPtrs.return
    add hl, de
    ld a, (hl)
    and a, $1F
    cp a, ti.ProgObj
    jr z, getProgramPtrs.isProgram
    cp a, ti.ProtProgObj
    jr nz, getProgramPtrs.skipEntry

getProgramPtrs.isProgram:
    ld de, -7
    push hl
    add hl, de
    ld a, (hl)
    pop hl
    bit 0, b
    jr nz, getProgramPtrs.loadAddress
    cp a, ti.tA
    jr c, getProgramPtrs.skipEntry

getProgramPtrs.loadAddress:
    call _asm_utils_checkHiddenHeader
    jr z, getProgramPtrs.skipEntry
    pop de
    bit 1, b
    call nz, fileSystem_checkString
    push de
    jr nz, getProgramPtrs.skipEntry
    ld (iy), hl
    lea iy, iy + 3

getProgramPtrs.skipEntry:
    ld de, -6
    add hl, de
    ld a, (hl)
    inc a
    neg
    ld e, a
    add hl, de
    jr getProgramPtrs.loop

getProgramPtrs.return:
    pop de
    ret

_asm_fileSystem_getAppVarPtrs:
    pop de
    pop iy
    ex (sp), hl
    push hl
    push de
    res 1, b
    xor a, a
    or a, (hl)
    jr z, $ + 4
    set 1, b ; z to not search, nz if we do search
    push hl
    ld hl, (ti.progPtr)

getAppVarPtrs.loop:
    ld de, (ti.pTemp)
    or a, a
    sbc hl, de
    jr z, getProgramPtrs.return
    jr c, getProgramPtrs.return
    add hl, de
    ld a, (hl)
    and a, $1F
    cp a, ti.AppVarObj
    jr nz, getAppVarPtrs.skipEntry
    pop de
    bit 1, b
    call nz, fileSystem_checkString
    push de
    jr nz, getAppVarPtrs.skipEntry
    ld (iy), hl
    lea iy, iy + 3

getAppVarPtrs.skipEntry:
    ld de, -6
    add hl, de
    ld a, (hl)
    inc a
    neg
    ld e, a
    add hl, de
    jr getAppVarPtrs.loop

_asm_fileSystem_findAllVars:
    ld iy, 0
    add iy, sp
    ld b, (iy + 9) ; show hidden programs
    ld hl, (iy + 12) ; search string pointer
    push ix
    res 1, b
    xor a, a
    or a, (hl)
    jr z, $ + 4
    set 1, b ; z to not search, nz if we do search
    push hl
    ld ix, 0 ; programs
    lea iy, ix + 0 ; appvars
    ld hl, (ti.progPtr)

findAllVars.loop:
    ld de, (ti.pTemp)
    or a, a
    sbc hl, de
    jr z, findAllVars.return
    jr c, findAllVars.return
    add hl, de
    pop de
    bit 1, b
    call nz, fileSystem_checkString
    push de
    jr nz, findAllVars.skipEntry
    ld a, (hl)
    and a, $1F
    cp a, ti.AppVarObj
    jr z, findAllVars.countAppVar
    cp a, ti.ProgObj
    jr z, findAllVars.countProgram
    cp a, ti.ProtProgObj
    jr nz, findAllVars.skipEntry

findAllVars.countProgram:
    ld de, -7
    push hl
    add hl, de
    ld a, (hl)
    pop hl
    cp a, ti.tA
    jr nc, findAllVars.visible
    bit 0, b
    jr z, findAllVars.skipEntry

findAllVars.visible:
    call _asm_utils_checkHiddenHeader
    jr z, findAllVars.skipEntry
    inc ix
    jr findAllVars.skipEntry

findAllVars.countAppVar:
    inc iy

findAllVars.skipEntry:
    ld de, -6
    add hl, de
    ld a, (hl)
    inc a
    neg
    ld e, a
    add hl, de
    jr findAllVars.loop

findAllVars.return:
    pop de
    lea bc, ix + 0
    lea de, iy + 0
    pop ix
    pop iy
    pop hl
    ld (hl), bc
    pop hl
    ld (hl), de
    push hl
    push hl
    jp (iy)

fileSystem_checkString:
    push hl
    push bc
    ld bc, -7
    add hl, bc
    ex de, hl
    push hl
    call ti.StrLength
    ld a, (de)
    cp a, ti.tA
    jr nc, checkString.loop
    xor a, 64

checkString.loop:
    cpi
    dec de
    jr nz, checkString.return
    ld a, b
    or a, c
    ld a, (de)
    jr nz, checkString.loop

checkString.return:
    pop de
    pop bc
    pop hl
    ret

_asm_fileSystem_findArrayOffset:
    pop hl
    pop de ; ptr
    ex (sp), iy ; ptrs
    push de
    push hl
    or a, a
    sbc hl, hl

findArrayOffset.loop:
    ld bc, (iy)
    ex de, hl
    or a, a
    sbc hl, bc
    add hl, bc
    ex de, hl
    ret z
    inc hl
    lea iy, iy + 3
    jr findArrayOffset.loop
