/*
 * debugProject_5.asm
 *
 *  Created: 04.01.2016 12:08:03
 *   Author: Diman
 */ 


 .org 0x000
 Reset:
	LDI r16, low(RAMEND)
	OUT spl, r16
	LDI r16, high(RAMEND)
	OUT sph, r16

	CLR r16
	CLR r17

	OUT DDRB, r16
	OUT DDRC, r16
	RJMP Main

.org 0x020
Main:
	IN r16, PINB
	IN r17, PINC

	RJMP Main