/*
 * debugProject_6.asm
 *
 *  Created: 12.01.2016 12:25:57
 *   Author: Diman
 */ 


 .org 0x000
 RJMP Reset
 RJMP Ext0
 RJMP Ext1


 .org 0x0030

 Reset:
	LDI r16, low(RAMEND)
	OUT spl, r16
	LDI r16, high(RAMEND)
	OUT sph, r16

	CLR r16
	ORI r16, 0xC0
	IN r17, gicr
	OR r17, r16
	OUT gicr, r17
	
	CLR r16
	ORI r16, 0x05
	IN r17, mcucr
	OR r17, r16
	OUT mcucr, r17

	SEI
	RJMP Main

Ext0:
	LDI r21, 0xAA
	RETI

Ext1:
	LDI r22, 0xBB
	RETI

Main:
	RJMP Main