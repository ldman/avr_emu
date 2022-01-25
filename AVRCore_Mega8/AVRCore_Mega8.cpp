// ֳכאגםûי DLL-פאיכ.

#include "stdafx.h"

#include "AVRCore_Mega8.h"

AVRCore_Mega8::AVR8_Core::AVR8_Core()
{
	coreName = gcnew System::String("AVR Mega 8");
	coreVersion = gcnew System::String("0.1p");	

	avrSyst = new AVRSystem();
	progMemWrapper = gcnew array<UInt16>(PROGRAM_MEMORY_SIZE_WORDS);
	ramMemoryWrapper = gcnew array<Byte>(RAM_MEMORY_SIZE_BYTES);
	eepromMemoryWrapper = gcnew array<Byte>(EEPROM_MEMORY_SIZE_BYTES);
	fileRegisters = gcnew array<Byte>(REGISTERS_END - REGISTERS_BEGIN);

	portDataWrapper = gcnew array<Byte>(TOTAL_PORTS);
}

AVRCore_Mega8::AVR8_Core::~AVR8_Core()
{
	
}

/* get core info data */
System::String^ AVRCore_Mega8::AVR8_Core::getCoreName() { return coreName; }
System::String^ AVRCore_Mega8::AVR8_Core::getCoreVersion() { return coreVersion; }
UInt32 AVRCore_Mega8::AVR8_Core::getProgMemorySizeBytes() { return PROGRAM_MEMORY_SIZE_BYTES; }
UInt32 AVRCore_Mega8::AVR8_Core::getProgMemorySizeWords() { return PROGRAM_MEMORY_SIZE_WORDS; }
UInt32 AVRCore_Mega8::AVR8_Core::getRamMemorySizeBytes() { return RAM_MEMORY_SIZE_BYTES; }
UInt32 AVRCore_Mega8::AVR8_Core::getEEPROMemorySizeBytes() { return EEPROM_MEMORY_SIZE_BYTES; }


/* core timer tick event */
void AVRCore_Mega8::AVR8_Core::clk()
{
	avrSyst->clkHandler();
}

/* core reset */
void AVRCore_Mega8::AVR8_Core::resetCore()
{
	progMemWrapper->Initialize();
	ramMemoryWrapper->Initialize();
	eepromMemoryWrapper->Initialize();

	avrSyst->reset();
}

void AVRCore_Mega8::AVR8_Core::softReset()
{
	avrSyst->softReset();
}

/* acces to unmanaged avrCore program memory*/
array<UInt16>^ AVRCore_Mega8::AVR8_Core::getProgMemoryWords()
{
	pin_ptr<Void> wrapPtr = &progMemWrapper[0];
	memcpy_s(wrapPtr, PROGRAM_MEMORY_SIZE_BYTES, (void*)avrSyst->pProgMemory, PROGRAM_MEMORY_SIZE_BYTES);

	return progMemWrapper;
}

/* acces to unmanaged avrCore ram memory */
array<Byte>^ AVRCore_Mega8::AVR8_Core::getRamMemoryBytes()
{
	pin_ptr<Void> wrapPtr = &ramMemoryWrapper[0];
	memcpy_s(wrapPtr, RAM_MEMORY_SIZE_BYTES, (void*)avrSyst->pRamMemory, RAM_MEMORY_SIZE_BYTES);

	// Extract file registers part of RAM
	System::Array::Copy(ramMemoryWrapper, fileRegisters, fileRegisters->Length);

	return ramMemoryWrapper;
}

/* acces to file registers part of avrCore */
array<Byte>^ AVRCore_Mega8::AVR8_Core::getFileRegisters()
{
	return fileRegisters;
}

/* acces to unmanaged avrCore eeprom memory */
array<Byte>^ AVRCore_Mega8::AVR8_Core::getEepromMemoryBytes()
{
	pin_ptr<Void> wrapPtr = &eepromMemoryWrapper[0];
	memcpy_s(wrapPtr, EEPROM_MEMORY_SIZE_BYTES, (void*)avrSyst->pEepromMemory, EEPROM_MEMORY_SIZE_BYTES);

	return eepromMemoryWrapper;
}

