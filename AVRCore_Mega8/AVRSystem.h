#pragma once

#include "CoreSettings.h"
#include "RegiserMap_Mega8.h"
#include "CoreRegisterMap.h"
#include <memory>
#include <stdlib.h>
#include <string>
#include <vector>

class AVRSystem
{
public:
	AVRSystem();
	~AVRSystem();

	const unsigned short* pProgMemory;
	const unsigned char* pRamMemory;
	const unsigned char* pEepromMemory;

	void _initProgMemoryWithTestValues();
	void _initRamMemoryWithTestValues();
	void _initEepromMemoryWithTestValues();

	void reset();
	void softReset();
	
	unsigned char readPortA();
	unsigned char readPortB();
	unsigned char readPortC();
	unsigned char readPortD();

	unsigned char readPortConfigA();
	unsigned char readPortConfigB();
	unsigned char readPortConfigC();
	unsigned char readPortConfigD();

	unsigned short readRegister(int addr);
	unsigned char readGeneralRegister(int add);

	void writePortA(unsigned char data);
	void writePortB(unsigned char data);
	void writePortC(unsigned char data);
	void writePortD(unsigned char data);

	void writeToADCChan(double voltage, unsigned int chnNo); // Write volatge value to ADC chanel

	void clkHandler(); // Give XTAL strobe for execution core

	unsigned int usedProgramMemory;

	std::string disassembleMemmory();

private:
	unsigned short progMem[PROGRAM_MEMORY_SIZE_WORDS];	
	unsigned char ramMem[RAM_MEMORY_SIZE_BYTES];
	unsigned char eepromMem[EEPROM_MEMORY_SIZE_BYTES];
	unsigned char voidCmdCyclesCount;
	bool skipNext;
		
	CPURegisterMap generalRegisters;
	PortSupervisor SvPORTA;
	PortSupervisor SvPORTB;
	PortSupervisor SvPORTC;
	PortSupervisor SvPORTD;
	TimerController timersController; // For timers 0/1
	TimerController exTimerController; // For timer 2
	TimerCounterType0 timer0;
	TimerCounterType1 timer1;
	TimerCounterType0 timer2;
	AdcSupervisor adcSupervisor;
	IRCSupervisor ircSupervisor;

	bool runFullLenParamCmd(unsigned short opcode);
	bool runSingleParamCmd(unsigned short opcode);
	bool runDoubleParamCmd(unsigned short opcode);
	bool runSpecParamCmd(unsigned short opcode);
	bool runImmeditateParamCmd(unsigned short opcode);

	std::string disassemblyListing;

	// Stack processing functions
	inline void _push_word(unsigned short val);
	inline unsigned short _pop_word();

	inline void _push_byte(unsigned char val);
	inline unsigned char _pop_byte();

	inline void _setStackPointer(unsigned short);
	inline unsigned short _getStackPonter();


	// Reset processing
	void _resetRegisters();
	void _resetRegisterFile();


	// Implementation flow
	inline void _mcuStepFunc();
	inline void _prescalerStepFunc();
	inline void _timersStepFunc(TimerCounterType0*, TimerController*);
	inline void _timersStepFunc(TimerCounterType1*, TimerController*);


	// Port processing functions
	inline void _writePort(PortSupervisor&, unsigned char data);
	inline unsigned char _readPort(PortSupervisor&);
	inline unsigned char _readPinPort(PortSupervisor&, int pin);
	inline unsigned char _readPortConfig(PortSupervisor&);
	inline int _checkPinFront(PortSupervisor&, int pin);

	inline void _portSupervisorStepFunc();
	inline void _updatePortRegisters();


	// Flag processing functions
	void _check_SREG_H(unsigned char Rd, unsigned char Rr);
	
	void _check_SREG_V(int R);
	void _check_SREG_V_word(int R);
	
	void _check_SREG_C(int R);
	void _check_SREG_C_word(int R);
	
	void _check_SREG_N(unsigned char R);
	void _check_SREG_N_word(unsigned short R);

	void _check_SREG_Z(unsigned short R);
	
	void _check_SREG_S();


	// ADC processing functions
	void _adcUpdateState(); // Update supervisor & registers
	void _adcStepFunc(); // Emulate ADC conversion


	// INT processing functions
	inline void _checkInteruptsEnabeld(); // Update supervisor & registers
	inline void _runInterupts(); // Run interupts marked as exec
	inline void _runInterupVector(unsigned int addr); // Run interupt logic
	

	// Disassembly processing
	std::vector<std::string> registerDict;
	void _generateListing();
	inline void _initListing();
	inline bool _analizeSingleParamCmd(unsigned short opcode);
	inline bool _analizeDoubleParamCmd(unsigned short opcode);
	inline bool _analizeSpecParamCmd(unsigned short opcode);
	inline bool _analizeImmeditateParamCmd(unsigned short opcode);
	inline bool _analizeFullLenParamCmd(unsigned short opcode);

};

