#include "stdafx.h"
#include "AVRSystem.h"


AVRSystem::AVRSystem()
{	
	// Reset all memory to 0 and set read pointers
	reset();
	pProgMemory = progMem;
	pRamMemory = ramMem;
	pEepromMemory = eepromMem;
	
	// General Registers initialization
	generalRegisters.R = ramMem;
	generalRegisters.X_register = (unsigned short*)&ramMem[27];
	generalRegisters.Y_register = (unsigned short*)&ramMem[29];
	generalRegisters.Z_register = (unsigned short*)&ramMem[31];
	generalRegisters.sp.wSP = (unsigned short*)&ramMem[CoreRegisterMap::SPL];
	generalRegisters.sreg = &ramMem[CoreRegisterMap::SREG];
	generalRegisters.ProgramCounter = 0x000;
	
	// Memory Boundares Initialization
	generalRegisters.IORegisters = &ramMem[REGISTERS_BEGIN];	
	generalRegisters.InternalRam = &ramMem[REGISTERS_END]; // AtMega8 not support Extended IO

	// Stack Pointer initialization
	generalRegisters.sp.wSP[0] = 0xFFFF; 

	usedProgramMemory = 0;

	// Cycles used to emulate cmd timing
	voidCmdCyclesCount = 0;

	// Asing supervisors to ports
	SvPORTA.memPIN = 0;
	SvPORTA.portDATA = NULL;
	SvPORTA.portDDR = NULL;
	SvPORTA.portPIN = NULL;

	SvPORTB.memPIN = 0;
	SvPORTB.portDATA = &ramMem[CoreRegisterMap::PORTB];
	SvPORTB.portDDR = &ramMem[CoreRegisterMap::DDRB];
	SvPORTB.portPIN = &ramMem[CoreRegisterMap::PINB];

	SvPORTC.memPIN = 0;
	SvPORTC.portDATA = &ramMem[CoreRegisterMap::PORTC];
	SvPORTC.portDDR = &ramMem[CoreRegisterMap::DDRC];
	SvPORTC.portPIN = &ramMem[CoreRegisterMap::PINC];

	SvPORTD.memPIN = 0;
	SvPORTD.portDATA = &ramMem[CoreRegisterMap::PORTD];
	SvPORTD.portDDR = &ramMem[CoreRegisterMap::DDRD];
	SvPORTD.portPIN = &ramMem[CoreRegisterMap::PIND];

	// Asign timers
	timersController.interruptFlagReg = &ramMem[CoreRegisterMap::TIFR];
	timersController.interruptMaskReg = &ramMem[CoreRegisterMap::TIMSK];
	timersController.specialFuncReg = &ramMem[CoreRegisterMap::SFIOR];

	timer0.timerReg = &ramMem[CoreRegisterMap::TCNT0];
	timer0.timerControlReg = &ramMem[CoreRegisterMap::TCCR0];

	timer2.timerReg = &ramMem[CoreRegisterMap::TCNT2];
	timer2.timerControlReg = &ramMem[CoreRegisterMap::TCCR2];

	timer1.timerReg = (unsigned short*)&ramMem[CoreRegisterMap::TCNT1L];
	timer1.inputCaptureReg = (unsigned short*)&ramMem[CoreRegisterMap::ICR1L];
	timer1.outputCompareRegA = (unsigned short*)&ramMem[CoreRegisterMap::OCR1AL];
	timer1.outputCompareRegB = (unsigned short*)&ramMem[CoreRegisterMap::OCR1BL];
	timer1.timerControlRegA = &ramMem[CoreRegisterMap::TCCR1A];
	timer1.timerControlRegB = &ramMem[CoreRegisterMap::TCCR1B];

	// Asign ADC
	adcSupervisor.isEnabled = false;
	adcSupervisor.useGND = false;
	adcSupervisor.useRef = false;

	adcSupervisor.conversionKoef = ADC_PRECISION_VAL / 5; // Write conversion K for 5V as max possible voltage value;
	adcSupervisor.ADCH = &ramMem[CoreRegisterMap::ADCH];
	adcSupervisor.ADCL = &ramMem[CoreRegisterMap::ADCL];
	adcSupervisor.ADMUX = &ramMem[CoreRegisterMap::ADMUX];


	// Asing INT
	ircSupervisor.isExt0Enabled = false;
	ircSupervisor.isExt0Exec = false;
	ircSupervisor.isExt1Enabeld = false;
	ircSupervisor.isExt1Exec = false;
	ircSupervisor.isINTEnabeld = false;
	ircSupervisor.isAdcEnabled = false;
	ircSupervisor.isAdcExec = false;
	ircSupervisor.MCUCR = &ramMem[CoreRegisterMap::MCUCR];
	ircSupervisor.GIFR = &ramMem[CoreRegisterMap::GIFR];
	ircSupervisor.GICR = &ramMem[CoreRegisterMap::GICR];
	ircSupervisor.ADCSRA = &ramMem[CoreRegisterMap::ADCSRA];
	
	/* disassemble init */
	registerDict = std::vector<std::string>{
		"TWBR\0",
		"TWSR\0",
		"TWAR\0",
		"TWDR\0",
		"ADCL\0",
		"ADCH\0",
		"ADCSRA\0",
		"ADMUX\0",
		"ACSR\0",
		"UBRRL\0",
		"UCSRB\0",
		"UCSRA\0",
		"UDR\0",
		"SPCR\0",
		"SPSR\0",
		"SPDR\0",
		"PIND\0",
		"DDRD\0",
		"PORTD\0",
		"PINC\0",
		"DDRC\0",
		"PORTC\0",
		"PINB\0",
		"DDRB\0",
		"PORTB\0",
		"RESERVED_1\0",
		"RESERVED_2\0",
		"RESERVED_3\0",
		"EECR\0",
		"EEDR\0",
		"EEARL\0",
		"EEARH\0",
		"UCSRC\0",
		"WDTCR\0",
		"ASSR\0",
		"OCR2\0",
		"TCNT2\0",
		"TCCR2\0",
		"ICR1L\0",
		"ICR1H\0",
		"OCR1BL\0",
		"OCR1BH\0",
		"OCR1AL\0",
		"OCR1AH\0",
		"TCNT1L\0",
		"TCNT1H\0",
		"TCCR1B\0",
		"TCCR1A\0",
		"SFIOR\0",
		"OSCCAL\0",
		"TCNT0\0",
		"TCCR0\0",
		"MCUCSR\0",
		"MCUCR\0",
		"TWCR\0",
		"SPMCR\0",
		"TIFR\0",
		"TIMSK\0",
		"GIFR\0",
		"GICR\0",
		"RESERVED_4\0",
		"SPL\0",
		"SPH\0",
		"SREG\0"
	};


	softReset();
}

AVRSystem::~AVRSystem()
{
}


/* MEMORY OPERATIONS */

void AVRSystem::_initProgMemoryWithTestValues()
{
	const unsigned short testVal = 0xAABB;

	for (int i = 0; i < PROGRAM_MEMORY_SIZE_WORDS; i++)
		progMem[i] = testVal;
}

void AVRSystem::_initRamMemoryWithTestValues()
{
	const unsigned char testVal = 0xCC;

	for (int i = 0; i < PROGRAM_MEMORY_SIZE_WORDS; i++)
		ramMem[i] = testVal;
}

unsigned short AVRSystem::readRegister(int addr)
{
	if (addr >= REGISTERS_BEGIN && addr < REGISTERS_END)
		return generalRegisters.IORegisters[addr - REGISTERS_BEGIN];
	else switch (addr)
	{
	case CoreRegisterMap::REG_PC:
		return generalRegisters.ProgramCounter;
	default:
		return 0x00;
	}
		
}

unsigned char AVRSystem::readGeneralRegister(int addr)
{
	if (addr >= 0 && addr < REGISTERS_BEGIN)
		return generalRegisters.R[addr];
	else
		return 0xFF;
}


/* CORE RESET */

void AVRSystem::reset()
{
	memset(progMem, 0, PROGRAM_MEMORY_SIZE_BYTES);
	memset(ramMem, 0, RAM_MEMORY_SIZE_BYTES);
	memset(eepromMem, 0, EEPROM_MEMORY_SIZE_BYTES);
}


/* RESET OPERATIONS */

void AVRSystem::softReset()
{
	_resetRegisters();
	_resetRegisterFile();

	generalRegisters.IORegisters[CoreRegisterMap::MCUCSR] &= ~BIT_4;
	generalRegisters.IORegisters[CoreRegisterMap::MCUCSR] |= BIT_0;
}

void AVRSystem::_resetRegisters()
{
	memset((void*)&ramMem[REGISTERS_BEGIN], 0, (REGISTERS_END - REGISTERS_BEGIN));
	skipNext = false;
	generalRegisters.ProgramCounter = 0x0000;
	*generalRegisters.sp.wSP = 0x0000;
}

void AVRSystem::_resetRegisterFile()
{
	memset((void*)&ramMem, 0, REGISTERS_BEGIN);
}


/* PORT OPERATIONS */

void AVRSystem::_portSupervisorStepFunc()
{
	_updatePortRegisters();
}

void AVRSystem::_updatePortRegisters()
{
	// See AVRmega8 portC datasheet
	*SvPORTC.portDDR = *SvPORTC.portDDR & 0x3F; //ddrC;
	*SvPORTC.portDATA = *SvPORTC.portDATA & 0x3F; //portC;

	*SvPORTB.portPIN |= *SvPORTB.portDATA & *SvPORTB.portDDR;
	*SvPORTC.portPIN |= *SvPORTC.portDATA & *SvPORTC.portDDR;
	*SvPORTD.portPIN |= *SvPORTD.portDATA & *SvPORTD.portDDR;
}

void AVRSystem::_writePort(PortSupervisor &sv, unsigned char data)
{
	sv.memPIN = *sv.portPIN;
	*sv.portPIN = (data & ~(*sv.portDDR)) | (*sv.portDATA & *sv.portDDR);
}

