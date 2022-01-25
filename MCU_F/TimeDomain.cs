using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Timers;

namespace MCU_F
{
    public delegate void OnMCUTick();
    public delegate void OnHdw1Tick();
    public delegate void OnHdw2Tick();

    public class TimeDomain : IDisposable
    {
        private Timer MCUTimer;
        private Timer HardwareTimer_1;
        private Timer HardwareTimer_2;

        private bool _mcuRunning;
        private bool _hdw1Running;
        private bool _hdw2Running;

        public bool IsMCURunning { get { return _mcuRunning; } }
        public bool IsHDW1Running { get { return _hdw1Running; } }
        public bool IsHDW2Running { get { return _hdw2Running; } }

        public TimeDomain()
        {
            /**
             *  MCU timer -> 8MHz default
             *  Hd 1 / 2 -> 1Hz default
             *  1Hz = 1000 ms
             */            

            MCUTimer = new Timer(0.000125); 
            HardwareTimer_1 = new Timer(1000);
            HardwareTimer_2 = new Timer(1000);

            MCUTimer.Elapsed += MCUTimer_Elapsed;
            HardwareTimer_1.Elapsed += HardwareTimer_1_Elapsed;
            HardwareTimer_2.Elapsed += HardwareTimer_2_Elapsed;

            _mcuRunning = false;
            _hdw1Running = false;
            _hdw2Running = false;

        }         

        public OnMCUTick  MCUTick;
        public OnHdw1Tick Hdw1Tick;
        public OnHdw2Tick Hdw2Tick;

        void MCUTimer_Elapsed(object sender, ElapsedEventArgs e) { if (MCUTick != null) MCUTick(); }
        void HardwareTimer_1_Elapsed(object sender, ElapsedEventArgs e) { if (Hdw1Tick != null) Hdw1Tick(); }
        void HardwareTimer_2_Elapsed(object sender, ElapsedEventArgs e) { if (Hdw2Tick != null) Hdw2Tick(); }

        public void manualStepForward()
        {
            if (IsMCURunning)
                stopMCU();
            
            if (IsHDW1Running)
                stopHDW1();

            //if (IsHDW2Running)
            //    stopHDW2();

            if (MCUTick != null)
                MCUTick();

            if (Hdw1Tick != null)
                Hdw1Tick();

            //if (Hdw2Tick != null)
            //    Hdw2Tick();
        }

        public void autoStepForward()
        {
            startMCU();
            startHDW1();
            //startHDW2();
        }

        private bool setTimerTime(double timeHz, Timer timer)
        {            
            timer.Stop();

            if (timeHz < 0)
                return false;

            double interval = (1000 / timeHz);
            timer.Interval = interval;            
            return true;
        }

        public bool setMCUTime(double time)
        {            
            _mcuRunning = false;
            return setTimerTime(time, MCUTimer);
        }

        public bool setHdw1Time(double time)
        {            
            _hdw1Running = false;
            return setTimerTime(time, HardwareTimer_1);
        }

        public bool setHdw2Time(double time)
        {
            _hdw2Running = false;
            return setTimerTime(time, HardwareTimer_2);
        }

        public void startMCU() { MCUTimer.Start(); _mcuRunning = true; }
        public void startHDW1() { HardwareTimer_1.Start(); _hdw1Running = true; }
        public void startHDW2() { HardwareTimer_2.Start(); _hdw2Running = true; }
        
        public void stopMCU() { MCUTimer.Stop(); _mcuRunning = false; }
        public void stopHDW1() { HardwareTimer_1.Stop(); _hdw1Running = false; }
        public void stopHDW2() { HardwareTimer_2.Stop(); _hdw2Running = false; }

        public void Dispose()
        {            
            MCUTimer.Dispose();
            HardwareTimer_1.Dispose();
            HardwareTimer_2.Dispose();
        }
    }
}
