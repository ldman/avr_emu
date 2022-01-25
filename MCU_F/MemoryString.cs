using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MCU_F
{
    struct MemoryString
    {
        public string hexView;
        public string cmdView;
        public string cmdAddr;

        public int hexCmd; // To be used in disassembly
    }
}