int AVRSystem::_checkPinFront(PortSupervisor &sv, int pin)
{
	if (pin > 7) pin = 7;
	if (pin < 0) pin = 0;

	int cmp = ((*sv.portPIN >> pin) & 0x01) - ((sv.memPIN >> pin) & 0x01);

	/*  cmp ==  0 -> not changed
		cmp ==  1 -> low to high
		cmp == -1 -> high to low
	*/
	return cmp;
}

unsigned char AVRSystem::_readPinPort(PortSupervisor &sv, int pin)
{
	if (pin < 0) pin = 0;
	if (pin > 7) pin = 7;

	return (unsigned char)((*sv.portPIN >> pin) & 0x01);
}

unsigned char AVRSystem::_readPort(PortSupervisor &sv)
{
	return *sv.portPIN;
}

unsigned char AVRSystem::_readPortConfig(PortSupervisor &sv)
{
	return *sv.portDDR;
}

unsigned char AVRSystem::readPortA()
{
	// Not implemented in MEGA8
	return 0x00;
}

unsigned char AVRSystem::readPortB()
{
	return _readPort(SvPORTB);
}

unsigned char AVRSystem::readPortC()
{
	return _readPort(SvPORTC);
}

unsigned char AVRSystem::readPortD()
{
	return _readPort(SvPORTD);
}

unsigned char AVRSystem::readPortConfigA()
{
	return 0x00;
}

unsigned char AVRSystem::readPortConfigB()
{
	return _readPortConfig(SvPORTB);
}

unsigned char AVRSystem::readPortConfigC()
{
	return _readPortConfig(SvPORTC);
}

unsigned char AVRSystem::readPortConfigD()
{
	return _readPortConfig(SvPORTD);
}

void AVRSystem::writePortA(unsigned char data)
{
	// Not implemented in MEGA8
}

void AVRSystem::writePortB(unsigned char data)
{
	SvPORTB.memPIN = *SvPORTB.portPIN;
	_writePort(SvPORTB, data);
}

void AVRSystem::writePortC(unsigned char data)
{
	SvPORTC.memPIN = *SvPORTC.portPIN;
	_writePort(SvPORTC, data);
}

void AVRSystem::writePortD(unsigned char data)
{
	SvPORTD.memPIN = *SvPORTD.portPIN;
	_writePort(SvPORTD, data);
}


/* MAIN CPU FUNCTIONS */

void AVRSystem::clkHandler()
{
	// Periferial runs

	// Timer sequence
	_prescalerStepFunc();
	_timersStepFunc(&timer0, &timersController);
	_timersStepFunc(&timer1, &timersController);
	_timersStepFunc(&timer2, &exTimerController);
	
	// ADC sequence
	_adcUpdateState();
	_adcStepFunc();
	
	_portSupervisorStepFunc();

	// INT sequence
	_checkInteruptsEnabeld();	
	_runInterupts();

	// Main core execution

	if (voidCmdCyclesCount > 0)
		--voidCmdCyclesCount;
	else
		_mcuStepFunc();
}

void AVRSystem::_prescalerStepFunc()
{
	if (ramMem[CoreRegisterMap::SFIOR] & 0x01) // Timer 0/1 prescaler
	{
		timersController.prescalerClock = 0;
		ramMem[CoreRegisterMap::SFIOR] &= ~0x01;
	}

	if (ramMem[CoreRegisterMap::SFIOR] & 0x02) // Timer 2 prescaler
	{
		exTimerController.prescalerClock = 0;
		ramMem[CoreRegisterMap::SFIOR] &= ~0x02;
	}

	timersController.prescalerClock = (++timersController.prescalerClock) & 0x3FF;
	exTimerController.prescalerClock = (++exTimerController.prescalerClock) & 0x3FF;
	// TODO: Add regiser logic
}

void AVRSystem::_timersStepFunc(TimerCounterType0 *timer, TimerController *timeControl)
{
	// Timer0 / 2 logic
	switch (*timer->timerControlReg & 0x07)
	{
	case NO_CLOCK_SOURCE: break;
	case NO_PRESCALE: *timer->timerReg += 1; break;
	case PRESCALE_8:
		if ((timeControl->prescalerClock % 8) == 0)
			*timer->timerReg += 1;
		break;
	case PRESCALE_64:
		if ((timeControl->prescalerClock % 64) == 0)
			*timer->timerReg += 1;
		break;
	case PRESCALE_256:
		if ((timeControl->prescalerClock % 256) == 0)
			*timer->timerReg += 1;
		break;
	case PRESCALE_1024:
		if ((timeControl->prescalerClock % 1024) == 0)
			*timer->timerReg += 1;
		break;
	}

}

void AVRSystem::_timersStepFunc(TimerCounterType1 *timer, TimerController *timeControl)
{
	switch (*timer->timerControlRegB & 0x07)
	{
	case NO_CLOCK_SOURCE: break;
	case NO_PRESCALE: *timer->timerReg += 1; break;
	case PRESCALE_8:
		if ((timeControl->prescalerClock % 8) == 0)
			*timer->timerReg += 1;
		break;
	case PRESCALE_64:
		if ((timeControl->prescalerClock % 64) == 0)
			*timer->timerReg += 1;
		break;
	case PRESCALE_256:
		if ((timeControl->prescalerClock % 256) == 0)
			*timer->timerReg += 1;
		break;
	case PRESCALE_1024:
		if ((timeControl->prescalerClock % 1024) == 0)
			*timer->timerReg += 1;
		break;
	}
}

void AVRSystem::_mcuStepFunc()
{	
	if (generalRegisters.ProgramCounter >= PROGRAM_MEMORY_SIZE_WORDS)
		generalRegisters.ProgramCounter = 0;

	unsigned short cmd = pProgMemory[generalRegisters.ProgramCounter++];


	//TODO: Run DoubleWord CMD
	//TODO: Add LDD STSS LPM SPM CMD's

	skipNext = false;

	if (runFullLenParamCmd(cmd))
		return;

	if (runSingleParamCmd(cmd))
		return;

	if (runDoubleParamCmd(cmd))
		return;

	if (runImmeditateParamCmd(cmd))
		return;

	if (runSpecParamCmd(cmd))
		return;
}

// Single operand cmds execution module
bool AVRSystem::runSingleParamCmd(unsigned short opcode)
{
	unsigned short cmd_oneWord = (opcode >> 5) & 0x03FF;
	int R = 0;
	unsigned char d = 0;
	int K = 0;

	switch (cmd_oneWord)
	{		
	default: break;
	}

	return false;
}

