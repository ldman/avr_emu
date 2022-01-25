/*
 * debugProject.asm
 *
 *  Created: 07.09.2015 22:36:56
 *   Author: Diman
 */ 

 ; VAR 1

 .org 0x0000
	RJMP Reset

.org 0x0009
Reset:
	LDI	r16,low(RAMEND)
	OUT spl,r16
	LDI r16,high(RAMEND)
	OUT sph,r16
	RJMP MAIN


	.EQU con_A = 8
	.EQU con_B = 3
	;var_K = r20
	;var_X = r16
	;answer = r30
	;divided = r24
	;divider = r25
	;inverted = r23

MAIN:
	; Load vars and calc X 
	LDI r20, 5 ; var_k <- 5
	RCALL CALC_X
	CP	r16, r20 ; var_x <> var_k
	BRLT CALC_V2
	RJMP CALC_V1

END:
	RJMP END


CALC_X:
	LDI	r18, con_A
	LDI r19, con_B
	ASR r18
	ADD r18, r19
	MOV r16, r18
	RET

CALC_V1:
	LDI r20, 9 ; var_k <- 9
	MULSU r16, r20 ; var_x * var_k
	MOV r16, r0

	LDI r24, con_A
	LDI r25, con_B
	RCALL DIV

	SUB r16, r24
	MOV r30, r16
	RJMP END

CALC_V2:
	MULS r16, r16
	MOV r24, r16
	LDI r25, con_B
	RCALL DIV
	
	LDI r18, con_A
	ADD r24, r18
	MOV r30, r24
	RJMP END


; DIV signed
DIV:
	; check A for neg
	EOR r23, r23 ; clear inverted
	SBRS r24, 7 ; if A < 0 set flag and neg 
	RJMP _tst_B ; if A > 0 test B
	SBR r23, 0 ; set flag that A is NEG
	LDI r24, con_A
	NEG r24
_tst_B: ; check B for neg
	SBRS r25, 7
	RJMP _div ; if B < 0 set flag and neg
	SBR r23, 1
	LDI r25, con_B
	NEG r25
	; if A and B is neg clear flags
	CPI r23, 3
	BRNE _div
	EOR r23, r23
_div:
	CP r24, r25
	BRSH DIV_PROC
	TST r23
	BRBC 1, _div_end
	NEG r24
_div_end:
	RET
DIV_PROC:
	SUB r24, r25
	RJMP DIV
