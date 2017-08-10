using System.Threading;
using Windows.Devices.Gpio;

namespace NF.TestApplication_NEW
{
    public class Program
    {
        private const int LED_PIN = 149;
        private const string NAME = "Chewbacca";

        // Set up the LED
        private static GpioPin LED;

        public static void Main()
        {
            var gpio = GpioController.GetDefault();
            LED = gpio.OpenPin(LED_PIN);
            LED.SetDriveMode(GpioPinDriveMode.Output);

            for (;;)
            {
                LED.Write(GpioPinValue.High);
                Thread.Sleep(100);
                LED.Write(GpioPinValue.Low);
                Thread.Sleep(400);
            }
        }
    }
}