// Two operand cmds execution module
bool AVRSystem::runDoubleParamCmd(unsigned short opcode)
{
	unsigned short cmd_twoWord = (opcode >> 10) & 0x003F;
	int R = 0;
	int Rd = 0;
	int Rr = 0;
	unsigned char ld_type = 0xFF;

	switch (cmd_twoWord)
	{

	case 0x03: // ADD | LSL
		Rd = ((opcode >> 4) & 0xF) | ((opcode >> 8) & 0x1) << 4;
		Rr = (opcode & 0xF) | ((opcode >> 9) & 0x1) << 4;
		R = (signed char)generalRegisters.R[Rd] + (signed char)generalRegisters.R[Rr];

		_check_SREG_H(generalRegisters.R[Rd], generalRegisters.R[Rr]);
		_check_SREG_V(R); //generalRegisters.R[Rd], generalRegisters.R[Rr], 
		_check_SREG_N(R & 0xFF);
		_check_SREG_S();
		_check_SREG_C(R);
		_check_SREG_Z(R & 0xFF);

		generalRegisters.R[Rd] = R & 0xFF;
		return true;

	case 0x07: // ADC | ROL
		Rd = ((opcode >> 4) & 0xF) | ((opcode >> 8) & 0x1) << 4;
		Rr = (opcode & 0xF) | ((opcode >> 9) & 0x1) << 4;
		R = (signed char)generalRegisters.R[Rd] + (signed char)generalRegisters.R[Rr] + (ramMem[CoreRegisterMap::SREG] & SREG_C ? 1 : 0);

		_check_SREG_H(generalRegisters.R[Rd], generalRegisters.R[Rr]);
		_check_SREG_V(R); //generalRegisters.R[Rd], generalRegisters.R[Rr], 
		_check_SREG_N(R & 0xFF);
		_check_SREG_S();
		_check_SREG_C(R);
		_check_SREG_Z(R & 0xFF);

		generalRegisters.R[Rd] = R & 0xFF;
		return true;

	case 0x6: // SUB
		Rd = ((opcode >> 4) & 0xF) | ((opcode >> 8) & 0x1) << 4;
		Rr = (opcode & 0xF) | ((opcode >> 9) & 0x1) << 4;
		R = (signed char)generalRegisters.R[Rd] - (signed char)generalRegisters.R[Rr];

		_check_SREG_H(generalRegisters.R[Rd], generalRegisters.R[Rr]);
		_check_SREG_V(R); //generalRegisters.R[Rd], generalRegisters.R[Rr], 
		_check_SREG_N(R & 0xFF);
		_check_SREG_S();
		_check_SREG_C(R);
		_check_SREG_Z(R & 0xFF);

		generalRegisters.R[Rd] = R & 0xFF;
		return true;

	case 0x2: // SBC
		Rd = ((opcode >> 4) & 0xF) | ((opcode >> 8) & 0x1) << 4;
		Rr = (opcode & 0xF) | ((opcode >> 9) & 0x1) << 4;
		R = (signed char)generalRegisters.R[Rd] - (signed char)generalRegisters.R[Rr] - (ramMem[CoreRegisterMap::SREG] & SREG_C ? 1 : 0);

		_check_SREG_H(generalRegisters.R[Rd], generalRegisters.R[Rr]);
		_check_SREG_V(R); //generalRegisters.R[Rd], generalRegisters.R[Rr], 
		_check_SREG_N(R & 0xFF);
		_check_SREG_S();
		_check_SREG_C(R);
		_check_SREG_Z(R & 0xFF);

		generalRegisters.R[Rd] = R & 0xFF;
		return true;

	case 0x8: // AND | TST
		Rd = ((opcode >> 4) & 0xF) | ((opcode >> 8) & 0x1) << 4;
		Rr = (opcode & 0xF) | ((opcode >> 9) & 0x1) << 4;
		R = generalRegisters.R[Rd] & generalRegisters.R[Rr];

		ramMem[CoreRegisterMap::SREG] &= ~SREG_V;
		_check_SREG_Z(R);
		_check_SREG_N(R);
		_check_SREG_S();

		generalRegisters.R[Rd] = R & 0xFF;
		return true;

	case 0x0A: // OR
		Rd = ((opcode >> 4) & 0xF) | ((opcode >> 8) & 0x1) << 4;
		Rr = (opcode & 0xF) | ((opcode >> 9) & 0x1) << 4;
		R = generalRegisters.R[Rd] | generalRegisters.R[Rr];

		ramMem[CoreRegisterMap::SREG] &= ~SREG_V;
		_check_SREG_Z(R);
		_check_SREG_N(R);
		_check_SREG_S();

		generalRegisters.R[Rd] = R & 0xFF;
		return true;

	case 0x9: // EOR | CLR
		Rd = ((opcode >> 4) & 0xF) | ((opcode >> 8) & 0x1) << 4;
		Rr = (opcode & 0xF) | ((opcode >> 9) & 0x1) << 4;
		R = generalRegisters.R[Rd] ^ generalRegisters.R[Rr];

		ramMem[CoreRegisterMap::SREG] &= ~SREG_V;
		_check_SREG_Z(R);
		_check_SREG_N(R);
		_check_SREG_S();

		generalRegisters.R[Rd] = R & 0xFF;
		return true;

	case 0x37: // COM | INC | DEC
		Rd = ((opcode >> 4) & 0xF) | ((opcode >> 8) & 0x1) << 4;
		Rr = (opcode & 0xF) | ((opcode >> 9) & 0x1) << 4;

		if (Rr == 0){ // COM
			R = (unsigned char)0xFF - generalRegisters.R[Rd];

			ramMem[CoreRegisterMap::SREG] &= ~SREG_V;
			ramMem[CoreRegisterMap::SREG] |= SREG_C;
			_check_SREG_N(R);
			_check_SREG_Z(R);
			_check_SREG_S();

			generalRegisters.R[Rd] = R & 0xFF;
			return true;
		}

		if (Rr == 3) { // INC
			R = generalRegisters.R[Rd] + 1;

			_check_SREG_Z(R);
			_check_SREG_N(R);
			_check_SREG_V(R); //generalRegisters.R[Rd], 0, 
			_check_SREG_S();

			generalRegisters.R[Rd] = R & 0xFF;
			return true;
		}

		break;

	case 0x27: // MUL
		Rd = ((opcode >> 4) & 0xF) | ((opcode >> 8) & 0x1) << 4;
		Rr = (opcode & 0xF) | ((opcode >> 9) & 0x1) << 4;

		R = (unsigned char)generalRegisters.R[Rd] * (unsigned char)generalRegisters.R[Rr];

		_check_SREG_Z(R);
		_check_SREG_C_word(R);

		generalRegisters.R[0] = R & 0xFF;
		generalRegisters.R[1] = (R >> 8) & 0xFF;
		return true;

	case 0x04: // CPSE
		Rd = ((opcode >> 4) & 0xF) | ((opcode >> 8) & 0x1) << 4;
		Rr = (opcode & 0xF) | ((opcode >> 9) & 0x1) << 4;
		if (generalRegisters.R[Rd] == generalRegisters.R[Rr])
		{
			generalRegisters.ProgramCounter++;
			skipNext = true;
		}
		return true;

	case 0x05: // CP
		Rd = ((opcode >> 4) & 0xF) | ((opcode >> 8) & 0x1) << 4;
		Rr = (opcode & 0xF) | ((opcode >> 9) & 0x1) << 4;
		R = generalRegisters.R[Rd] - generalRegisters.R[Rr];

		_check_SREG_Z(R);
		_check_SREG_H(generalRegisters.R[Rd], generalRegisters.R[Rr]);
		_check_SREG_C(R);
		_check_SREG_N(R);		
		_check_SREG_V(R); //generalRegisters.R[Rd], generalRegisters.R[Rr], 
		_check_SREG_S();
		return true;

	case 0x01: // CPC
		Rd = ((opcode >> 4) & 0xF) | ((opcode >> 8) & 0x1) << 4;
		Rr = (opcode & 0xF) | ((opcode >> 9) & 0x1) << 4;
		R = generalRegisters.R[Rd] - generalRegisters.R[Rr] - (ramMem[CoreRegisterMap::SREG] & SREG_C ? 1 : 0);

		_check_SREG_Z(R);
		_check_SREG_H(generalRegisters.R[Rd], generalRegisters.R[Rr]);
		_check_SREG_C(R);
		_check_SREG_N(R);
		_check_SREG_V(R); //generalRegisters.R[Rd], generalRegisters.R[Rr], 
		_check_SREG_S();
		return true;

	case 0x3F: // SBRC | SBRS
		Rd = ((opcode >> 4) & 0xF) | ((opcode >> 8) & 0x1) << 4;
		Rr = opcode & 0x7;

		if (!((opcode >> 9) & 0x1)) // SBRC
		if (!(generalRegisters.R[Rd] & (1 << Rr)))
		{
			generalRegisters.ProgramCounter++;
			skipNext = true;
		}
		return true;

		if (((opcode >> 9) & 0x1)) // SBRS
		if ((generalRegisters.R[Rd] & (1 << Rr)))
		{
			generalRegisters.ProgramCounter++;
			skipNext = true;
		}
		return true;

	case 0x3C: // BRBS | BREQ | BRCS | BRLO | BRMI | BRLT | BRHS | BRTS | BRVS | BRIE
		Rr = opcode & 0x07;
		Rd = (opcode >> 3) & 0x7F;
		
		if (Rd & 0x40)
			Rd = (signed char)(Rd | 0x80);

		if (ramMem[CoreRegisterMap::SREG] & (1 << Rr))
			generalRegisters.ProgramCounter += Rd;
		return true;

	case 0x3D: // BRBC | BRNE | BRCC | BRSH | BRPL | BRGE | BRHC | BRTC | BRVC | BRID
		Rr = opcode & 0x07;
		Rd = (opcode >> 3) & 0x7F;

		if (Rd & 0x40)
			Rd = (signed char)(Rd | 0x80);

		if (!(ramMem[CoreRegisterMap::SREG] & (1 << Rr)))
			generalRegisters.ProgramCounter += Rd;
		return true;

	case 0x3E: // BST | BLD
		Rd = (opcode >> 4) & 0x1F;
		Rr = opcode & 0x7;
		ld_type = (opcode >> 9) & 0x01;
		
		if (ld_type)
		{
			if (generalRegisters.R[Rd] & (1 << Rr))
				ramMem[CoreRegisterMap::SREG] |= SREG_T;
			else
				ramMem[CoreRegisterMap::SREG] &= ~SREG_T;
		}
		else
		{
			if (ramMem[CoreRegisterMap::SREG] & SREG_T)
				generalRegisters.R[Rd] |= (1 << Rr);
			else
				generalRegisters.R[Rd] &= ~(1 << Rr);
		}

		return true;

	case 0x0B: // MOV
		Rd = ((opcode >> 4) & 0xF) | ((opcode >> 8) & 0x1) << 4;
		Rr = (opcode & 0xF) | ((opcode >> 9) & 0x1) << 4;

		generalRegisters.R[Rd] = generalRegisters.R[Rr];
		return true;

	case 0x24: // LD X   ST X Y Z	LPM
		ld_type = opcode & 0x0F;
		Rd = (opcode >> 0x0F) & 0x1F;

		if (!((opcode >> 9) & 0x01)) // LD
		{
			if (ld_type == 0xC) // X
			{
				generalRegisters.R[Rd] = ramMem[*generalRegisters.X_register];
			}
			else if (ld_type == 0xD) // X
			{
				generalRegisters.R[Rd] = ramMem[*generalRegisters.X_register];
				(*generalRegisters.X_register)++;
			}
			else if (ld_type == 0xE) // X
			{
				(*generalRegisters.X_register)--;
				generalRegisters.R[Rd] = ramMem[*generalRegisters.X_register];
			}
			if (ld_type == 9) // Y
			{
				generalRegisters.R[Rd] = ramMem[*generalRegisters.Y_register];
				(*generalRegisters.Y_register)++;
			}
			else if (ld_type == 0xA) // Y
			{
				(*generalRegisters.Y_register)--;
				generalRegisters.R[Rd] = ramMem[*generalRegisters.Y_register];
			}
			else if (ld_type == 0x1) // Z
			{
				generalRegisters.R[Rd] = ramMem[*generalRegisters.Z_register];
				(*generalRegisters.Z_register)++;
			}
			else if (ld_type == 0x2) // Z
			{
				(*generalRegisters.Z_register)--;
				generalRegisters.R[Rd] = ramMem[*generalRegisters.Z_register];
			}
			else if (ld_type == 0x0) // LDS
			{
				generalRegisters.ProgramCounter++; // Next byte is addr
				generalRegisters.R[Rd] = ramMem[progMem[generalRegisters.ProgramCounter]];
			}
			else if (ld_type == 0x5) // LPM
			{
				(*generalRegisters.Z_register)++;
				generalRegisters.R[Rd] = ((char*)progMem)[*generalRegisters.Z_register];
			}
			else if (ld_type == 0x4) // LPM
			{
				generalRegisters.R[Rd] = ((char*)progMem)[*generalRegisters.Z_register];
			}
			else if (ld_type == 0xF) // POP
			{
				generalRegisters.R[Rd] = ramMem[*generalRegisters.sp.wSP];
			}
		}

		if ((opcode >> 9) & 0x01) // ST X Y Z   STS
		{
			if (ld_type == 0xC) // X
			{
				ramMem[*generalRegisters.X_register] = generalRegisters.R[Rd];
			}
			else if (ld_type == 0xD) // X
			{
				ramMem[*generalRegisters.X_register] = generalRegisters.R[Rd];
				(*generalRegisters.X_register)++;
			}
			else if (ld_type == 0xE) // X
			{
				(*generalRegisters.X_register)--;
				ramMem[*generalRegisters.X_register] = generalRegisters.R[Rd];
			}
			else if (ld_type == 9) // Y
			{
				ramMem[*generalRegisters.Y_register] = generalRegisters.R[Rd];
				(*generalRegisters.Y_register)++;
			}
			else if (ld_type == 0xA) // Y
			{
				(*generalRegisters.Y_register)--;
				ramMem[*generalRegisters.Y_register] = generalRegisters.R[Rd];
			}
			else if (ld_type == 0x1) // Z
			{
				ramMem[*generalRegisters.Z_register] = generalRegisters.R[Rd];
				(*generalRegisters.Z_register)++;
			}
			else if (ld_type == 0x2) // Z
			{
				(*generalRegisters.Z_register)--;
				ramMem[*generalRegisters.Z_register] = generalRegisters.R[Rd];
			}
			else if (ld_type == 0x0) // STS
			{
				generalRegisters.ProgramCounter++; // Next byte is addr
				ramMem[progMem[generalRegisters.ProgramCounter]] = generalRegisters.R[Rd];
			}
			else if (ld_type == 0xF) // PUSH
			{
				ramMem[*generalRegisters.sp.wSP] = generalRegisters.R[Rd];
			}
		}

		return true;

	case 0x20: // LD Y Z
		ld_type = opcode & 0x0F;
		Rd = (opcode >> 0x0F) & 0x1F;

		if (ld_type == 0x8)
		{
			generalRegisters.R[Rd] = ramMem[*generalRegisters.Y_register];
		}
		else if (ld_type == 0x0)
		{
			generalRegisters.R[Rd] = ramMem[*generalRegisters.Z_register];
		}

		return true;

	case 0x25: // LSR | ROR | ASR | SWAP | CALL | NEG | DEC
		ld_type = opcode & 0x0F;
		Rd = (opcode >> 4) & 0x1F;

		if (ld_type == 0x6) // LSR
		{
			if (generalRegisters.R[Rd] & 0x01)
				ramMem[CoreRegisterMap::SREG] |= SREG_C;

			generalRegisters.R[Rd] = generalRegisters.R[Rd] >> 1;
		}
		else if (ld_type == 0x7) // ROR
		{
			int temp = ramMem[CoreRegisterMap::SREG] & SREG_C ? 1 : 0;

			if (generalRegisters.R[Rd] & 0x01)
				ramMem[CoreRegisterMap::SREG] |= SREG_C;

			generalRegisters.R[Rd] = (generalRegisters.R[Rd] >> 1) | (temp << 7);
		}
		else if (ld_type == 0x5) // ASR
		{
			if (generalRegisters.R[Rd] & 0x01)
				ramMem[CoreRegisterMap::SREG] |= SREG_C;

			generalRegisters.R[Rd] = ((signed char)generalRegisters.R[Rd]) >> 1;
		}
		else if (ld_type == 0x2) // SWAP
		{			
			generalRegisters.R[Rd] = ((generalRegisters.R[Rd] >> 4) & 0xF) | ((generalRegisters.R[Rd] << 4) & 0xF);
		}
		//else if (((ld_type >> 1) & 0x07) == 0x07) // CALL
		//{
		//	R = progMem[generalRegisters.ProgramCounter++];
		//	_push_word(generalRegisters.ProgramCounter);
		//	generalRegisters.ProgramCounter = R;

		//}
		else if (ld_type == 1) // NEG
		{ 
			R = (signed char)0x00 - (signed char)(generalRegisters.R[Rd]);

			_check_SREG_Z(R);
			_check_SREG_H(generalRegisters.R[Rd], 0);
			_check_SREG_C(R);
			_check_SREG_N(R);
			_check_SREG_V(R); //generalRegisters.R[Rd], 0, 
			_check_SREG_S();

			generalRegisters.R[Rd] = R & 0xFF;
			return true;
		}
		else if (ld_type == 0x0A){ // DEC
			R = generalRegisters.R[Rd] - 1;

			_check_SREG_Z(R);
			_check_SREG_N(R);
			_check_SREG_V(R); //generalRegisters.R[Rd], 0, 
			_check_SREG_S();

			generalRegisters.R[Rd] = R & 0xFF;
			return true;
		}
		return false; // check for SBI cmd

	}
		return false;	
}

