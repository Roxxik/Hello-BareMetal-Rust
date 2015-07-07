; entry point to c compatible languages
; Written by Stefan Nitz
;
; BareMetal compile:
; nasm start.asm -o start.o
; then link to your objectfile(s)


[BITS 64]

; %INCLUDE "bmdev.asm"

extern main

start:                 ; Start of program label
    call main
ret                    ; Return to OS
