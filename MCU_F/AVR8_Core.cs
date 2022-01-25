using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.IO;
using System.Windows.Forms;
using System.Reflection;

using AVRCore_Mega8;

namespace MCU_F
{
    public class AVR8Core : ICore, IDisposable, IAdc, IGeneralIO, IDisassembler
    {
        private AVR8_Core mcuCore;
       
        private List<CoreMemoryInfo> coreMemory = new List<CoreMemoryInfo>();
        private List<RegisterData> registersData = new List<RegisterData>();
        private List<String> programDisassembly = new List<String>();
        private Dictionary<string, int> registersMap = new Dictionary<string,int>();
        private uint totalFileRegistersCount;
        private IOInfo GeneralIOInfo = new IOInfo();

        private bool disposed = false;

        public AVR8Core()
        {
            mcuCore = new AVR8_Core();
            mcuCore.resetCore();

            /* Loading file registers information */
            totalFileRegistersCount = 0;
            initRegisterData();
            totalFileRegistersCount = (uint)registersMap.Count;


            /* Loading memory information */
            CoreMemoryInfo progMemory;
            progMemory.memoryArchitecture = CoreMemoryArch.WORD_MEMORY;
            progMemory.memoryType = CoreMemoryTypes.PROG_MEM;
            progMemory.memorySize = mcuCore.getProgMemorySizeWords();
            progMemory.memoryUsed = mcuCore.getUsedProgramMemorySizeBytes();

            CoreMemoryInfo ramMemory;
            ramMemory.memoryArchitecture = CoreMemoryArch.BYTE_MEMORY;
            ramMemory.memoryType = CoreMemoryTypes.RAM_MEM;
            ramMemory.memorySize = mcuCore.getRamMemorySizeBytes();
            ramMemory.memoryUsed = 0;

            CoreMemoryInfo eepromMemory;
            eepromMemory.memoryArchitecture = CoreMemoryArch.BYTE_MEMORY;
            eepromMemory.memoryType = CoreMemoryTypes.EEPROM_MEM;
            eepromMemory.memorySize = mcuCore.getEEPROMemorySizeBytes();
            eepromMemory.memoryUsed = 0;

            CoreMemoryInfo fileRegisterMemory;
            fileRegisterMemory.memoryArchitecture = CoreMemoryArch.BYTE_MEMORY;
            fileRegisterMemory.memoryType = CoreMemoryTypes.REGISTER_MEM;
            fileRegisterMemory.memorySize = totalFileRegistersCount;
            fileRegisterMemory.memoryUsed = fileRegisterMemory.memorySize;

            coreMemory.Clear();
            coreMemory.Add(progMemory);
            coreMemory.Add(ramMemory);
            coreMemory.Add(eepromMemory);
            coreMemory.Add(fileRegisterMemory);

            programDisassembly.Clear();
        }

        public string getCoreName() { return mcuCore.getCoreName(); }
        
        public string getCoreVersion() { return mcuCore.getCoreVersion(); }
        
        public void coreCLK() { mcuCore.clk(); }
        
        public List<CoreMemoryInfo> getCoreMemoryInfo() { return coreMemory; }
        
        public void resetCore() { mcuCore.resetCore(); }

        public void softResetCore() { mcuCore.softReset();  }

        public uint readRegister(int regAddr) 
        {
            if (regAddr >= mcuCore.settings_getFileRegistersStartAddr())
                return mcuCore.getRegisterValue(regAddr);
            else if (regAddr >= mcuCore.settings_getGeneralRegistersStartAddr())
                return mcuCore.getGeneralRegisterValue(regAddr);
            else return 0;
        }
        
        public byte[] getHexMemoryModelByte(string hexFilePath)
        {
            byte[] flashMemory = new byte[mcuCore.getProgMemorySizeBytes()];
            for (int i = 0; i < flashMemory.Length; i++)
                flashMemory[i] = 0xFF;

            StreamReader sr = new StreamReader(hexFilePath);

            String line;
            while (!sr.EndOfStream)
            {
                line = sr.ReadLine();
                if (line[0] == ':') // Find intel hex string begin
                {
                    int size = Convert.ToInt32(line.Substring(1, 2), 16);
                    int addr = Convert.ToInt16(line.Substring(3, 4), 16);
                    int type = Convert.ToInt32(line.Substring(7, 2), 16);
                    string data = line.Substring(9, size * 2);

                    switch (type)
                    {
                        case 0x00: // Data record
                            int pos = 0;
                            for (int i = 0; i < size; i++)
                            {
                                flashMemory[addr + i] = Convert.ToByte(data.Substring(pos, 2), 16);
                                pos += 2;
                            }
                            break;
                    }
                }
            }

            return flashMemory;
        }

        public ushort[] getHexMemoryModelWord(string hexFilePath)
        {
            byte[] byteModel = getHexMemoryModelByte(hexFilePath);

            ushort[] wordModel = new ushort[byteModel.Length/2];

            int pos = 0;
            for(int i = 0; i < wordModel.Length; i++)
            {
                wordModel[i] = (ushort)((int)byteModel[pos] | ((int)byteModel[pos+1] << 8));
                pos += 2;
            }

            return wordModel;
        }

