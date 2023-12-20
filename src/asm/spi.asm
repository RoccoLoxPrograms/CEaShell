;--------------------------------------
;
; CEaShell Source Code - spi.asm
; By RoccoLox Programs and TIny_Hacker
; Some code from: https://github.com/Zaalan3/AnotherWorldCE/blob/main/src/spi.asm
; Copyright 2022 - 2023
; License: GPL-3.0
;
;--------------------------------------

    assume adl=1

    section .text

include 'include/equates.inc'

    public _asm_spi_beginFrame
    public _asm_spi_endFrame
    public _asm_spi_setupSPI

;--------------------------------------

macro spi cmd, params&
    ld a, cmd
    call spi_spiCmd
    match any, params
        iterate param, any
            ld a, param
            call spi_spiParam
        end iterate
    end match
end macro

;--------------------------------------

_asm_spi_beginFrame:
    ld a, (ti.mpLcdRis)
    and a, ti.lcdIntVcomp
    jr z, _asm_spi_beginFrame
    ld (ti.mpLcdIcr), a
    spi $B0, $01 ; disable framebuffer copies
    spi $2C
    ret

_asm_spi_endFrame:
    ld a, (ti.mpLcdCurr + 2) ; a = *mpLcdCurr >> 16
    ld hl, (ti.mpLcdCurr + 1) ; hl = *mpLcdCurr >> 8
    sub a, h
    jr nz, _asm_spi_endFrame ; nz ==> lcdCurr may have updated mid-read; retry read
    ld de, -ti.lcdWidth * ti.lcdHeight
    add hl, de
    ld de, (ti.mpLcdBase)
    or a, a
    sbc hl, de
    or a, a
    sbc hl, de
    jr z, .resetVcomp
    ld a, ti.lcdIntVcomp
    ld (ti.mpLcdIcr), a

.loop:
    ld a, (ti.mpLcdRis)
    bit ti.bLcdIntVcomp, a
    jr z, .loop

.resetVcomp:
    ld a, ti.lcdIntVcomp
    ld (ti.mpLcdIcr), a
    spi $B0, $11 ; enable framebuffer copies
    ret

_asm_spi_setupSPI: ; set these defaults for the SPI so everything works on Python models (this seems to work instead of using boot.InitializeHardware)
    scf
    ld hl, $2000B
    ld (ti.mpSpiRange + ti.spiCtrl1), hl
    ld hl, $1828

.loop:
    ld (ti.mpSpiRange + ti.spiCtrl0), hl
    ld hl, $0C
    ld (ti.mpSpiRange + ti.spiCtrl2), hl
    ccf
    ld hl, $40
    ld (ti.mpSpiRange + ti.spiCtrl2), hl
    ld hl, $182B
    jr nc, .loop
    ld hl, $21
    ld (ti.mpSpiRange + ti.spiIntCtrl), hl
    ld hl, $100
    ld (ti.mpSpiRange + ti.spiCtrl2), hl
    ret

;--------------------------------------

spi_spiParam:
    scf
    virtual
        jr nc, $
        load .jr_nc : byte from $$
    end virtual
    db .jr_nc

spi_spiCmd:
    or a, a
    ld hl, ti.mpSpiData or spiValid shl 8
    ld b, 3

.loop:
    rla
    rla
    rla
    ld (hl), a
    djnz .loop
    ld l, h
    ld (hl), 1

.wait:
    ld l, ti.spiStatus + 1

.wait1:
    ld a, (hl)
    and a, $F0
    jr nz, .wait1
    dec l

.wait2:
    bit 2, (hl)
    jr nz, .wait2
    ld l, h
    ld (hl), a
    ret
