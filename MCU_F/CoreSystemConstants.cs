using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MCU_F
{
    public enum CoreMemoryTypes
    {
        PROG_MEM,
        RAM_MEM,
        EEPROM_MEM,
        REGISTER_MEM
    }

    public enum CoreMemoryArch
    {
        BYTE_MEMORY,
        WORD_MEMORY
    }

    public struct CoreMemoryInfo
    {
        public UInt32 memorySize;
        public UInt32 memoryUsed;
        public CoreMemoryArch memoryArchitecture;
        public CoreMemoryTypes memoryType;
    }
}
