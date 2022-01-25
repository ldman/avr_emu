using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;



namespace MCU_F
{
    /// <summary>
    /// Логика взаимодействия для MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window, IDisposable
    {

        public ICore avrCore;

        private UInt32 PC_position;

        private List<CoreMemoryInfo> memory;
        //private List<MemoryString> programMemory;

        private UInt32 totalProgramMemoryUsed;

        public TimeDomain timeDomain;

        protected ComandPanel cPanel;

        public MainWindow()
        {
            InitializeComponent();

            /* Create core and init views using it params */
            avrCore = new AVR8Core();           
            loadCoreInfo();
            initFileRegisterView();
            initGeneralRegisterView();
            updateGeneralRegisterView();

            PC_position = 0;
            mcuMHz.Text = "100"; // Set default MHz to 8

            //programMemory = new List<MemoryString>();
            totalProgramMemoryUsed = 0;

            /* System timers configurartion */
            timeDomain = new TimeDomain();                        
            setUpMCUTimer();

            timeDomain.MCUTick += avrCore.coreCLK;

            /* UI creation */
            cPanel = new ComandPanel();
            cPanel.avrCore = avrCore as ICore;
            cPanel.timeDomain = timeDomain;
            timeDomain.Hdw1Tick += cPanel.Clk;
            timeDomain.MCUTick += cPanel.McuTick;
            cPanel.Init();
            cPanel.Show();

            int hdw250KHz = 250 * 1000;
            timeDomain.setHdw1Time(hdw250KHz);

            timeDomain.setHdw2Time(10);
            timeDomain.Hdw2Tick += UpdateUI;
            timeDomain.Hdw2Tick += cPanel.UpdateUI;

            timeDomain.startHDW1();
            timeDomain.startHDW2();
        }


        #region coreInitialization

        private void loadCoreInfo()
        {
            if(avrCore == null)
            {
                coreInfoLabel.Content = SystemMessages.ErrorLoadingCore;
                return;
            }

            string coreData = string.Format("Ядро: {0}. Ver: {1}.\n", avrCore.getCoreName(), avrCore.getCoreVersion());
            string memoryData = "";

            memory = avrCore.getCoreMemoryInfo();
            foreach(CoreMemoryInfo memInfo in memory)
            {
                string tempStr = "";

                switch(memInfo.memoryType)
                {
                    case CoreMemoryTypes.PROG_MEM:
                        tempStr += string.Format("Програмная память {0}", memInfo.memorySize.ToString());
                        break;
                    case CoreMemoryTypes.RAM_MEM:
                        tempStr += string.Format("Оперативная память {0}", memInfo.memorySize.ToString());
                        break;
                    case CoreMemoryTypes.EEPROM_MEM:
                        tempStr += string.Format("EEPROM память {0}", memInfo.memorySize.ToString());
                        break;
                }

                switch(memInfo.memoryArchitecture)
                {
                    case CoreMemoryArch.WORD_MEMORY:
                        tempStr += " слов\n";
                        break;
                    case CoreMemoryArch.BYTE_MEMORY:
                        tempStr += " байт\n";
                        break;
                }

                memoryData += tempStr;                
            }

            coreInfoLabel.Content = coreData + memoryData;
            updateTotalLoadedProgMemory();
        }

        private void initFileRegisterView()
        {
            foreach (RegisterData regDat in avrCore.getRegisters())
                fileRegistersView.Items.Add(regDat.ToString());

            return;
        }

        private void initGeneralRegisterView()
        {
            generalRegisters.Text = "";
        }

        #endregion


        #region uiHandlers
        
        /* Load program to MCU */
        private void programHexBtn_Click(object sender, RoutedEventArgs e)
        {
            if (avrCore == null)
                return;

            avrCore.loadProgram();
            
            updateTotalLoadedProgMemory();
            //updateProgMemoryImage();
            //updateMemoryWindow();
            updateMemoryWindowDisassembly();
        }

        /* MCU timer setup */
        private void setMCUButton_Click(object sender, RoutedEventArgs e)
        {
            setUpMCUTimer();
        }

        /* Run MCU by timer event */
        private void mcuRunBtn_Click(object sender, RoutedEventArgs e)
        {
            timeDomain.startMCU();
            timeDomain.startHDW1();
            mcuRunBtn.IsEnabled = false;
            mcuStepBtn.IsEnabled = false;
        }

        private void mcuStepBtn_Click(object sender, RoutedEventArgs e)
        {            
            timeDomain.manualStepForward();
        }

        private void mcuStopBtn_Click(object sender, RoutedEventArgs e)
        {
            mcuRunBtn.IsEnabled = true;
            mcuStepBtn.IsEnabled = true;

            timeDomain.stopMCU();
            timeDomain.stopHDW1();
            //updateCurrentCMDHighlight();
            //avrCore.updateRegisters();
            //updateRegisterView();
        }

        /* Update register view*/
        private void updateRegisterView()
        {
            fileRegistersView.Items.Clear();            
            foreach(RegisterData regDat in avrCore.getRegisters())
                fileRegistersView.Items.Add(regDat.ToString());
        }
        
        /* Update general registers view*/
        private void updateGeneralRegisterView()
        {
            string registers = "";
            string ports = "";

            int regAddr = 0;
            uint regVal;

            for(int i = 1; i <= 8; i++)
            {
                for (int j = 1; j <= 4; j++)
                {                    
                    regVal = avrCore.readRegister(regAddr);
                    registers += string.Format("R{0,2}=0x{1}  ", regAddr, regVal);
                    regAddr++;
                }
                registers += " \n\n";
            }

            ports += string.Format("PORTB {0}\n", avrCore.readRegister(avrCore.getRegAddrByName("PORTB")));
            ports += string.Format("PORTC {0}\n", avrCore.readRegister(avrCore.getRegAddrByName("PORTC")));
            ports += string.Format("PORTD {0}\n\n", avrCore.readRegister(avrCore.getRegAddrByName("PORTD")));

            ports += string.Format("PINB {0}\n", avrCore.readRegister(avrCore.getRegAddrByName("PINB")));
            ports += string.Format("PINC {0}\n", avrCore.readRegister(avrCore.getRegAddrByName("PINC")));
            ports += string.Format("PIND {0}\n\n", avrCore.readRegister(avrCore.getRegAddrByName("PIND")));

            ports += string.Format("DDRB {0}\n", avrCore.readRegister(avrCore.getRegAddrByName("DDRB")));
            ports += string.Format("DDRC {0}\n", avrCore.readRegister(avrCore.getRegAddrByName("DDRC")));
            ports += string.Format("DDRD {0}\n\n", avrCore.readRegister(avrCore.getRegAddrByName("DDRD")));

            if (cPanel != null)
            {
                ports += string.Format("Signal B {0}\n", cPanel.portMaping.portB);
                ports += string.Format("Signal C {0}\n", cPanel.portMaping.portC);
                ports += string.Format("Signal D {0}\n", cPanel.portMaping.portD);
            }

            
            generalRegisters.Text = registers;
            portStatus.Text = ports;
        }

        private void UpdateUI()
        {
            Dispatcher.Invoke((Action)delegate() {
                avrCore.updateRegisters();
                updateRegisterView();
                updateGeneralRegisterView();
                updateCurrentCMDHighlight();
            });

        }

        #endregion


        #region uiFunctions
       
        /* Print total used program memory */
        private void updateTotalLoadedProgMemory()
        {
            string memInfo = "-/-";

            foreach(CoreMemoryInfo _mem in memory)
            {
                if (_mem.memoryType == CoreMemoryTypes.PROG_MEM)
                {
                    string usedSize = "";

                    switch (_mem.memoryArchitecture)
                    {
                    case CoreMemoryArch.WORD_MEMORY:
                        usedSize = string.Format("{0} / {1} слов", _mem.memoryUsed, _mem.memorySize);
                        break;
                    case CoreMemoryArch.BYTE_MEMORY:
                        usedSize = string.Format("{0} / {1} байт", _mem.memoryUsed, _mem.memorySize);
                        break;
                    }

                    totalProgramMemoryUsed = _mem.memoryUsed;

                    memInfo = string.Format("Загружено в память:\n " + usedSize);
                }
            }

            totalProgLoaded.Content = memInfo;
        }

        /* Print memory image on screen */
        //private void updateMemoryWindow()
        //{            
        //    programMemoryView.Items.Clear();

        //    int curItem = 0;
        //    foreach(MemoryString memStr in programMemory)
        //    {
        //        if (curItem++ < totalProgramMemoryUsed)
        //        {
        //            string cmdRepr = string.Format("{0}  {1}    {2}", memStr.cmdAddr, memStr.hexView, memStr.cmdView);
        //            programMemoryView.Items.Add(cmdRepr);
        //        }
        //        else
        //        {
        //            string cmdRepr = string.Format("{0}  {1}", memStr.cmdAddr, memStr.hexView);
        //            programMemoryView.Items.Add(cmdRepr);
        //        }                               
        //    }

        //    updateCurrentCMDHighlight();
        //}        

        private void updateMemoryWindowDisassembly()
        {
            programMemoryView.Items.Clear();
            List<String> lising = (avrCore as IDisassembler).GetDisassemblyListing();

            foreach (String cmd in lising)
                programMemoryView.Items.Add(cmd);
        }

        /* set prog memory highlight */
        private void updateCurrentCMDHighlight()
        {

            if (programMemoryView.Items.Count < 1)
                return;

            PC_position = avrCore.readRegister(avrCore.getRegAddrByName("REG_PC"));

            //programMemoryView.Focus();
            //programMemoryView.ScrollIntoView(programMemoryView.Items[(int)PC_position]);
            programMemoryView.SelectedIndex = (int)PC_position;

            updateGeneralRegisterView();
        }

        private void setUpMCUTimer()
        {
            try
            {
                UInt64 MHz = Convert.ToUInt64(mcuMHz.Text) * 10; //1000000000;
                timeDomain.setMCUTime(MHz);
            }
            catch (System.FormatException) // On error set default parameter
            {
                mcuMHz.Text = "100";
                UInt64 MHz = Convert.ToUInt64(mcuMHz.Text)  *10; //1000000000;
                timeDomain.setMCUTime(MHz);
            }
        }
        
        #endregion


        #region memoryFunctions
        
        //void updateProgMemoryImage()
        //{
        //    var prgmMemory = avrCore.readWordMemory(CoreMemoryTypes.PROG_MEM);

        //    programMemory.Clear();
            
        //    int addr = 0;

        //    foreach (var progCmd in prgmMemory)
        //    {
        //        MemoryString cmd;

        //        cmd.hexView = string.Format("0x{0:X4}", progCmd);
        //        cmd.cmdView = "-/-";
        //        cmd.cmdAddr = string.Format("{0:0000}d | 0x{1:X4}", addr, addr);
        //        cmd.hexCmd = progCmd;

        //        programMemory.Add(cmd);
        //        addr++;
        //    }
        //}

        #endregion


        #region IDispose
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool clearNonMachine)
        {
            if(clearNonMachine)
            {
                timeDomain.Dispose();
                (avrCore as IDisposable).Dispose();
            }

            GC.Collect();
        }

        #endregion
    }
}