// Unstandart cmds execution module
bool AVRSystem::runSpecParamCmd(unsigned short opcode)
{
	unsigned short cmd_short = (opcode >> 12) & 0xF;
	int R = 0;
	int K = 0;
	int Rd = 0;

	switch (cmd_short)
	{
	case 0x5: // SUBI
		K = opcode & 0xF | ((opcode >> 8) & 0xF) << 4;
		Rd = ((opcode >> 4) & 0xF) + 16;
		R = generalRegisters.R[Rd] - K;
		
		_check_SREG_H(generalRegisters.R[Rd], K);
		_check_SREG_V(R); //generalRegisters.R[Rd], K, 
		_check_SREG_N(R & 0xFF);
		_check_SREG_S();
		_check_SREG_C(R);
		_check_SREG_Z(R & 0xFF);

		generalRegisters.R[Rd] = R & 0xFF;
		return true;

	case 0x4: // SBCI
		K = opcode & 0xF | ((opcode >> 8) & 0xF) << 4;
		Rd = ((opcode >> 4) & 0xF)+16;
		R = generalRegisters.R[Rd] - K - (ramMem[CoreRegisterMap::SREG] & SREG_C ? 1 : 0);

		_check_SREG_H(generalRegisters.R[Rd], K);
		_check_SREG_V(R); //generalRegisters.R[Rd], K, 
		_check_SREG_N(R & 0xFF);
		_check_SREG_S();
		_check_SREG_C(R);
		_check_SREG_Z(R & 0xFF);

		generalRegisters.R[Rd] = R & 0xFF;
		return true;

	case 0x7: // ANDI
		K = opcode & 0xF | ((opcode >> 8) & 0xF) << 4;
		Rd = ((opcode >> 4) & 0xF)+16;
		R = generalRegisters.R[Rd] & K;
		ramMem[CoreRegisterMap::SREG] &= ~SREG_V;

		_check_SREG_Z(R);
		_check_SREG_N(R);
		_check_SREG_S();

		generalRegisters.R[Rd] = R & 0xFF;
		return true;

	case 0x6: // ORI | SBR
		K = opcode & 0xF | ((opcode >> 8) & 0xF) << 4;
		Rd = ((opcode >> 4) & 0xF)+16;
		R = generalRegisters.R[Rd] | K;
		ramMem[CoreRegisterMap::SREG] &= ~SREG_V;

		_check_SREG_Z(R);
		_check_SREG_N(R);
		_check_SREG_S();

		generalRegisters.R[Rd] = R & 0xFF;
		return true;

	case 0xC: // RJMP
		K = opcode & 0xFFF;
		
		if (K & 0x800)
			K = (signed short)(K | 0xF000);

		generalRegisters.ProgramCounter += K;
		return true;

	case 0xD: // RCALL
		_push_word(generalRegisters.ProgramCounter);
		K = opcode & 0xFFF;

		if (K & 0x800)
			K = (signed short)(K | 0xF000);

		generalRegisters.ProgramCounter += K;
		return true;

	case 0x03: // CPI
		K = opcode & 0xF | ((opcode >> 8) & 0xF) << 4;
		Rd = ((opcode >> 4) & 0xF)+16;
		R = generalRegisters.R[Rd] - K;

		_check_SREG_H(generalRegisters.R[Rd], K);
		_check_SREG_V(R); // generalRegisters.R[Rd], K, 
		_check_SREG_N(R & 0xFF);
		_check_SREG_S();
		_check_SREG_C(R);
		_check_SREG_Z(R & 0xFF);
		return true;

	case 0x0E: // LDI | SER
		K = opcode & 0xF | ((opcode >> 8) & 0xF) << 4;
		Rd = ((opcode >> 4) & 0xF)+16;

		generalRegisters.R[Rd] = K;
		return true;

	case 0x0A: // STS
		Rd = ((opcode >> 4) & 0xF)+16;
		K = opcode & 0xF | ((opcode >> 9) & 0x3) << 4 | ((opcode >> 4) & 0x01) << 6 | (~((opcode >> 4) & 0x01)) << 6;
		ramMem[K] = generalRegisters.R[Rd];
		return true;

	case 0x0B: // IN | OUT
		K = opcode & 0xF | ((opcode >> 9) & 0x3) << 4;
		Rd = (opcode >> 4) & 0x1F;
		
		if (!((opcode >> 11) & 0x01))
			generalRegisters.R[Rd] = generalRegisters.IORegisters[K];
		else if ((opcode >> 11) & 0x01)
			generalRegisters.IORegisters[K] = generalRegisters.R[Rd];

		return true;
	}

	return false;
}

