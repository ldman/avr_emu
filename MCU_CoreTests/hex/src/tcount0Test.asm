/*
 * debugProject_4.asm
 *
 *  Created: 30.11.2015 11:26:06
 *   Author: Diman
 */ 


 .org 0x000
 Reset:
	LDI r16, high(RAMEND)
	OUT SPH, r16
	LDI r16, low(RAMEND)
	OUT SPL, r16
	CLR r16
	RJMP Main


.org 0x009
Main:
	CLR r16
	OUT TCNT0, r16
	SBR r16, 1
	OUT TCCR0, r16
	RCALL LongProc
	CLR r16
	OUT TCCR0, r16
	IN r16, TCNT0

End:
	RJMP End
	; R16: 0x05
	; R19: 0x01
	; R20: 0x01

LongProc:
	LDI r20, 0xFF
	LDI r19, 1
_procLoop:
	CP r19, r20
	BREQ _procEnd
	DEC r20
	RJMP _procLoop
_procEnd:
	RET