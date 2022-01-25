using System;
using System.IO;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using MCU_F;

namespace MCU_CoreTests
{
    [TestClass]
    public class GeneralMCUTest
    {

        public void executeCore(string hexPath, int clocks, AVR8Core core)
        {
            core.loadProgram(hexPath);

            for (int i = 0; i < clocks; i++)
                core.coreCLK();

            core.updateRegisters();
        }

        public void executeCoreWithData(string hexPath, int clocks, AVR8Core core, byte portA = 0, byte portB = 0, byte portC = 0, byte portD = 0, int clockInterval = 0)
        {
            core.loadProgram(hexPath);

            for (int i = 0; i < clocks; i++)
            {
                core.coreCLK();

                if(clockInterval != 0 && i % clockInterval == 0)
                {
                    core.writePort(0xB, portB);
                    core.writePort(0xC, portC);
                    core.writePort(0xD, portD);
                }
            }

            core.updateRegisters();
        }

        [TestMethod]
        public void TestSceleton() // Used to test main implementation flow and RJMP cmd
        {
            int clocks = 100;
            byte target_pos = 0x0E;
            string hexPath = TestSettings.HexDirectory + TestSettings.SkeletonTestHexName;
            Assert.IsTrue(File.Exists(hexPath), String.Format("Hex File not foud! [{0}]", hexPath));

            AVR8Core core = new AVR8Core();
            executeCore(hexPath, clocks, core);

            byte pos = (byte)core.readRegister(core.getRegAddrByName("REG_PC"));
            Assert.AreEqual(target_pos, pos);
        }

        [TestMethod]
        public void TestArith() // Used to test common instructions for overview
        {
            int clocks = 200;
            int regAddr = 30;
            byte target = 0x41;
            string hexPath = TestSettings.HexDirectory + TestSettings.GeneralArithTestHexName;
            Assert.IsTrue(File.Exists(hexPath), String.Format("Hex File not foud! [{0}]", hexPath));

            AVR8Core core = new AVR8Core();
            executeCore(hexPath, clocks, core);            
            
            byte result = (byte)core.readRegister(regAddr);
            Assert.AreEqual(target, result);
        }

        [TestMethod]
        public void HexMemoryModelTest()
        {
            AVR8Core core = new AVR8Core();

            Assert.AreNotEqual(null, core);

            string hexPath = TestSettings.HexDirectory + TestSettings.GeneralArithTestHexName;
            Assert.IsTrue(File.Exists(hexPath), String.Format("Hex File not foud! [{0}]", hexPath));

            ushort[] hexMemoryModel = core.getHexMemoryModelWord(hexPath);
            core.loadProgram(hexPath);
            ushort[] coreModel = core.readWordMemory(CoreMemoryTypes.PROG_MEM);

            Assert.IsTrue(hexMemoryModel.Length == coreModel.Length);

            for (int i = 0; i < hexMemoryModel.Length; i++)
                Assert.IsTrue(hexMemoryModel[i] == coreModel[i]);
        }

        [TestMethod]
        public void TCount0Test()
        {
            int clocks = 1500;
            byte timeCount = 0x00;
            byte r19 = 0x01;
            byte r20 = 0x01;

            AVR8Core core = new AVR8Core();
            Assert.AreNotEqual(null, core);

            string hexPath = TestSettings.HexDirectory + TestSettings.TCount0HexName;
            Assert.IsTrue(File.Exists(hexPath), String.Format("Hex File not foud! [{0}]", hexPath));

            executeCore(hexPath, clocks, core);
            byte resultCount = (byte)core.readRegister(16);
            byte resultR19 = (byte)core.readRegister(19);
            byte resultR20 = (byte)core.readRegister(20);

            Assert.AreEqual(r19, resultR19, "R19 value incorrect!");
            Assert.AreEqual(r20, resultR20, "R20 value incorrect!");
            Assert.AreEqual(timeCount, resultCount, "Timer value incorect!");

        }

        [TestMethod]
        public void PortOutTest()
        {
            int clocks = 250;
            byte PORTB_target = 0xBB;
            byte PORTC_target = 0x0C;
            byte PORTD_target = 0xDD;

            byte DDRB_target = 0xFF;
            byte DDRC_target = 0x3F;
            byte DDRD_target = 0xFF;

            AVR8Core core = new AVR8Core();
            Assert.AreNotEqual(null, core);

            string hexPath = TestSettings.HexDirectory + TestSettings.PortOutHexName;
            Assert.IsTrue(File.Exists(hexPath), String.Format("Hex File not foud! [{0}]", hexPath));

            executeCore(hexPath, clocks, core);
            byte PORTB_result = (byte)core.readRegister(core.getRegAddrByName("PORTB"));
            byte PORTC_result = (byte)core.readRegister(core.getRegAddrByName("PORTC"));
            byte PORTD_result = (byte)core.readRegister(core.getRegAddrByName("PORTD"));

            byte DDRB_result = (byte)core.readRegister(core.getRegAddrByName("DDRB"));
            byte DDRC_result = (byte)core.readRegister(core.getRegAddrByName("DDRC"));
            byte DDRD_result = (byte)core.readRegister(core.getRegAddrByName("DDRD"));

            Assert.AreEqual(PORTB_target, PORTB_result, String.Format("PORTB incorect {0} | {1}", PORTB_result, PORTB_target));
            Assert.AreEqual(PORTC_target, PORTC_result, String.Format("PORTC incorect {0} | {1}", PORTC_result, PORTC_target));
            Assert.AreEqual(PORTD_target, PORTD_result, String.Format("PORTD incorect {0} | {1}", PORTD_result, PORTD_target));

            Assert.AreEqual(DDRB_target, DDRB_result, String.Format("DDRB incorect {0} | {1}", DDRB_result, DDRB_target));
            Assert.AreEqual(DDRC_target, DDRC_result, String.Format("DDRC incorect {0} | {1}", DDRC_result, DDRC_target));
            Assert.AreEqual(DDRD_target, DDRD_result, String.Format("DDRD incorect {0} | {1}", DDRD_result, DDRD_target));
        }

