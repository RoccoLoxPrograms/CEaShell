;--------------------------------------
;
; CEaShell Source Code - spi.s
; By RoccoLox Programs and TIny_Hacker
; Some code from: https://github.com/Zaalan3/AnotherWorldCE/blob/main/src/spi.asm
; Copyright 2022 - 2026
; License: GPL-3.0
;
;--------------------------------------

    .assume adl=1

    .include "src/asm/include/equates.inc"

    .global _asm_spi_beginFrame
    .type   _asm_spi_beginFrame, @function
    .global _asm_spi_endFrame
    .type   _asm_spi_endFrame, @function
    .global _asm_spi_setupSPI
    .type   _asm_spi_setupSPI, @function

;--------------------------------------

    .section .text

;--------------------------------------

.macro spi cmd, params:vararg
    ld a, \cmd
    call spi_spiCmd
    .ifnb \params
        .irp param, \params
            ld a, \param
            call spi_spiParam
        .endr
    .endif
.endm

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
    jr z, endFrame.resetVcomp
    ld a, ti.lcdIntVcomp
    ld (ti.mpLcdIcr), a

endFrame.loop:
    ld a, (ti.mpLcdRis)
    bit ti.bLcdIntVcomp, a
    jr z, endFrame.loop

endFrame.resetVcomp:
    ld a, ti.lcdIntVcomp
    ld (ti.mpLcdIcr), a
    spi $B0, $11 ; enable framebuffer copies
    ret

_asm_spi_setupSPI: ; set these defaults for the SPI so everything works on Python models (this seems to work instead of using boot.InitializeHardware)
    ld hl, $2000B
    ld (ti.mpSpiRange + ti.spiCtrl1), hl
    ld hl, $1828
    ld (ti.mpSpiRange), hl
    ld hl, $0C
    ld (ti.mpSpiRange + ti.spiCtrl2), hl
    nop
    ld hl, $40
    ld (ti.mpSpiRange + ti.spiCtrl2), hl
    call ti.Delay10ms
    ld hl, $182B
    ld (ti.mpSpiRange), hl
    ld hl, $0C
    ld (ti.mpSpiRange + ti.spiCtrl2), hl
    nop
    ld hl, $40
    ld (ti.mpSpiRange + ti.spiCtrl2), hl
    call ti.Delay10ms
    ld hl, $21
    ld (ti.mpSpiRange + ti.spiIntCtrl), hl
    ld hl, $100
    ld (ti.mpSpiRange + ti.spiCtrl2), hl
    ret

;--------------------------------------

spi_spiParam:
    scf
    db 0x30

spi_spiCmd:
    or a, a
    ld hl, ti.mpSpiData | (spiValid << 8)
    ld b, 3

spiCmd.loop:
    rla
    rla
    rla
    ld (hl), a
    djnz spiCmd.loop
    ld l, h
    ld (hl), 1

spiCmd.wait:
    ld l, ti.spiStatus + 1

spiCmd.wait1:
    ld a, (hl)
    and a, $F0
    jr nz, spiCmd.wait1
    dec l

spiCmd.wait2:
    bit 2, (hl)
    jr nz, spiCmd.wait2
    ld l, h
    ld (hl), a
    ret
