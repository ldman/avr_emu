using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MCU_CoreTests
{
    public static class TestSettings
    {
        /**
         * For each test hex file needed: 
         *   
         *  1. Put hex file in the folder 'hex'.
         *  3. Go to the VS Properties window for hex file.
         *  4. Set Build Action to None and Copy to Output Directory to Copy always or Copy if newer.
         *  
         */

        public static string HexDirectory = @"hex\";
        public static string GeneralArithTestHexName = @"arithTest.hex";
        public static string SkeletonTestHexName = @"skeletonTest.hex";
        public static string TCount0HexName = @"tcount0Test.hex";
        public static string PortOutHexName = @"portOutTest.hex";
        public static string PortInHexName = @"portInTest.hex";
        public static string IrcExtHexName = @"ircText.hex";
        public static string AdcHexName = @"adcConvTest.hex";
    }
}
