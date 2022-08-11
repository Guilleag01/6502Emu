    ; Multiplicar los valores de los registros X e Y y dejar el resultado en el acumulador
;     ldx #3
;     ldy #5
;     stx $200
;     sty $201
;     ldx #0
;     lda #0
; :loop
;     lsr $200
;     bcc salto
;     clc 
;     adc $201
; :salto
;     rol $201
;     inx 
;     cpx #4
;     bne loop
;     brk 

lda #3
ror
ror