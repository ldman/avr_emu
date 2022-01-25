/*
 * testFactory.asm
 *
 *  Created: 02.10.2015 18:39:36
 *   Author: Diman
 */ 


 .ORG 0x0000
 RJMP Reset

 .ORG 0x0009
 Reset:
	LDI r16, low(RAMEND)
	OUT spl, r16
	LDI r16, high(RAMEND)
	OUT sph, r16
	RJMP Main

Main:

End:
	RJMP End