using System;
using System.Threading;
using Windows.Devices.Gpio;
using Windows.Devices.Spi;

namespace NF.TestApplication_NEW
{
    public class Program
    {
        // For F4-Discovery -> Off board LED is on PB7 = 1 * 16 + 7 = 23 
        // For F429I-Discovery -> Off board LED is on PG13 = 6 * 16 + 13 = 109 
        // For F769I-DISCO -> LED2_GREEN is on PJ5 = 9 * 16 + 5 = 149
        // For F769I-DISCO -> LED2_RED is on PJ13 = 9 * 16 + 13 = 157
        // For F769I-DISCO -> USER_BUTTON is on PA0 = 0 * 16 + 0 = 0
        // For F746ZG-NUCLEO -> Off board LED is on PC10 = 2 * 16 + 10 = 42 
        private const int LED_PIN = 149;
        private const int LED_RED_PIN = 157;
        private const int USER_BUTTON_PIN = 0;

        // Set up the LED
        private static GpioPin LED;
        private static GpioPin LED1;
        private static GpioPin UserButton;

        private static SpiDevice MemsSensor;

        private static int dummyIntForTimer = 0;
        private static int dummyIntForGpio = 0;

        public static void Main()
        {
            ////////////////////////////////////////////////////////////////////////////////////
            // ToString() test
            int thisIsAnInteger = 64;
            var testString = thisIsAnInteger.ToString();

            
            ////////////////////////////////////////////////////////////////////////////////////
            // timer test
            Timer t = new Timer(TimerCb, null, 5000, 5000);


            ////////////////////////////////////////////////////////////////////////////////////
            // Gpio test

            var gpio = GpioController.GetDefault();

            // red LED
            LED1 = gpio.OpenPin(109);
            LED1.SetDriveMode(GpioPinDriveMode.Output);

            // user button
            UserButton = gpio.OpenPin(USER_BUTTON_PIN);
            UserButton.SetDriveMode(GpioPinDriveMode.Input);
            UserButton.ValueChanged += UserButton_ValueChanged;


            ////////////////////////////////////////////////////////////////////////////////////
            // Spi test

            var settings = new SpiConnectionSettings(3)        // PA3:CS1
            {
                Mode = SpiMode.Mode1,
                ClockFrequency = 12 * 1000 * 1000,  // 12Mhz
                DataBitLength = 8
            };

            MemsSensor = SpiDevice.FromId("SPI5", settings);

            //ushort[] redBuffer = new ushort[4];
            //MemsSensor.TransferFullDuplex(new ushort[] { 0x100, 0x200, 0x300, 0x400 }, redBuffer);

            MemsSensor.Write(new ushort[] { 0x100, 0x200, 0x300, 0x400 });

            ////////////////////////////////////////////////////////////////////////////////////
            while (true)
            {
                Thread.Sleep(1000);
            }
        }

        private static void TimerCb(object state)
        {
            // doing stuff
            dummyIntForTimer++;

            Console.WriteLine(dummyIntForTimer.ToString());
        }

        static void UserButton_ValueChanged(object sender, GpioPinValueChangedEventArgs e)
        {
            dummyIntForGpio++;

            if (e.Edge == GpioPinEdge.RisingEdge)
            {
                LED1.Write(GpioPinValue.High);
            }
            else
            {
                LED1.Write(GpioPinValue.Low);
            }
        }
    }
}
