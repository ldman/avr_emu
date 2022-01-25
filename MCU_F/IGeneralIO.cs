using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MCU_F
{

    public struct IOInfo
    {
        public byte bitsPerPort; // Port bit capacity
        public uint portCount; // Total GPIO ports
        public byte[] portId; // GPIO Id's
    }

    public interface IGeneralIO
    {
         uint readPort(byte id);

         void writePort(byte id, uint data);

         IOInfo getGeneralIOInfo();
    }
}