        private void placeHex(string hexFilePath)
        {
            byte[] flashMemory = getHexMemoryModelByte(hexFilePath);
            mcuCore.softReset();
            mcuCore.loadHex(flashMemory);
        }

        public void loadProgram() 
        {
            OpenFileDialog openDialog = new OpenFileDialog();
   
            if(openDialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                placeHex(openDialog.FileName);

                for(int i = 0; i < coreMemory.Count; i++)                
                    if(coreMemory[i].memoryType == CoreMemoryTypes.PROG_MEM)
                    {
                        var memData = coreMemory[i];
                        memData.memoryUsed = mcuCore.getUsedProgramMemorySizeWords();
                        coreMemory[i] = memData;
                    }                                                               
            }
        }
        
        // Load hex by filePath, used for test and autoloading
        public void loadProgram(string filePath) { placeHex(filePath); }

        public byte[] readByteMemory(CoreMemoryTypes memType)
        {
            switch(memType)
            {
                case CoreMemoryTypes.PROG_MEM:
                    throw new NotImplementedException();
                case CoreMemoryTypes.RAM_MEM:
                    return mcuCore.getRamMemoryBytes();
                case CoreMemoryTypes.EEPROM_MEM:
                    return mcuCore.getEepromMemoryBytes();
            }

            return null;
        }

        public ushort[] readWordMemory(CoreMemoryTypes memType)
        {
            switch(memType)
            {
                case CoreMemoryTypes.PROG_MEM:
                    return mcuCore.getProgMemoryWords();
                case CoreMemoryTypes.RAM_MEM:
                    throw new NotImplementedException();
                case CoreMemoryTypes.EEPROM_MEM:
                    throw new NotImplementedException();
            }

            return null;
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposed)
                return;
            
            if (disposing)
            {
                mcuCore.Dispose();
            }

            coreMemory = null;
            registersData = null;
            registersMap = null;

            GC.Collect();
            disposed = true;
        }

        public List<RegisterData> getRegisters() { return registersData; }
        
        public void updateRegisters()
        {
            List<RegisterData> updatetdList = new List<RegisterData>();

            foreach(RegisterData regDat in registersData)
            {
                RegisterData updatedReg = regDat;
                updatedReg.registerVal = mcuCore.getRegisterValue(regDat.registerAddr);
                updatetdList.Add(updatedReg);
            }

            registersData = updatetdList;
        }

        private void initRegisterData() // Using reflection to load register map data in List
        {
            Type regMapType = typeof(CoreRegisterMap);
            MemberInfo[] memberInfo = regMapType.GetMembers();

            foreach (MemberInfo regInfo in memberInfo)
            {
                FieldInfo fieldInfo = typeof(CoreRegisterMap).GetField(regInfo.Name);

                if (fieldInfo == null)
                    continue;

                if (fieldInfo.FieldType.FullName == "System.Int32" && (regInfo.Name != "REGISTER_START" && regInfo.Name != "REGISTER_END"))
                {
                    RegisterData regData;
                    regData.registerName = regInfo.Name;
                    regData.registerAddr = (int)fieldInfo.GetValue(null);
                    regData.registerVal = 0;

                    registersData.Add(regData);
                    registersMap.Add(regInfo.Name, (int)fieldInfo.GetValue(null));
                }
            }
        }

        public Dictionary<string, int> getRegistersMap() { return registersMap;  }

        public int getRegAddrByName(string regname)
        {
            try
            {
                return registersMap[regname];
            }
            catch(Exception ex) {
                return 0;
            }
        }

        public void writeRegister(int regAddr, byte value)
        {
            throw new NotImplementedException();
        }

        public void writeToADC(double voltage, uint chnNo)
        {
            mcuCore.writeADC(voltage, chnNo);
        }

        public uint readPort(byte id)
        {
            switch (id)
            {
                case 0xB:
                    return mcuCore.readPortB();

                case 0xC:
                    return mcuCore.readPortC();

                case 0xD:
                    return mcuCore.readPortD();
            }

            // Raise exception

            return 0x00;
        }

        public void writePort(byte id, uint data)
        {
            byte portData = (byte)(data & 0xFF);

            switch (id)
            {
                case 0xB: mcuCore.writePortB(portData); return;
                case 0xC: mcuCore.writePortC(portData); return;
                case 0xD: mcuCore.writePortD(portData); return;
            }

        }

        public IOInfo getGeneralIOInfo()
        {
            return GeneralIOInfo;
        }

        public List<string> GetDisassemblyListing()
        {
            programDisassembly.Clear();
            String rawListing = mcuCore.disassembleMemory();
            string[] cmds = rawListing.Split('\n');
            foreach (string cmd in cmds)
                programDisassembly.Add(cmd);

            return programDisassembly;
        }
    }
}
