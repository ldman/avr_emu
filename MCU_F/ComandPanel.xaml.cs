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
using System.Windows.Shapes;

namespace MCU_F
{

    struct MatrixKeyboardData
    {
        public int keyboardResult;
        public bool isActive;
    };

    public struct PortMaping
    {
        public uint portB;
        public uint portC;
        public uint portD;
    }

    /// <summary>
    /// Логика взаимодействия для ComandPanel.xaml
    /// </summary>
    public partial class ComandPanel : Window
    {

        public ICore avrCore;

        public TimeDomain timeDomain;

        private MatrixKeyboardData keyboardData;

        private bool mcuTimerNeedRestart;

        private bool int0;
        private bool int1;

        public PortMaping portMaping;

        private Hd44780 displayDriver;
        private string[] displayLines;

        int debugCount;
        
        double potValue;

        public ComandPanel()
        {
            InitializeComponent();

            keyboardData.keyboardResult = 0;
            keyboardData.isActive = false;

            portMaping.portB = 0x0;
            portMaping.portC = 0x0;
            portMaping.portD = 0x0;

            int0 = false;
            int1 = false;

            displayLines = new string[2];
            displayDriver = new Hd44780();
        }

        public void Init()
        {
            if (avrCore == null)
                return;

            updatePortMapping();
            debugCount = 0;

            (avrCore as IAdc).writeToADC(sPot.Value, 0);
            potValue = 0.0;
            displayLines = displayDriver.readLines();
        }

        #region Clk functions
        public void Clk()
        {
            // read signals from mcu
            //updatePortMapping();
        }

        public void UpdateUI()
        {
            Dispatcher.Invoke((Action)delegate()
            {
                CScreen.Text = string.Format("{0}\n{1}", displayLines[0], displayLines[1]);
                potValue = sPot.Value;
            });
        }

        public void McuTick()
        {
            updatePortMapping();

            // Display cmd assembling
            /*
             * E  PC5
             * RS PC4
             * D7 PC3
             * D6 PC2
             * D5 PC1
             * D4 PB5
             * D3 PB4
             * D2 PB3
             * D1 PB2
             * D0 PB1
             * 
             */

            uint cmdH = (portMaping.portC >> 1) & 0x1F;
            uint cmdL = (portMaping.portB >> 1) & 0x1F;
            uint cmdLine = (cmdH << 5) | cmdL;

            displayDriver.writePort(0x00, cmdLine);
            displayLines = displayDriver.readLines();

            // Matrix keyboard
            if (keyboardData.isActive)
            {
                // route signals for pressed button
                /*
                 * Port & pins for V+:
                 *  
                 *  PD5 -> 1 col
                 *  PD6 -> 2 col
                 *  PD7 -> 3 col
                 *  
                 * Port & pins for read:
                 * 
                 *  PB6 -> row 1 
                 *  PB7 -> row 2
                 *  PB0 -> row 3
                 * 
                 *  TODO: If it will unwanted 'memory' of old scan signals
                 *      add deleteSignal() for othre lines
                 * 
                 */
                switch (keyboardData.keyboardResult)
                {
                    case 1:
                        if ((portMaping.portD & (1 << 5)) == (1 << 5))
                            addSignal((1 << 6), 0xB);
                        break;
                    case 2:
                        if ((portMaping.portD & (1 << 6)) == (1 << 6))
                            addSignal((1 << 6), 0xB);
                        break;
                    case 3:
                        if ((portMaping.portD & (1 << 7)) == (1 << 7))
                            addSignal((1 << 6), 0xB);
                        break;
                    case 4:
                        if ((portMaping.portD & (1 << 5)) == (1 << 5))
                            addSignal((1 << 7), 0xB);
                        break;
                    case 5:
                        if ((portMaping.portD & (1 << 6)) == (1 << 6))
                            addSignal((1 << 7), 0xB);
                        break;
                    case 6:
                        if ((portMaping.portD & (1 << 7)) == (1 << 7))
                            addSignal((1 << 7), 0xB);
                        break;
                    case 7:
                        if ((portMaping.portD & (1 << 5)) == (1 << 5))
                            addSignal((1 << 0), 0xB);
                        break;
                    case 8:
                        if ((portMaping.portD & (1 << 6)) == (1 << 6))
                            addSignal((1 << 0), 0xB);
                        break;
                    case 9:
                        if ((portMaping.portD & (1 << 7)) == (1 << 7))
                            addSignal((1 << 0), 0xB);
                        break;
                }
            }
            else
            {
                // route for no button pressed: reset all scan signals

                deleteSignal((1 << 0), 0xB);
                deleteSignal((1 << 7), 0xB);
                deleteSignal((1 << 6), 0xB);
            }

            if(int0)
            {
                addSignal(0x04, 0xD);
            }
            else
            {
                deleteSignal(0x4, 0xD);
            }

            if(int1)
            {
                addSignal(0x8, 0xD);
            }
            else
            {
                deleteSignal(0x8, 0xD);
            }

            if (potValue > 3.3)
                addSignal(0x01, 0xC);
            else
                deleteSignal(0x01, 0xC);

            loadPortMappingToMCU();
        }
        #endregion

