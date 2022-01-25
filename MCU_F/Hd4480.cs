using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MCU_F
{
    class Hd44780 : IGeneralIO
    {
        private IOInfo ioInfo;
        private string[] displayLines;
        private string[] voidLines;
        private int cursor;

        private const string BLANK_LINE = "                ";
        private const int LINE_LEN = 16;

        uint last_cmd;

        /*
         * 
         * I/D - 0 = decrement cursor position, 1 = increment cursor position;
         * S - 0 = no display shift, 1 = display shift;
         * D - 0 = display off, 1 = display on;
         * C - 0 = cursor off, 1 = cursor on;
         * B - 0 = cursor blink off, 1 = cursor blink on ;
         * S/C - 0 = move cursor, 1 = shift display;
         * R/L - 0 = shift left, 1 = shift right;
         * DL - 0 = 4-bit interface, 1 = 8-bit interface;
         * N - 0 = 1/8 or 1/11 duty (1 line), 1 = 1/16 duty (2 lines);
         * F - 0 = 5×8 dots, 1 = 5×10 dots;
         * BF - 0 = can accept instruction, 1 = internal operation in progress.
         * 
         */

        private struct hd_state
        {
            public int D_0n_0ff;
            public int C_cursor;
            public int B_cursor_blink;
            public int ID_cursor_direction;
            public int DL_data_length;
            public int N_display_line_num;
        }

        hd_state state;

        public Hd44780()
        {
            // E RS D7 D6 D5 D4 D3 D2 D1 D0
            ioInfo.bitsPerPort = 10; ;
            ioInfo.portCount = 1;
            ioInfo.portId = new byte[1];
            ioInfo.portId[0] = 0x00;

            displayLines = new string[2];
            displayLines[0] = BLANK_LINE;
            displayLines[1] = BLANK_LINE;

            voidLines = new string[2];
            voidLines[0] = BLANK_LINE;
            voidLines[1] = BLANK_LINE;

            cursor = 0;

            state.D_0n_0ff = 0;
            state.C_cursor = 0;
            state.B_cursor_blink = 0;
            state.ID_cursor_direction = 1;
            state.DL_data_length = 8;
            state.N_display_line_num = 0;
        }

        public string[] readLines()
        {
            if(state.D_0n_0ff == 0)
            {
                return voidLines;
            }
            
            return displayLines;
        }

        public uint readPort(byte id)
        {
            return 0x00;
        }

        public void writePort(byte id, uint data)
        {
            if (id != 0x00)
                return;

            if (((data >> 9) & 0x01) == 1 && last_cmd == 0)
            {
                last_cmd = 1;
                uint cmd = (data >> 8) & 0x01;

                if (cmd == 0)
                {
                    uint dataCmd = data & 0xFF;
                    if (dataCmd == 0x01)
                    {
                        displayLines[0] = BLANK_LINE;
                        displayLines[1] = BLANK_LINE;
                    }
                    else if (((dataCmd >> 1) & 0xFF) == 0x01)
                    {
                        cursor = 0;
                    }
                    else if (((dataCmd >> 2) & 0xFF) == 0x01)
                    {
                        state.ID_cursor_direction = (int)(dataCmd >> 1) & 0x01;
                    }
                    else if (((dataCmd >> 3) & 0xFF) == 0x01)
                    {
                        state.D_0n_0ff = (int)(dataCmd >> 2) & 0x01;
                        state.C_cursor = (int)(dataCmd >> 1) & 0x01;
                        state.B_cursor_blink = (int)dataCmd & 0x01;
                    }
                    else if (((dataCmd >> 4) & 0xFF) == 0x01)
                    {
                        //not implemented
                    }
                    else if (((dataCmd >> 5) & 0xFF) == 0x01)
                    {
                        state.DL_data_length = (int)(dataCmd >> 4) & 0x01;
                        state.N_display_line_num = (int)(dataCmd >> 3) & 0x01;
                        // font not implemented
                    }
                }
                else
                {
                    char dataChar = (char)(data & 0xFF);
                    string currentLine = displayLines[state.N_display_line_num];

                    char[] repr = currentLine.ToCharArray();
                    repr[cursor] = dataChar;
                    displayLines[state.N_display_line_num] = new string(repr);
                    
                    if (state.ID_cursor_direction == 1)
                        cursor++;
                    else
                    {
                        cursor--;
                        if (cursor < 0)
                            cursor = 0;
                    }

                    cursor %= LINE_LEN;
                }
            }
            else
                last_cmd = ((data >> 9) & 0x01);
        }

        public IOInfo getGeneralIOInfo()
        {
            return ioInfo;
        }
    }
}
