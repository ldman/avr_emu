/*
 * debugProject_7.asm
 *
 *  Created: 01.02.2016 19:07:37
 *   Author: Diman
 */ 

 .include "m8def.inc"

.MACRO outi			
LDI 	R16,@1 			
OUT 	@0,R16 	
.ENDM

 .org 0x000
 RJMP Reset
 .org ADCCaddr  ;.org 0x00E
 RJMP INT_ADC

 .org 0x040
 Reset:
	LDI r16, high(RAMEND)
	OUT sph, r16
	LDI r16, low(RAMEND)
	OUT spl, r16
	CLR r16
	SEI
	RJMP Main

INT_ADC:
	IN r20, ADCH
	RETI

Main:
; ADC Init
	OUTI ADCSRA,(1<<ADEN)|(1<<ADIE)|(1<<ADSC)|(1<<5)|(3<<ADPS0)
	OUTI ADMUX,0b01000101 ; 5-th chan
_Loop:
	RJMP _Loop