// Immediate cmds execution module
bool AVRSystem::runImmeditateParamCmd(unsigned short opcode)
{
	unsigned short cmd_Immediate = (opcode >> 8) & 0xFF;
	int R = 0;
	unsigned char d = 0;
	unsigned char r = 0;
	int K = 0;
	unsigned short* regPair = NULL;

	switch (cmd_Immediate) //TODO: Check for only 4 upper REG pairs
	{
	case 0x96: // ADIW
		d = ((opcode >> 4) & 0x3)*2 + 24;
		K = (opcode & 0xF) | ((opcode >> 6) & 0x3) << 4;
		
		regPair = (unsigned short*)((unsigned char*) &generalRegisters.R[d]);
		R = *regPair + K;

		_check_SREG_V_word(R); //*regPair, K,
		_check_SREG_C_word(R);
		_check_SREG_Z(R & 0xFFFF);
		_check_SREG_N_word(R);
		_check_SREG_S();

		*regPair = R & 0xFFFF;
		return true;

	case 0x97: // SBIW
		d = ((opcode >> 4) & 0x3)*2 + 24;
		K = (opcode & 0xF) | ((opcode >> 6) & 0x3) << 4;

		regPair = (unsigned short*)((unsigned char*) &generalRegisters.R[d]);
		R = *regPair - K;

		_check_SREG_V_word(R); //*regPair, K,
		_check_SREG_C_word(R);
		_check_SREG_Z(R & 0xFFFF);
		_check_SREG_N_word(R);
		_check_SREG_S();

		*regPair = R & 0xFFFF;
		return true;

	case 0x02: // MULS
		d = ((opcode >> 4) & 0xF)+16;
		r = (opcode & 0xF)+16;
		R = (signed char)generalRegisters.R[d] * (signed char)generalRegisters.R[r];

		_check_SREG_Z(R);
		_check_SREG_C_word(R);

		generalRegisters.R[0] = R & 0xFF;
		generalRegisters.R[1] = (R >> 8) & 0xFF;
		return true;

	case 0x3: // MULSU | FMUL | FMULS | FMULSU
		d = ((opcode >> 4) & 0x7)+16;
		r = (opcode & 0x7)+16;

		if (!(opcode & 0x8) && !(opcode & 0x80)){ // MULSU
			R = (signed char)generalRegisters.R[d] * (unsigned char)generalRegisters.R[r];

			_check_SREG_Z(R);
			_check_SREG_C_word(R);

			generalRegisters.R[0] = R & 0xFF;
			generalRegisters.R[1] = (R >> 8) & 0xFF;
			return true;
		}

		if ((opcode & 0x8) && !(opcode & 0x80)){ // FMUL
			R = ((unsigned char)generalRegisters.R[d] * (unsigned char)generalRegisters.R[r]) << 1;

			_check_SREG_Z(R);
			_check_SREG_C_word(R);

			generalRegisters.R[0] = R & 0xFF;
			generalRegisters.R[1] = (R >> 8) & 0xFF;
			return true;
		}

		if (!(opcode & 0x8) && (opcode & 0x80)){ // FMULS
			R = ((signed char)generalRegisters.R[d] * (signed char)generalRegisters.R[r]) << 1;

			_check_SREG_Z(R);
			_check_SREG_C_word(R);

			generalRegisters.R[0] = R & 0xFF;
			generalRegisters.R[1] = (R >> 8) & 0xFF;
			return true;
		}

		if ((opcode & 0x8) && (opcode & 0x80)){ // FMULSU
			R = ((signed char)generalRegisters.R[d] * (unsigned char)generalRegisters.R[r]) << 1;

			_check_SREG_Z(R);
			_check_SREG_C_word(R);

			generalRegisters.R[0] = R & 0xFF;
			generalRegisters.R[1] = (R >> 8) & 0xFF;
			return true;
		}
		break;
		
	case 0x99: // SBIC
		d = (opcode >> 3) & 0x1F;
		r = opcode & 0x7;

		if (!(generalRegisters.IORegisters[d] & (1 << r)))
		{
			generalRegisters.ProgramCounter++;
			skipNext = true;
		}
		return true;

	case 0x9B: // SBIS
		d = (opcode >> 3) & 0x1F;
		r = opcode & 0x7;

		if (generalRegisters.IORegisters[d] & (1 << r))
		{
			generalRegisters.ProgramCounter++;
			skipNext = true;
		}
		return true;

	case 0x01: // MOVW
		d = ((opcode >> 4) & 0x0F)*2;
		r = (opcode & 0x0F)*2;

		generalRegisters.R[d] = generalRegisters.R[r];
		generalRegisters.R[d+1] = generalRegisters.R[r+1];
		return true;

	case 0x9A: // SBI
		d = (opcode >> 3) & 0x1F;
		r = opcode & 0x7;

		generalRegisters.IORegisters[d] |= 1 << r;
		return true;

	case 0x98: // CBI
		d = (opcode >> 3) & 0x1F;
		r = opcode & 0x7;

		generalRegisters.IORegisters[d] &= ~(1 << r);
		return true;

	case 0x94: // BSET | BCLR
		d = opcode & 0x04;
		r = (opcode >> 4) & 0x7;
		K = (opcode & 0x80) ? 1 : 0;

		if (K) // BSET
			ramMem[CoreRegisterMap::SREG] &= ~(1 << r);
		else // BCLR
			ramMem[CoreRegisterMap::SREG] |= (1 << r);
		
		return true;
	}

	return false;
}

// Cmds with full opcode pattern
bool AVRSystem::runFullLenParamCmd(unsigned short opcode)
{
	unsigned short* stack = NULL;

	switch (opcode)
	{
	case 0x9409: // IJMP
		generalRegisters.ProgramCounter = *generalRegisters.Z_register;
		return true;

	case 0x9509: // ICALL
		_push_word(generalRegisters.ProgramCounter);
		generalRegisters.ProgramCounter = *generalRegisters.Z_register;
		return true;

	case 0x9508: // RET
		generalRegisters.ProgramCounter = _pop_word();
		return true;

	case 0x9518: // RETI
		ramMem[CoreRegisterMap::SREG] |= SREG_I;
		generalRegisters.ProgramCounter = _pop_word();
		return true;
	}

	return false;
}


/* SREG OPERATIONS */

void AVRSystem::_check_SREG_H(unsigned char Rd, unsigned char Rr)
{
	if (((char)(Rd & 0x0F) - (char)(Rr & 0x0F)) & 0xF0)
		ramMem[CoreRegisterMap::SREG] |= SREG_H;
	else
		ramMem[CoreRegisterMap::SREG] &= ~SREG_H;
}

void AVRSystem::_check_SREG_V(int R)
{
	if ((R < -128) && (R & 0x80)) //(Rd > 0 && Rr >0 && R < 0 || Rd < 0 && Rr < 0 && R > 0)
		ramMem[CoreRegisterMap::SREG] |= SREG_V;
	else
		ramMem[CoreRegisterMap::SREG] &= ~SREG_V;
}

void AVRSystem::_check_SREG_C(int R)
{
	if (R & 0x100)
		ramMem[CoreRegisterMap::SREG] |= SREG_C;
	else
		ramMem[CoreRegisterMap::SREG] &= ~SREG_C;
}

void AVRSystem::_check_SREG_V_word(int R)
{
	if (R < -32768) //(Rdw > 0 && Rrw >0 && R < 0 || Rdw < 0 && Rrw < 0 && R > 0)
		ramMem[CoreRegisterMap::SREG] |= SREG_V;
	else
		ramMem[CoreRegisterMap::SREG] &= ~SREG_V;
}

void AVRSystem::_check_SREG_C_word(int R)
{
	if (R & 0x10000)
		ramMem[CoreRegisterMap::SREG] |= SREG_C;
	else
		ramMem[CoreRegisterMap::SREG] &= ~SREG_C;
}

// SREG_S flag must be checked after SREG_N & SREG_V
void AVRSystem::_check_SREG_S()
{
	if (((((ramMem[CoreRegisterMap::SREG] & SREG_N) >> 1) + ((ramMem[CoreRegisterMap::SREG] & SREG_V) >> 3)) & 0x3) != 0)
		ramMem[CoreRegisterMap::SREG] |= SREG_S;
	else
		ramMem[CoreRegisterMap::SREG] &= ~SREG_S;
}

void AVRSystem::_check_SREG_N(unsigned char R)
{
	if (R & 0x80)
		ramMem[CoreRegisterMap::SREG] |= SREG_N;
	else
		ramMem[CoreRegisterMap::SREG] &= ~SREG_N;
}

void AVRSystem::_check_SREG_N_word(unsigned short R)
{
	if (R & 0x8000)
		ramMem[CoreRegisterMap::SREG] |= SREG_N;
	else
		ramMem[CoreRegisterMap::SREG] &= ~SREG_N;
}

void AVRSystem::_check_SREG_Z(unsigned short R)
{
	if (R == 0)
		ramMem[CoreRegisterMap::SREG] |= SREG_Z;
	else
		ramMem[CoreRegisterMap::SREG] &= ~SREG_Z;
}


/* STACK OPERATIONS */

void AVRSystem::_push_word(unsigned short val)
{
	generalRegisters.sp.wSP[0] -= 2;
	unsigned int stPoint = generalRegisters.sp.wSP[0];
	unsigned short *savePoint = (unsigned short*)&ramMem[stPoint];
	*savePoint = val;
	//*((unsigned short*)&ramMem[generalRegisters.sp.wSP[0]]) = val;
}

unsigned short AVRSystem::_pop_word()
{
	unsigned int stPoint = generalRegisters.sp.wSP[0];
	unsigned short *savePoint = (unsigned short*)&ramMem[stPoint];
	unsigned short val = *savePoint;
	//unsigned short val = *((unsigned short*)&ramMem[generalRegisters.sp.wSP[0]]);
	generalRegisters.sp.wSP[0] += 2;
	return val;
}

void AVRSystem::_push_byte(unsigned char val)
{
	generalRegisters.sp.wSP[0] -= 1;
	ramMem[generalRegisters.sp.wSP[0]] = val;
}

unsigned char AVRSystem::_pop_byte()
{
	unsigned char val = ramMem[generalRegisters.sp.wSP[0]];
	generalRegisters.sp.wSP[0] -= 1;
	return val;
}

void AVRSystem::_setStackPointer(unsigned short stVal)
{
	if (stVal < 0 || stVal >= RAM_MEMORY_SIZE_BYTES / sizeof(unsigned short))
		return;

	ramMem[CoreRegisterMap::SPL] = stVal & 0xFF;
	ramMem[CoreRegisterMap::SPH] = (stVal >> 8) & 0xFF;
}

unsigned short AVRSystem::_getStackPonter()
{
	unsigned short stVal = (((unsigned short)ramMem[CoreRegisterMap::SPH] << 8) & 0xFF00) | ramMem[CoreRegisterMap::SPL];
	return stVal;
}


/* ADC OPERATIONS */

