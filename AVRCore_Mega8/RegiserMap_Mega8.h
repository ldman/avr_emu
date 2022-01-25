
#ifndef REGISTERMAP_MEGA8
#define REGISTERMAP_MEGA8

#define byte unsigned char
#define word unsigned short

// I/O Register File Boundaries
#define REGISTERS_BEGIN 0x020
#define REGISTERS_END 0x060

// General Bitmap
#define BIT_0 0x01
#define BIT_1 0x02
#define BIT_2 0x04
#define BIT_3 0x08
#define BIT_4 0x10
#define BIT_5 0x20
#define BIT_6 0x40
#define BIT_7 0x80


// Flags in SREG
#define SREG_C 0x01 // Carry
#define SREG_Z 0x02 // Zero
#define SREG_N 0x04 // Negative
#define SREG_V 0x08 // Two's Complement Overflow
#define SREG_S 0x10 // Sign Bit
#define SREG_H 0x20 // Half Cary Flag
#define SREG_T 0x40 // Bit Copy Storage
#define SREG_I 0x80 // Global Interrup Enable


typedef struct {
	byte *bSP;
	word *wSP;
} StackPointer;

typedef struct {
	byte *R;
	word *X_register;
	word *Y_register;
	word *Z_register;

	// Part of I/O Registers space
	byte *sreg;
	StackPointer sp;

	byte *IORegisters; // 0x20-0x60 for MEGA8
	byte *InternalRam; // Internal SRAM starts from 0x60 for MEGA8		

	byte *ExtendedIORegisters; // Not implemented in MEGA8

	// Special MCU Registers
	word ProgramCounter;
} CPURegisterMap;

#define FRONT_RIS 1
#define FRONT_FAL -1
#define FRONT_NO 0

typedef struct {
	byte *portDDR;
	byte *portDATA;
	byte *portPIN;
	byte memPIN;
} PortSupervisor;

#define NO_CLOCK_SOURCE 0x00
#define NO_PRESCALE 0x01
#define PRESCALE_8 0x02
#define PRESCALE_64 0x03
#define PRESCALE_256 0x04
#define PRESCALE_1024 0x05
#define EXT_T0_FAL 0x06
#define EXT_T0_RIS 0x07

typedef struct {
	byte *timerReg;
	byte *timerControlReg;
} TimerCounterType0;

typedef struct {
	word *timerReg;
	word *timerControlReg;
	word *inputCaptureReg;
	word *outputCompareRegA;
	word *outputCompareRegB;
	byte *timerControlRegA;
	byte *timerControlRegB;
} TimerCounterType1;

typedef struct {
	byte *interruptMaskReg;
	byte *interruptFlagReg;
	word  prescalerClock;
	byte *specialFuncReg;
} TimerController;

/* ADC */

//ADC Params
#define MAX_ADC_VOLTAGE 5.
#define MAX_ADC_CHNS 6

// ADMUX : MUX
#define ADC_CH_0 0x00
#define ADC_CH_1 0x01
#define ADC_CH_2 0x02
#define ADC_CH_3 0x03
#define ADC_CH_4 0x04
#define ADC_CH_5 0x05
#define ADC_CH_6 0x06
#define ADC_CH_7 0x07
#define ADC_CH_REF 0x0E
#define ADC_CH_GND 0x0F

// ADMUX : REFS
#define REFS_AREF 0x0
#define REFS_VCC 0x1
#define REFS_INT_2_56V 0x3

#define ADMUX_ADLAR_MASK 0x10

// ADC Control
#define ADCSRA_ADEN_MASK 0x80 // Enable ADC
#define ADCSRA_ADSC_MASK 0x40 // ADC Start conversion
#define ADCSRA_ADFR_MASK 0x20 // ADC Free runing select
#define ADCSRA_ADIF_MASK 0x10 // ADC Interupt flag
#define ADCSRA_ADIE_MASK 0x08 // ADC Interupt enable

// ADCSRA : ADPS
// ADC Prescaler bits
// Affect's precision of ADC, not used now
#define ADCSRA_ADPS_D2 0x00
#define ADCSRA_ADPS_D2 0x01
#define ADCSRA_ADPS_D4 0x02
#define ADCSRA_ADPS_D8 0x03
#define ADCSRA_ADPS_D16 0x04
#define ADCSRA_ADPS_D32 0x05
#define ADCSRA_ADPS_D64 0x06
#define ADCSRA_ADPS_D128 0x07

