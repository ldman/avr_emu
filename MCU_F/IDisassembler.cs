using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MCU_F
{
    public interface IDisassembler
    {
        List<String> GetDisassemblyListing();
    }
}