void AVRSystem::_adcStepFunc()
{
	if (!adcSupervisor.isEnabled)
		return;

	if (adcSupervisor.conversionTimer > 0)
	{
		adcSupervisor.conversionTimer--;
		return;
	}
	else
		adcSupervisor.conversionTimer = ADC_CONVERSION_TICKS;

	// IF ADC started
	unsigned char SFIOR_ADTS = *timersController.specialFuncReg  & SFIOR_ADTS_MASK;

	switch (SFIOR_ADTS)
	{
	case SFIOR_ADTS_NOCOND:
		unsigned int adcConversionResult = (unsigned int)(adcSupervisor.adcCHN[adcSupervisor.selectedCHN] * adcSupervisor.conversionKoef) & 0x3FF;

		if (*adcSupervisor.ADMUX & ADMUX_ADLAR_MASK)
			adcConversionResult = adcConversionResult << 6;

		// INT flag set after conversion
		*ircSupervisor.ADCSRA |= ADCSRA_ADIF_MASK;

		// Write conversion result to register
		*adcSupervisor.ADCH = (adcConversionResult >> 8) & 0xFF;
		*adcSupervisor.ADCL = adcConversionResult & 0xFF;

		ircSupervisor.isAdcExec = true;
		return;

	}

	ircSupervisor.isAdcExec = false;
}

void AVRSystem::_adcUpdateState()
{
	// Configure input chanell [ADMUX : MUX3..MUX0] -> 1110: internal 1.22V, 1111: GND
	adcSupervisor.selectedCHN = *adcSupervisor.ADMUX & 0x0F;
	if (adcSupervisor.selectedCHN > (MAX_ADC_CHNS-1)) // ADC first chan is 0 
	{
		switch (adcSupervisor.selectedCHN)
		{
		case 0x0E:
			adcSupervisor.useGND = false;
			adcSupervisor.useRef = true;
			break;

		case 0x0F:
			adcSupervisor.useGND = true;
			adcSupervisor.useRef = false;
			break;

		default:
			adcSupervisor.useGND = false;
			adcSupervisor.useRef = false;
			adcSupervisor.isEnabled = false;
			return; // If set invalid chan disable conversion
		}
	} 
	else
	{
		adcSupervisor.useGND = false;
		adcSupervisor.useRef = false;
	}

	// For now AREF logic not affects conversion -> AREF configuration skipped

	// Check for enabled
	if (*ircSupervisor.ADCSRA  & ADCSRA_ADEN_MASK)
	{
		// Conversion activated 
		if (*ircSupervisor.ADCSRA & ADCSRA_ADSC_MASK)
		{
			adcSupervisor.isEnabled = true;
		}
		else
			adcSupervisor.conversionTimer = ADC_CONVERSION_TICKS;

		return;
	}

	adcSupervisor.conversionTimer = ADC_CONVERSION_TICKS;
	adcSupervisor.isEnabled = false;
	return;
	
	// Speed configuration not affects now -> only check for correct values
}

void AVRSystem::writeToADCChan(double voltage, unsigned int chnNo)
{
	if (voltage < 0)
		voltage *= -1.;
	if (voltage > MAX_ADC_VOLTAGE)
		voltage = MAX_ADC_VOLTAGE;

	if (chnNo >= MAX_ADC_CHNS)
		return;

	adcSupervisor.adcCHN[chnNo] = voltage;
}


/* INT OPERATIONS*/

void AVRSystem::_checkInteruptsEnabeld()
{
	ircSupervisor.isINTEnabeld = (*generalRegisters.sreg) & SREG_I ? true : false;

	ircSupervisor.isExt0Enabled = (*ircSupervisor.GICR) & INT_INT0 ? true : false;
	ircSupervisor.isExt1Enabeld = (*ircSupervisor.GICR) & INT_INT1 ? true : false;
	ircSupervisor.isAdcEnabled = (*ircSupervisor.ADCSRA) & ADCSRA_ADIE_MASK ? true : false;

	// Check if there must be ext interupts

	unsigned int ext0Condition = (*ircSupervisor.MCUCR) & 0x03;

	if (ircSupervisor.isExt0Enabled){

		if (_checkPinFront(SvPORTD, INT_INT0_PIN_NO) != FRONT_NO && ext0Condition == INT_EXT_SIG_CHG)
			*ircSupervisor.GIFR |= 0x40;
		else if (_checkPinFront(SvPORTD, INT_INT0_PIN_NO) == FRONT_RIS && ext0Condition == INT_EXT_SIG_RIS)
			*ircSupervisor.GIFR |= 0x40;
		else if (_checkPinFront(SvPORTD, INT_INT0_PIN_NO) == FRONT_FAL && ext0Condition == INT_EXT_SIG_FAL)
			*ircSupervisor.GIFR |= 0x40;
		else if ((*SvPORTD.portPIN & INT_INT0_PIN_MASK) == 0 && ext0Condition == INT_EXT_SIG_LOW)
			*ircSupervisor.GIFR |= 0x40;

		if (*ircSupervisor.GIFR & 0x40)
			ircSupervisor.isExt0Exec = true;
		else
			ircSupervisor.isExt0Exec = false;
	}


	unsigned int ext1Condition = ((*ircSupervisor.MCUCR) >> 2) & 0x03;

	if (ircSupervisor.isExt1Enabeld){

		if (_checkPinFront(SvPORTD, INT_INT1_PIN_NO) != FRONT_NO && ext1Condition == INT_EXT_SIG_CHG)
			*ircSupervisor.GIFR |= 0x80;
		else if (_checkPinFront(SvPORTD, INT_INT1_PIN_NO) == FRONT_RIS && ext1Condition == INT_EXT_SIG_RIS)
			*ircSupervisor.GIFR |= 0x80;
		else if (_checkPinFront(SvPORTD, INT_INT1_PIN_NO) == FRONT_FAL && ext1Condition == INT_EXT_SIG_FAL)
			*ircSupervisor.GIFR |= 0x80;
		else if ((*SvPORTD.portPIN & INT_INT1_PIN_MASK) == 0 && ext1Condition == INT_EXT_SIG_LOW)
			*ircSupervisor.GIFR |= 0x80;

		if (*ircSupervisor.GIFR & 0x80)
			ircSupervisor.isExt1Exec = true;
		else
			ircSupervisor.isExt1Exec = false;
	}
	
}

void AVRSystem::_runInterupts()
{	
	if (ircSupervisor.isExt0Exec)
	{
		// Run EXT0
		_runInterupVector(INT_EXT0_ADDR);
		*ircSupervisor.GIFR &= ~GIFR_EXT0_MASK;
		return;
	}

	if (ircSupervisor.isExt1Exec)
	{
		// Run EXT1
		_runInterupVector(INT_EXT1_ADDR);
		*ircSupervisor.GIFR &= ~GIFR_EXT1_MASK;
		return;
	}

	if (ircSupervisor.isAdcExec)
	{
		// Run ADC
		_runInterupVector(INT_ADC_ADDR);
		*ircSupervisor.ADCSRA &= ~ADCSRA_ADIF_MASK;
		return;
	}
}

void AVRSystem::_runInterupVector(unsigned int addr)
{
	if (addr >= PROGRAM_MEMORY_SIZE_WORDS)
		return;

	if (!ircSupervisor.isINTEnabeld)
		return;

	//ramMem[CoreRegisterMap::SREG] &= ~SREG_I;
	
	*generalRegisters.sreg &= ~SREG_I;
	_push_word(generalRegisters.ProgramCounter);
	generalRegisters.ProgramCounter = addr;
}


/* DISASSEMBLY */

// Disassembly processing
void AVRSystem::_initListing()
{
	disassemblyListing = "";
}

void AVRSystem::_generateListing()
{
	char voidCmd[50];

	for (int progMemString = 0; progMemString < PROGRAM_MEMORY_SIZE_WORDS; progMemString++)
	{
		unsigned short cmd = progMem[progMemString];

		if (cmd == 0xFFFF)
		{
			disassemblyListing += "++\n";
			continue;
		}

		if (cmd == 0x0000)
		{
			disassemblyListing += "NOP\n";
			continue;
		}

		if (_analizeFullLenParamCmd(cmd))
			continue;

		if (_analizeSingleParamCmd(cmd))
			continue;

		if (_analizeDoubleParamCmd(cmd))
			continue;

		if (_analizeImmeditateParamCmd(cmd))
			continue;

		if (_analizeSpecParamCmd(cmd))
			continue;

		sprintf(voidCmd, "%x\n", cmd);
		disassemblyListing += voidCmd;
	}
}

bool AVRSystem::_analizeSingleParamCmd(unsigned short opcode)
{

	return false;
}

