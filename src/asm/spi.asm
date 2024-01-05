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
    ld hl, $02000B
    ld ($F80004), hl
    ld hl, $001828
    ld (ti.mpSpiRange), hl
    ld hl, $00000C
    ld ($F80008), hl
    nop
    ld hl, $000040
    ld ($F80008), hl
    push af
    call ti.Delay10ms
    ;call $0061C2
    ;call $0061C2
    ;call $0061C2
    ld hl, $00182B
    ld (ti.mpSpiRange), hl
    ld hl, $00000C
    ld ($F80008), hl
    nop
    ld hl, $000040
    ld ($F80008), hl
    ;nop
    ;nop
    ;nop
    call ti.Delay10ms
    pop af
    ld hl, $000021
    ld ($F80010), hl
    ld hl, $000100
    ld ($F80008), hl
    ;ld a, ($F80014)
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
