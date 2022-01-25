/*
 * debugProject_5.asm
 *
 *  Created: 07.12.2015 13:03:44
 *   Author: Diman
 */ 


 .org 0x000
 Reset:
	LDI r16, high(RAMEND)
	OUT SPH, r16
	LDI r16, low(RAMEND)
	OUT SPL, r16
	RJMP Main


.org 0x009
Main:
	LDI r16, 0xFF
	OUT DDRB, r16
	OUT DDRC, r16 ; must be 0x3F
	OUT DDRD, r16

	LDI r16, 0xBB
	OUT PORTB, r16

	LDI r16, 0xCC
	OUT PORTC, r16 ; must be 0x0C

	LDI r16, 0xDD
	OUT PORTD, r16

End:
	RJMP End