bool AVRSystem::_analizeDoubleParamCmd(unsigned short opcode)
{
	char cmdRepr[80];
	unsigned short cmd_twoWord = (opcode >> 10) & 0x003F;
	int R = 0;
	int Rd = 0;
	int Rr = 0;
	unsigned char ld_type = 0xFF;

	switch (cmd_twoWord)
	{

	case 0x03: // ADD | LSL
		Rd = ((opcode >> 4) & 0xF) | ((opcode >> 8) & 0x1) << 4;
		Rr = (opcode & 0xF) | ((opcode >> 9) & 0x1) << 4;

		sprintf(cmdRepr, "ADD | LSL R%d, R%d\n", Rd, Rr);
		disassemblyListing += cmdRepr;
		return true;

	case 0x07: // ADC | ROL
		Rd = ((opcode >> 4) & 0xF) | ((opcode >> 8) & 0x1) << 4;
		Rr = (opcode & 0xF) | ((opcode >> 9) & 0x1) << 4;

		sprintf(cmdRepr, "ADC | ROL R%d, R%d\n", Rd, Rr);
		disassemblyListing += cmdRepr;
		return true;

	case 0x6: // SUB
		Rd = ((opcode >> 4) & 0xF) | ((opcode >> 8) & 0x1) << 4;
		Rr = (opcode & 0xF) | ((opcode >> 9) & 0x1) << 4;

		sprintf(cmdRepr, "SUB R%d, R%d\n", Rd, Rr);
		disassemblyListing += cmdRepr;
		return true;

	case 0x2: // SBC
		Rd = ((opcode >> 4) & 0xF) | ((opcode >> 8) & 0x1) << 4;
		Rr = (opcode & 0xF) | ((opcode >> 9) & 0x1) << 4;

		sprintf(cmdRepr, "SBC R%d, R%d\n", Rd, Rr);
		return true;

	case 0x8: // AND | TST
		Rd = ((opcode >> 4) & 0xF) | ((opcode >> 8) & 0x1) << 4;
		Rr = (opcode & 0xF) | ((opcode >> 9) & 0x1) << 4;

		sprintf(cmdRepr, "AND | TST R%d\n", Rr);
		disassemblyListing += cmdRepr;
		return true;

	case 0x0A: // OR
		Rd = ((opcode >> 4) & 0xF) | ((opcode >> 8) & 0x1) << 4;
		Rr = (opcode & 0xF) | ((opcode >> 9) & 0x1) << 4;

		sprintf(cmdRepr, "OR R%d, R%d\n", Rd, Rr);
		disassemblyListing += cmdRepr;
		return true;

	case 0x9: // EOR | CLR
		Rd = ((opcode >> 4) & 0xF) | ((opcode >> 8) & 0x1) << 4;
		Rr = (opcode & 0xF) | ((opcode >> 9) & 0x1) << 4;

		if (Rd == Rr)
			sprintf(cmdRepr, "CLR R%d\n", Rd);
		else
			sprintf(cmdRepr, "EOR R%d, R%d\n", Rd, Rr);

		disassemblyListing += cmdRepr;
		return true;

	case 0x37: // COM | INC | DEC
		Rd = ((opcode >> 4) & 0xF) | ((opcode >> 8) & 0x1) << 4;
		Rr = (opcode & 0xF) | ((opcode >> 9) & 0x1) << 4;

		if (Rr == 0){ // COM

			sprintf(cmdRepr, "COM R%d\n", Rd);
			disassemblyListing += cmdRepr;
			return true;
		}

		if (Rr == 3) { // INC

			sprintf(cmdRepr, "INC R%d\n", Rd);
			disassemblyListing += cmdRepr;
			return true;
		}

		break;

	case 0x27: // MUL
		Rd = ((opcode >> 4) & 0xF) | ((opcode >> 8) & 0x1) << 4;
		Rr = (opcode & 0xF) | ((opcode >> 9) & 0x1) << 4;

		sprintf(cmdRepr, "MUL R%d, R%d\n", Rd, Rr);
		disassemblyListing += cmdRepr;
		return true;

	case 0x04: // CPSE
		Rd = ((opcode >> 4) & 0xF) | ((opcode >> 8) & 0x1) << 4;
		Rr = (opcode & 0xF) | ((opcode >> 9) & 0x1) << 4;

		sprintf(cmdRepr, "CPSE R%d, R%d\n", Rd, Rr);
		disassemblyListing += cmdRepr;
		return true;

	case 0x05: // CP
		Rd = ((opcode >> 4) & 0xF) | ((opcode >> 8) & 0x1) << 4;
		Rr = (opcode & 0xF) | ((opcode >> 9) & 0x1) << 4;

		sprintf(cmdRepr, "CP R%d, R%d\n", Rd, Rr);
		disassemblyListing += cmdRepr;
		return true;

	case 0x01: // CPC
		Rd = ((opcode >> 4) & 0xF) | ((opcode >> 8) & 0x1) << 4;
		Rr = (opcode & 0xF) | ((opcode >> 9) & 0x1) << 4;

		sprintf(cmdRepr, "CPC R%d, R%d\n", Rd, Rr);
		disassemblyListing += cmdRepr;
		return true;

	case 0x3F: // SBRC | SBRS
		Rd = ((opcode >> 4) & 0xF) | ((opcode >> 8) & 0x1) << 4;
		Rr = opcode & 0x7;

		if (!((opcode >> 9) & 0x1)) // SBRC
		{
			sprintf(cmdRepr, "SBRC R%d\n", Rd);
			disassemblyListing += cmdRepr;
			return true;
		}

		if (((opcode >> 9) & 0x1)) // SBRS
		{
			sprintf(cmdRepr, "SBRS R%d\n", Rr);
			disassemblyListing += cmdRepr;
			return true;
		}
		break;

	case 0x3C: // BRBS | BREQ | BRCS | BRLO | BRMI | BRLT | BRHS | BRTS | BRVS | BRIE
		Rr = opcode & 0x07;
		Rd = (opcode >> 3) & 0x7F;

		sprintf(cmdRepr, "BRBS | ... %d, %d\n", Rr, Rd);
		disassemblyListing += cmdRepr;
		return true;

	case 0x3D: // BRBC | BRNE | BRCC | BRSH | BRPL | BRGE | BRHC | BRTC | BRVC | BRID
		Rr = opcode & 0x07;
		Rd = (opcode >> 3) & 0x7F;

		sprintf(cmdRepr, "BRBC | ... %d, %d\n", Rr, Rd);
		disassemblyListing += cmdRepr;
		return true;

	case 0x3E: // BST | BLD
		Rd = (opcode >> 4) & 0x1F;
		Rr = opcode & 0x7;
		ld_type = (opcode >> 9) & 0x01;

		if (ld_type)
		{
			sprintf(cmdRepr, "BST R%d, %d\n", Rd, Rr);
			disassemblyListing += cmdRepr;
		}
		else
		{
			sprintf(cmdRepr, "BLD R%d, %d\n", Rd, Rr);
			disassemblyListing += cmdRepr;
		}

		return true;

	case 0x0B: // MOV
		Rd = ((opcode >> 4) & 0xF) | ((opcode >> 8) & 0x1) << 4;
		Rr = (opcode & 0xF) | ((opcode >> 9) & 0x1) << 4;

		sprintf(cmdRepr, "MOV R%d, R%d\n", Rd, Rr);
		disassemblyListing += cmdRepr;
		return true;

	case 0x24: // LD X   ST X Y Z	LPM
		ld_type = opcode & 0x0F;
		Rd = (opcode >> 0x0F) & 0x1F;

		if (!((opcode >> 9) & 0x01)) // LD
		{
			if (ld_type == 0xC) // X
			{
				sprintf(cmdRepr, "LD R%d, X\n", Rd);
				disassemblyListing += cmdRepr;
			}
			else if (ld_type == 0xD) // X
			{
				sprintf(cmdRepr, "LD R%d, X+\n", Rd);
				disassemblyListing += cmdRepr;
			}
			else if (ld_type == 0xE) // X
			{
				sprintf(cmdRepr, "LD R%d, -X\n", Rd);
				disassemblyListing += cmdRepr;
			}
			if (ld_type == 9) // Y
			{
				sprintf(cmdRepr, "LD R%d, Y+\n", Rd);
				disassemblyListing += cmdRepr;
			}
			else if (ld_type == 0xA) // Y
			{
				sprintf(cmdRepr, "LD R%d, -Y\n", Rd);
				disassemblyListing += cmdRepr;
			}
			else if (ld_type == 0x1) // Z
			{
				sprintf(cmdRepr, "LD R%d, Z+\n", Rd);
				disassemblyListing += cmdRepr;
			}
			else if (ld_type == 0x2) // Z
			{
				sprintf(cmdRepr, "LD R%d, -Z\n", Rd);
				disassemblyListing += cmdRepr;
			}
			else if (ld_type == 0x0) // LDS
			{
				//generalRegisters.ProgramCounter++; // Next byte is addr
				//generalRegisters.R[Rd] = ramMem[progMem[generalRegisters.ProgramCounter]];
			}
			else if (ld_type == 0x5) // LPM
			{
				sprintf(cmdRepr, "LPM R%d, Z+\n", Rd);
				disassemblyListing += cmdRepr;
			}
			else if (ld_type == 0x4) // LPM
			{
				sprintf(cmdRepr, "LPM R%d, Z\n", Rd);
				disassemblyListing += cmdRepr;
			}
			else if (ld_type == 0xF) // POP
			{
				sprintf(cmdRepr, "POP R%d\n", Rd);
				disassemblyListing += cmdRepr;
			}
		}

		if ((opcode >> 9) & 0x01) // ST X Y Z   STS
		{
			if (ld_type == 0xC) // X
			{
				sprintf(cmdRepr, "ST X, R%d\n", Rd);
				disassemblyListing += cmdRepr;
			}
			else if (ld_type == 0xD) // X
			{
				sprintf(cmdRepr, "ST X+, R%d\n", Rd);
				disassemblyListing += cmdRepr;
			}
			else if (ld_type == 0xE) // X
			{
				sprintf(cmdRepr, "ST -X, R%d\n", Rd);
				disassemblyListing += cmdRepr;
			}
			else if (ld_type == 9) // Y
			{
				sprintf(cmdRepr, "ST Y+, R%d\n", Rd);
				disassemblyListing += cmdRepr;
			}
			else if (ld_type == 0xA) // Y
			{
				sprintf(cmdRepr, "ST -Y, R%d\n", Rd);
				disassemblyListing += cmdRepr;
			}
			else if (ld_type == 0x1) // Z
			{
				sprintf(cmdRepr, "ST Z+, R%d\n", Rd);
				disassemblyListing += cmdRepr;
			}
			else if (ld_type == 0x2) // Z
			{
				sprintf(cmdRepr, "ST -Z, R%d\n", Rd);
				disassemblyListing += cmdRepr;
			}
			else if (ld_type == 0x0) // STS
			{
				//generalRegisters.ProgramCounter++; // Next byte is addr
				//ramMem[progMem[generalRegisters.ProgramCounter]] = generalRegisters.R[Rd];
			}
			else if (ld_type == 0xF) // PUSH
			{
				sprintf(cmdRepr, "PUSH R%d\n", Rd);
				disassemblyListing += cmdRepr;
			}
		}

		return true;

	case 0x20: // LD Y Z
		ld_type = opcode & 0x0F;
		Rd = (opcode >> 0x0F) & 0x1F;

		if (ld_type == 0x8)
		{
			sprintf(cmdRepr, "LD R%d, Y\n", Rd);
			disassemblyListing += cmdRepr;
		}
		else if (ld_type == 0x0)
		{
			sprintf(cmdRepr, "LD R%d, Z\n", Rd);
			disassemblyListing += cmdRepr;
		}

		return true;

	case 0x25: // LSR | ROR | ASR | SWAP | CALL | NEG | DEC
		ld_type = opcode & 0x0F;
		Rd = (opcode >> 4) & 0x1F;

		if (ld_type == 0x6) // LSR
		{
			sprintf(cmdRepr, "LSR R%d\n", Rd);
			disassemblyListing += cmdRepr;
			return true;
		}
		else if (ld_type == 0x7) // ROR
		{
			sprintf(cmdRepr, "ROR R%d\n", Rd);
			disassemblyListing += cmdRepr;
			return true;
		}
		else if (ld_type == 0x5) // ASR
		{
			sprintf(cmdRepr, "ASR R%d\n", Rd);
			disassemblyListing += cmdRepr;
			return true;
		}
		else if (ld_type == 0x2) // SWAP
		{
			sprintf(cmdRepr, "SWAP R%d\n", Rd);
			disassemblyListing += cmdRepr;
			return true;
		}
		//else if (((ld_type >> 1) & 0x07) == 0x07) // CALL
		//{
		//	sprintf(cmdRepr, "CALL ");
		//	disassemblyListing += cmdRepr;
		//	return true;
		//}
		else if (ld_type == 1) // NEG
		{
			sprintf(cmdRepr, "NEG R%d\n", Rd);
			disassemblyListing += cmdRepr;
			return true;
		}
		else if (ld_type == 0x0A){ // DEC

			sprintf(cmdRepr, "DEC R%d\n", Rd);
			disassemblyListing += cmdRepr;
			return true;
		}
		return false; // check for SBI cmd
	}

	return false;
}

