using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MCU_F
{
    public interface ICore: IRegisterMap
    {
        string getCoreName();

        string getCoreVersion();

        void coreCLK();

        void loadProgram();

        byte[] readByteMemory(CoreMemoryTypes memType);

        UInt16[] readWordMemory(CoreMemoryTypes memType);

        void resetCore();

        void softResetCore();

        List<CoreMemoryInfo> getCoreMemoryInfo();

        UInt32 readRegister(int regAddr);

        void writeRegister(int regAddr, byte value);
    }
}