        #region Matrix keyboard
        private void Matrix_MouseDown (object sender, MouseButtonEventArgs e)
        {
            string yCoord = (sender as Button).Name.Split('_')[1];
            string xCoord =(sender as Button).Name.Split('_')[2];

            switch(yCoord)
            {
                case "1":
                    switch (xCoord)
                    {
                        case "1": keyboardData.keyboardResult = 1; break;
                        case "2": keyboardData.keyboardResult = 4; break;
                        case "3": keyboardData.keyboardResult = 7; break;
                    }
                    break;
                case "2":
                    switch (xCoord)
                    {
                        case "1": keyboardData.keyboardResult = 2; break;
                        case "2": keyboardData.keyboardResult = 5; break;
                        case "3": keyboardData.keyboardResult = 8; break;
                    }
                    break;
                case "3":
                    switch (xCoord)
                    {
                        case "1": keyboardData.keyboardResult = 3; break;
                        case "2": keyboardData.keyboardResult = 6; break;
                        case "3": keyboardData.keyboardResult = 9; break;
                    }
                    break;
            }

            keyboardData.isActive = true;
        }

        private void Matrix_MouseUp(object sender, MouseButtonEventArgs e)
        {
            keyboardData.isActive = false;
        }
        #endregion

        #region Upper Panel Buttons
        private void bInt0_MouseDown(object sender, MouseButtonEventArgs e)
        {
            debugCount++;
            int0 = true;
        }

        private void bInt0_MouseUp(object sender, MouseButtonEventArgs e)
        {
            int0 = false;
        }

        private void bInt1_MouseDown(object sender, MouseButtonEventArgs e)
        {
            int1 = true;
        }

        private void bInt1_MouseUp(object sender, MouseButtonEventArgs e)
        {
            int1 = false;
        }

        private void bReset_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (timeDomain.IsMCURunning)
            {
                mcuTimerNeedRestart = true;
                timeDomain.stopMCU();
            }
        }

        private void bReset_MouseUp(object sender, MouseButtonEventArgs e)
        {
            avrCore.softResetCore();

            if(mcuTimerNeedRestart)
                timeDomain.startMCU();
        }
        #endregion

        #region Logic Utils
        private void addSignal(uint pattern, int portId)
        {
            switch(portId)
            {
                case 0xB:
                    portMaping.portB |= pattern & 0xFF;
                    break;

                case 0xC:
                    portMaping.portC |= pattern & 0xFF;
                    break;

                case 0xD:
                    portMaping.portD |= pattern & 0xFF;
                    break;
            }
        }

        private void deleteSignal(uint pattern, int portId)
        {
            switch(portId)
            {
                case 0xB:
                    portMaping.portB &= ~((uint)pattern);
                    portMaping.portB &= 0xFF;
                    break;

                case 0xC:
                    portMaping.portC &= ~((uint)pattern);
                    portMaping.portC &= 0xFF;
                    break;

                case 0xD:
                    portMaping.portD &= ~((uint)pattern);
                    portMaping.portD &= 0xFF;
                    break;
            }
        }

        private void updatePortMapping()
        {
            portMaping.portB = (avrCore as IGeneralIO).readPort(0xB);
            portMaping.portC = (avrCore as IGeneralIO).readPort(0xC);
            portMaping.portD = (avrCore as IGeneralIO).readPort(0xD);
        }

        private void loadPortMappingToMCU()
        {
            (avrCore as IGeneralIO).writePort(0x0B, portMaping.portB);
            (avrCore as IGeneralIO).writePort(0x0C, portMaping.portC);
            (avrCore as IGeneralIO).writePort(0x0D, portMaping.portD);
        }
        #endregion

        private void sPot_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            (avrCore as IAdc).writeToADC(sPot.Value, 0);
        }

    }
}
