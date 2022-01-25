using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MCU_F
{
    public struct RegisterData 
    {
        public string registerName;
        public int registerAddr;
        public uint registerVal;

        public override string ToString()
        {
            return string.Format("{0}[0x{1:x}]\t{2}", registerName, registerAddr, registerVal);
        }
    }

    public interface IRegisterMap
    {
        List<RegisterData> getRegisters();

        Dictionary<string, int> getRegistersMap();

        int getRegAddrByName(string regname);

        void updateRegisters();
    }
}