bool AVRSystem::_analizeSpecParamCmd(unsigned short opcode)
{
	char cmdRepr[80];
	unsigned short cmd_short = (opcode >> 12) & 0xF;
	int R = 0;
	int K = 0;
	int Rd = 0;

	switch (cmd_short)
	{
	case 0x5: // SUBI
		K = opcode & 0xF | ((opcode >> 8) & 0xF) << 4;
		Rd = ((opcode >> 4) & 0xF) + 16;

		sprintf(cmdRepr, "SUBI R%d, %d\n", Rd, K);
		disassemblyListing += cmdRepr;
		return true;

	case 0x4: // SBCI
		K = opcode & 0xF | ((opcode >> 8) & 0xF) << 4;
		Rd = ((opcode >> 4) & 0xF) + 16;

		sprintf(cmdRepr, "SBCI R%d, %d\n", Rd, K);
		disassemblyListing += cmdRepr;
		return true;

	case 0x7: // ANDI
		K = opcode & 0xF | ((opcode >> 8) & 0xF) << 4;
		Rd = ((opcode >> 4) & 0xF) + 16;

		sprintf(cmdRepr, "ANDI R%d, %d\n", Rd, K);
		disassemblyListing += cmdRepr;
		return true;

	case 0x6: // ORI | SBR
		K = opcode & 0xF | ((opcode >> 8) & 0xF) << 4;
		Rd = ((opcode >> 4) & 0xF) + 16;

		sprintf(cmdRepr, "ORI R%d, %d\n", Rd, K);
		disassemblyListing += cmdRepr;
		return true;

	case 0xC: // RJMP
		K = opcode & 0xFFF;

		if (K & 0x800)
			K = (signed short)(K | 0xF000);

		sprintf(cmdRepr, "RJMP %d\n", K);
		disassemblyListing += cmdRepr;
		return true;

	case 0xD: // RCALL
		K = opcode & 0xFFF;

		if (K & 0x800)
			K = (signed short)(K | 0xF000);

		sprintf(cmdRepr, "RCALL %d\n", K);
		disassemblyListing += cmdRepr;
		return true;

	case 0x03: // CPI
		K = opcode & 0xF | ((opcode >> 8) & 0xF) << 4;
		Rd = ((opcode >> 4) & 0xF) + 16;

		sprintf(cmdRepr, "CPI R%d, %d\n", Rd, K);
		disassemblyListing += cmdRepr;
		return true;

	case 0x0E: // LDI | SER
		K = opcode & 0xF | ((opcode >> 8) & 0xF) << 4;
		Rd = ((opcode >> 4) & 0xF) + 16;

		sprintf(cmdRepr, "LDI R%d, %d\n", Rd, K);
		disassemblyListing += cmdRepr;
		return true;

	case 0x0A: // STS
		Rd = ((opcode >> 4) & 0xF) + 16;
		K = opcode & 0xF | ((opcode >> 9) & 0x3) << 4 | ((opcode >> 4) & 0x01) << 6 | (~((opcode >> 4) & 0x01)) << 6;

		sprintf(cmdRepr, "STS %d, R%d\n", K, Rd);
		disassemblyListing += cmdRepr;
		return true;

	case 0x0B: // IN | OUT
		K = opcode & 0xF | ((opcode >> 9) & 0x3) << 4;
		Rd = (opcode >> 4) & 0x1F;

		std::string regName = registerDict.at(K) + "\0";
		if (!((opcode >> 11) & 0x01))
		{
			sprintf(cmdRepr, "IN R%d, %s\n", Rd, regName.c_str());
			disassemblyListing += cmdRepr;
		}
		else if ((opcode >> 11) & 0x01)
		{
			sprintf(cmdRepr, "OUT %s, R%d\n", regName.c_str(), Rd);
			disassemblyListing += cmdRepr;
		}

		return true;
	}

	return false;
}

bool AVRSystem::_analizeImmeditateParamCmd(unsigned short opcode)
{

	char cmdRepr[80];
	unsigned short cmd_Immediate = (opcode >> 8) & 0xFF;
	int R = 0;
	unsigned char d = 0;
	unsigned char r = 0;
	int K = 0;
	unsigned short* regPair = NULL;

	switch (cmd_Immediate) //TODO: Check for only 4 upper REG pairs
	{
	case 0x96: // ADIW
		d = ((opcode >> 4) & 0x3) * 2 + 24;
		K = (opcode & 0xF) | ((opcode >> 6) & 0x3) << 4;

		sprintf(cmdRepr, "ADIW R%d:R%d, %d\n", d + 1, d, K);
		disassemblyListing += cmdRepr;
		return true;

	case 0x97: // SBIW
		d = ((opcode >> 4) & 0x3) * 2 + 24;
		K = (opcode & 0xF) | ((opcode >> 6) & 0x3) << 4;

		sprintf(cmdRepr, "SBIW R%d:R%d, %d\n", d + 1, d, K);
		disassemblyListing += cmdRepr;
		return true;

	case 0x02: // MULS
		d = ((opcode >> 4) & 0xF) + 16;
		r = (opcode & 0xF) + 16;

		sprintf(cmdRepr, "MULS R%d, R%d\n", d, r);
		disassemblyListing += cmdRepr;
		return true;

	case 0x3: // MULSU | FMUL | FMULS | FMULSU
		d = ((opcode >> 4) & 0x7) + 16;
		r = (opcode & 0x7) + 16;

		if (!(opcode & 0x8) && !(opcode & 0x80)){ // MULSU

			sprintf(cmdRepr, "MULSU R%d, R%d\n", d, r);
			disassemblyListing += cmdRepr;
			return true;
		}

		if ((opcode & 0x8) && !(opcode & 0x80)){ // FMUL

			sprintf(cmdRepr, "FMUL R%d, R%d\n", d, r);
			disassemblyListing += cmdRepr;
			return true;
		}

		if (!(opcode & 0x8) && (opcode & 0x80)){ // FMULS

			sprintf(cmdRepr, "FMULS R%d, R%d\n", d, r);
			disassemblyListing += cmdRepr;
			return true;
		}

		if ((opcode & 0x8) && (opcode & 0x80)){ // FMULSU

			sprintf(cmdRepr, "FMULSU R%d, R%d\n", d, r);
			disassemblyListing += cmdRepr;
			return true;
		}
		break;

	case 0x99: // SBIC
		d = (opcode >> 3) & 0x1F;
		r = opcode & 0x7;

		sprintf(cmdRepr, "SBIC %d, %d\n", d, r);
		disassemblyListing += cmdRepr;
		return true;

	case 0x9B: // SBIS
		d = (opcode >> 3) & 0x1F;
		r = opcode & 0x7;

		sprintf(cmdRepr, "SBIS %d, %d\n", d, r);
		return true;

	case 0x01: // MOVW
		d = ((opcode >> 4) & 0x0F) * 2;
		r = (opcode & 0x0F) * 2;

		sprintf(cmdRepr, "MOVW R%d:R%d, R%d,R%d\n", d + 1, d, r + 1, r);
		disassemblyListing += cmdRepr;
		return true;

	case 0x9A: // SBI
		d = (opcode >> 3) & 0x1F;
		r = opcode & 0x7;

		sprintf(cmdRepr, "SBI %d, %d\n", d, r);
		disassemblyListing += cmdRepr;
		return true;

	case 0x98: // CBI
		d = (opcode >> 3) & 0x1F;
		r = opcode & 0x7;

		sprintf(cmdRepr, "CBI %d, %d\n", d, r);
		disassemblyListing += cmdRepr;
		return true;

	case 0x94: // BSET | BCLR
		d = opcode & 0x04;
		r = (opcode >> 4) & 0x7;
		K = (opcode & 0x80) ? 1 : 0;

		if (K) // BSET
		{
			sprintf(cmdRepr, "BSET %d\n", r);
			disassemblyListing += cmdRepr;
		}
		else // BCLR
		{
			sprintf(cmdRepr, "BCLR %d\n", r);
			disassemblyListing += cmdRepr;
		}
		return true;
	}

	return false;
}

bool AVRSystem::_analizeFullLenParamCmd(unsigned short opcode)
{
	char cmdRepr[80];

	switch (opcode)
	{
	case 0x9409: // IJMP
		sprintf(cmdRepr, "IJMP\n");
		disassemblyListing += cmdRepr;
		return true;

	case 0x9509: // ICALL
		sprintf(cmdRepr, "ICALL\n");
		disassemblyListing += cmdRepr;
		return true;

	case 0x9508: // RET
		sprintf(cmdRepr, "RET\n");
		disassemblyListing += cmdRepr;
		return true;

	case 0x9518: // RETI
		sprintf(cmdRepr, "RETI\n");
		disassemblyListing += cmdRepr;
		return true;
	}

	return false;
}

std::string AVRSystem::disassembleMemmory()
{
	_initListing();
	_generateListing();

	return disassemblyListing;
}