        [TestMethod]
        public void CorePortOutTest()
        {
            int clocks = 250;
            byte PORTB_target = 0xBB;
            byte PORTC_target = 0x0C;
            byte PORTD_target = 0xDD;

            AVR8Core core = new AVR8Core();
            Assert.AreNotEqual(null, core);

            string hexPath = TestSettings.HexDirectory + TestSettings.PortOutHexName;
            Assert.IsTrue(File.Exists(hexPath), String.Format("Hex File not foud! [{0}]", hexPath));

            executeCore(hexPath, clocks, core);
            byte PORTB_result = (byte)(core.readPort(0xB) & 0xFF);
            byte PORTC_result = (byte)(core.readPort(0xC) & 0xFF);
            byte PORTD_result = (byte)(core.readPort(0xD) & 0xFF);

            Assert.AreEqual(PORTB_target, PORTB_result, String.Format("PORTB incorect {0} | {1}", PORTB_result, PORTB_target));
            Assert.AreEqual(PORTC_target, PORTC_result, String.Format("PORTC incorect {0} | {1}", PORTC_result, PORTC_target));
            Assert.AreEqual(PORTD_target, PORTD_result, String.Format("PORTD incorect {0} | {1}", PORTD_result, PORTD_target));

        }

        [TestMethod]
        public void PortInTest()
        {
            int clocks = 250;
            byte PORTB_target = 0x0B;
            byte PORTC_target = 0x0C;


            AVR8Core core = new AVR8Core();
            Assert.AreNotEqual(null, core);

            string hexPath = TestSettings.HexDirectory + TestSettings.PortInHexName;
            Assert.IsTrue(File.Exists(hexPath), String.Format("Hex File not foud! [{0}]", hexPath));

            executeCoreWithData(hexPath, clocks, core, portB: PORTB_target, portC: PORTC_target, clockInterval: 50);
            byte PORTB_result = (byte)core.readRegister(16);
            byte PORTC_result = (byte)core.readRegister(17);

            Assert.AreEqual(PORTB_target, PORTB_result, String.Format("PORTB incorect {0} | {1}", PORTB_result, PORTB_target));
            Assert.AreEqual(PORTC_target, PORTC_result, String.Format("PORTC incorect {0} | {1}", PORTC_result, PORTC_target));
        }

        [TestMethod]
        public void IrcExtTest()
        {
            int clocks = 500;
            byte EXT0_target = 0xAA;
            byte EXT1_target = 0xBB;
            byte PORTD_Ext0 = 0x04;
            byte PORTD_Ext1 = 0x08;
            byte extResult = 0;

            AVR8Core core = new AVR8Core();
            Assert.AreNotEqual(null, core);

            string hexPath = TestSettings.HexDirectory + TestSettings.IrcExtHexName;
            Assert.IsTrue(File.Exists(hexPath), String.Format("Hex File not foud! [{0}]", hexPath));

            
            executeCoreWithData(hexPath, clocks, core, portD: PORTD_Ext0, clockInterval: 150);
            extResult = (byte)core.readRegister(21);
            Assert.AreEqual(EXT0_target, extResult, String.Format("EXT0 incorect {0} | {1}", EXT0_target, extResult));

            core.resetCore();

            executeCoreWithData(hexPath, clocks, core, portD: PORTD_Ext1, clockInterval: 150);
            extResult = (byte)core.readRegister(22);
            Assert.AreEqual(EXT1_target, extResult, String.Format("EXT1 incorect {0} | {1}", EXT1_target, extResult));
            
        }
    
        [TestMethod]
        public void AdcTest()
        {
            int clocks = 800;

            AVR8Core core = new AVR8Core();
            Assert.AreNotEqual(null, core);

            string hexPath = TestSettings.HexDirectory + TestSettings.AdcHexName;
            Assert.IsTrue(File.Exists(hexPath), String.Format("Hex File not foud! [{0}]", hexPath));

            // Write only to 5-th chanel for test
            core.writeToADC(4.95, 5);
            executeCore(hexPath, clocks, core);

            byte ADC_result = (byte)(core.readRegister(20));
            Assert.AreNotEqual(0, ADC_result);
        }
    }
}
