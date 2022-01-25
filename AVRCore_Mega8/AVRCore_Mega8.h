// AVRCore_Mega8.h

#pragma once

using namespace System;

#include "Messages.h"
#include "CoreSettings.h"
#include "AVRSystem.h"
#include "CoreRegisterMap.h"

namespace AVRCore_Mega8 {
	
	public ref struct RegInfo {
		System::String^ regName;
		UInt32 regAddr;
		UInt32 regValue;
	};

	public ref class AVR8_Core
	{
	public:
		AVR8_Core();					
		~AVR8_Core();

		System::String^ getCoreName();
		System::String^ getCoreVersion();
		System::String^ disassembleMemory();

		array<UInt16>^ getProgMemoryWords();
		array<Byte>^ getRamMemoryBytes();
		array<Byte>^ getEepromMemoryBytes();
		array<Byte>^ getPortData();

		array<Byte>^ getFileRegisters();		 

		UInt32 getProgMemorySizeBytes();
		UInt32 getProgMemorySizeWords();
		UInt32 getRamMemorySizeBytes();
		UInt32 getEEPROMemorySizeBytes();

		UInt32 getUsedProgramMemorySizeBytes();
		UInt32 getUsedProgramMemorySizeWords();

		UInt32 getRegisterValue(int registerAddr);		
		UInt32 getGeneralRegisterValue(int registerAddr);

		UInt32 settings_getGeneralRegistersStartAddr();
		UInt32 settings_getGeneralRegistersEndAddr();
		UInt32 settings_getFileRegistersStartAddr();
		UInt32 settings_getFileRegistersEndAddr();

		Byte readPortB();
		Byte readPortC();
		Byte readPortD();

		void writePortB(Byte data);
		void writePortC(Byte data);
		void writePortD(Byte data);

		void writeADC(double voltage, unsigned int chnNo);

		void clk();
		void resetCore();
		void softReset();

		void loadHex(array<Byte>^ hex);

	private:		
		System::String^ coreName;
		System::String^ coreVersion;		

		AVRSystem *avrSyst;

		array<UInt16>^ progMemWrapper;
		array<Byte>^ ramMemoryWrapper;
		array<Byte>^ eepromMemoryWrapper;
		array<Byte>^ portDataWrapper;

		array<Byte>^ fileRegisters;
		
	};

	// Provides register information
	class CoreRegisterMap;
}