/* acces to ports data*/
array<Byte>^ AVRCore_Mega8::AVR8_Core::getPortData()
{
	portDataWrapper[PORT_A] = static_cast<Byte>(avrSyst->readPortA());
	portDataWrapper[PORT_B] = static_cast<Byte>(avrSyst->readPortB());
	portDataWrapper[PORT_C] = static_cast<Byte>(avrSyst->readPortC());
	portDataWrapper[PORT_D] = static_cast<Byte>(avrSyst->readPortD());

	return portDataWrapper;
}

/* load program to memory*/
void AVRCore_Mega8::AVR8_Core::loadHex(array<Byte>^ hex)
{
	int hex_size = hex->Length;

	if (hex_size > PROGRAM_MEMORY_SIZE_BYTES)
		hex_size = PROGRAM_MEMORY_SIZE_BYTES;

	pin_ptr<Void> pHex = &hex[0];
	memcpy_s((void*)avrSyst->pProgMemory, PROGRAM_MEMORY_SIZE_BYTES,  pHex, hex_size);

	avrSyst->usedProgramMemory = hex_size;
	avrSyst->softReset(); // Reset registers after firmware uploading
}

/* size of used program memory */
UInt32 AVRCore_Mega8::AVR8_Core::getUsedProgramMemorySizeBytes()
{
	UInt32 result = avrSyst->usedProgramMemory;
	return result;
}

UInt32 AVRCore_Mega8::AVR8_Core::getUsedProgramMemorySizeWords()
{
	UInt32 result = avrSyst->usedProgramMemory / 2;
	return result;
}

UInt32 AVRCore_Mega8::AVR8_Core::getRegisterValue(int registerAddr)
{
	UInt32 result = avrSyst->readRegister(registerAddr);
	return result;	
}

UInt32 AVRCore_Mega8::AVR8_Core::getGeneralRegisterValue(int registerAddr)
{
	UInt32 result = avrSyst->readGeneralRegister(registerAddr);
	return result;
}

 /* get register bounds in memory */
UInt32  AVRCore_Mega8::AVR8_Core::settings_getGeneralRegistersStartAddr() { UInt32 bound = 0x00; return bound; }
UInt32  AVRCore_Mega8::AVR8_Core::settings_getGeneralRegistersEndAddr(){ UInt32 bound = REGISTERS_BEGIN; return bound; }
UInt32  AVRCore_Mega8::AVR8_Core::settings_getFileRegistersStartAddr() { UInt32 bound = REGISTERS_BEGIN; return bound; }
UInt32  AVRCore_Mega8::AVR8_Core::settings_getFileRegistersEndAddr() { UInt32 bound = REGISTERS_END; return bound; }

/* port data access */
Byte AVRCore_Mega8::AVR8_Core::readPortB() { return static_cast<Byte>(avrSyst->readPortB()); }
Byte AVRCore_Mega8::AVR8_Core::readPortC() { return static_cast<Byte>(avrSyst->readPortC()); }
Byte AVRCore_Mega8::AVR8_Core::readPortD() { return static_cast<Byte>(avrSyst->readPortD()); }

void AVRCore_Mega8::AVR8_Core::writePortB(Byte data) { avrSyst->writePortB(static_cast<unsigned char>(data)); }
void AVRCore_Mega8::AVR8_Core::writePortC(Byte data) { avrSyst->writePortC(static_cast<unsigned char>(data)); }
void AVRCore_Mega8::AVR8_Core::writePortD(Byte data) { avrSyst->writePortD(static_cast<unsigned char>(data)); }

/* ADC access */
void AVRCore_Mega8::AVR8_Core::writeADC(double voltage, unsigned int chnNo)
{
	avrSyst->writeToADCChan(voltage, chnNo);
}

/* Disassembly */
System::String^ AVRCore_Mega8::AVR8_Core::disassembleMemory()
{
	std::string listing = avrSyst->disassembleMemmory();
	return gcnew System::String((char*)listing.c_str());
}