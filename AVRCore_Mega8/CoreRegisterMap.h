#pragma once
using namespace System;

public ref class CoreRegisterMap
{
public:
	CoreRegisterMap() {}

	//String^ getRegisterFunctionalInfo(int addr);

	// Two-wire Serial Interface Bit Rate Register
	const static int TWBR = 0x20;

	const static int TWSR = 0x21;
	const static int TWAR = 0x22;

	// Two-wire Serial Interface Data Register
	const static int TWDR = 0x23;

	// ADC Data Register
	const static int ADCL = 0x24;
	const static int ADCH = 0x25;

	const static int ADCSRA = 0x26;
	const static int ADMUX = 0x27;
	const static int ACSR = 0x28;

	// USART BoudRate LowByte
	const static int UBRRL = 0x29;

	const static int UCSRB = 0x2A;
	const static int UCSRA = 0x2B;
	const static int UDR = 0x2C;

	const static int SPCR = 0x2D;
	const static int SPSR = 0x2E;
	const static int SPDR = 0x2F;

	// Port D Registers
	const static int PIND = 0x30;
	const static int DDRD = 0x31;
	const static int PORTD = 0x32;

	// Port C Registers
	const static int PINC = 0x33;
	const static int DDRC = 0x34;
	const static int PORTC = 0x35;

	// Port B Registers
	const static int PINB = 0x36;
	const static int DDRB = 0x37;
	const static int PORTB = 0x38;

	const static int RESERVED_1 = 0x39;
	const static int RESERVED_2 = 0x3A;
	const static int RESERVED_3 = 0x3B;

	const static int EECR = 0x3C;
	const static int EEDR = 0x3D;

	// EEPROM Register
	const static int EEARL = 0x3E;
	const static int EEARH = 0x3F;

	const static int UCSRC = 0x40;
	const static int UBRRH = 0x40;

	// Watchdog Timer Control Register
	const static int WDTCR = 0x41;

	const static int ASSR = 0x42;

	// Timer / Counter 2 Compare Register
	const static int OCR2 = 0x43;

	// Timer / Counter 2 
	const static int TCNT2 = 0x44;

	const static int TCCR2 = 0x45;

	// Timer / Counter 1 - Input Capture Register
	const static int ICR1L = 0x46;
	const static int ICR1H = 0x47;

	// Timer / Counter 1 - Output Compare Register B
	const static int OCR1BL = 0x48;
	const static int OCR1BH = 0x49;

	// Timer / Counter 1 - Output Compare Register A
	const static int OCR1AL = 0x4A;
	const static int OCR1AH = 0x4B;

	// Timer / Counter 1 - Counter Register
	const static int TCNT1L = 0x4C;
	const static int TCNT1H = 0x4D;

	const static int TCCR1B = 0x4E;
	const static int TCCR1A = 0x4F;

	// Special Function IO Register
	const static int SFIOR = 0x50;

	// Oscillator Calibration Register
	const static int OSCCAL = 0x51;

	// Timer / Counter 0 
	const static int TCNT0 = 0x52;

	const static int TCCR0 = 0x53;

	// MCU Control & Status Register
	const static int MCUCSR = 0x54;

	// MCU Control Register
	const static int MCUCR = 0x55;

	const static int TWCR = 0x56;
	const static int SPMCR = 0x57;
	const static int TIFR = 0x58;
	const static int TIMSK = 0x59;

	const static int GIFR = 0x5A;

	// General Interrupt Control Register
	const static int GICR = 0x5B;

	const static int RESERVED_4 = 0x5C;

	// Stack Pointer
	const static int SPL = 0x5D;
	const static int SPH = 0x5E;

	// Flag Register
	const static int SREG = 0x5F;

	// Special non file registers
	const static int REG_PC = 0xFFFE;

};


