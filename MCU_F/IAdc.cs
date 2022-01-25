using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MCU_F
{
    public interface IAdc
    {
        void writeToADC(double voltage, UInt32 chnNo);
    }
}