#define ADC_PRECISION_BITS 10
#define ADC_PRECISION_VAL 1024
#define ADC_CONVERSION_TICKS 30

// SFIOR : ADTS
// ADC Conversion control & timer logic bind
#define SFIOR_ADTS_MASK 0x07
#define SFIOR_ADTS_NOCOND 0x00 // Run always
#define SFIOR_ADTS_ANALOG 0x01 // Analog comparator run
#define SFIOR_ADTS_INT0 0x02 // Run if EXT0
#define SFIOR_ADTS_T0_MATCH 0x03 // T0 match
#define SFIOR_ADTS_T0_OVRFL 0x04 // T0 overflow
#define SFIOR_ADTS_T1_MATCH 0x05 // T1 match
#define SFIOR_ADTS_T1_OVRFL 0x06 // T1 overflow
#define SFIOR_ADTS_T1_CAPTR 0x07 // T1 capture

typedef struct {
	bool isEnabled;
	bool useRef;
	bool useGND;
	int conversionKoef; // value for conversion fron A to D
	int conversionTimer;

	// ADC channel values & logic 
	double adcCHN[MAX_ADC_CHNS];
	byte selectedCHN;
	
	// ADC Result registers
	byte* ADCH;
	byte* ADCL;
	byte* ADMUX;
} AdcSupervisor;

/* INT */

#define INT_RESET_ADDR 0x000 // Reset
#define INT_EXT0_ADDR 0x001 // EXT0 pin
#define INT_EXT1_ADDR 0x002 // EXT1 pin
#define INT_TIMER2_COMP_ADDR 0x003 // Timer2 compare match
#define INT_TIMER2_OVF_ADDR 0x004 // Timer2 overflow
#define INT_TIMER1_CAPT_ADDR 0x005 // Timer1 capture event
#define INT_TIMER1_COMPA_ADDR 0x006 // Timer1 compare match A
#define INT_TIMER_COMPB_ADDR 0x007 // Timer1 compare match B
#define INT_TIMER1_OVF_ADDR 0x008 // Timer1 overflow
#define INT_TIMER0_OVF_ADDR 0x009 // Timer0 overflow
#define INT_SPI_STC_ADDR 0x00A // Serial transfer complete
#define INT_USART_RXC_ADDR 0x00B // Rx complete
#define INT_USART_UDRE_ADDR 0x00C // USART data register empty
#define INT_USART_TXC_ADDR 0x00D // USART, Tx complete
#define INT_ADC_ADDR 0x00E // ADC converion complete
#define INT_EE_RDY_ADDR 0x00F // EEPROM ready
#define INT_ANA_COMP_ADDR 0x010 // Analog comparator
#define INT_TWI_ADDR 0x011 // Two-wire serial interface
#define INT_EXT2_ADDR 0x012 // EXT2 pin
#define INT_TIMER0_COMP_ADDR 0x013 // Timer0 compare match
#define INT_SPM_RDY_ADDR 0x014 // Store program memory ready


#define INT_INT1 0x80
#define INT_INT0 0x40

#define INT_INT0_PIN_NO 2
#define INT_INT0_PIN_MASK 0x04

#define INT_INT1_PIN_NO 3
#define INT_INT1_PIN_MASK 0x08

#define INT_EXT_SIG_LOW 0
#define INT_EXT_SIG_CHG 1
#define INT_EXT_SIG_FAL 2
#define INT_EXT_SIG_RIS 3

#define GIFR_EXT0_MASK 0x40
#define GIFR_EXT1_MASK 0x80

typedef struct {

	bool isExt0Enabled;
	bool isExt0Exec;

	bool isExt1Enabeld;
	bool isExt1Exec;

	bool isAdcEnabled;
	bool isAdcExec;
	
	bool isINTEnabeld;

	// Ext Interupts Registers
	byte *MCUCR; // Control register
	byte *GICR;
	byte *GIFR; // INT Flag registers
	byte *ADCSRA; // For ADC flag ADIF
} IRCSupervisor;

#endif