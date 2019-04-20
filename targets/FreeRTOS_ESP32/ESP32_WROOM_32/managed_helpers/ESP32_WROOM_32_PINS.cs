//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


using System;

namespace Pins
{
    public static class ESP32Device
    {
        public static class Gpio
        {
            public const int IO00 = 0;
            public const int IO01 = 1;
            public const int IO02 = 2;
            public const int IO03 = 3;
            public const int IO04 = 4;
            public const int IO05 = 5;
            public const int IO06 = 6;
            public const int IO07 = 7;
            public const int IO08 = 8;
            public const int IO09 = 9;
            public const int IO10 = 10;

            public const int IO11 = 11;
            public const int IO12 = 12;
            public const int IO13 = 13;
            public const int IO14 = 14;
            public const int IO15 = 15;
            public const int IO16 = 16;
            public const int IO17 = 17;
            public const int IO18 = 18;
            public const int IO19 = 19;
            public const int IO20 = 20;

            public const int IO21 = 21;
            public const int IO22 = 22;
            public const int IO23 = 23;
            public const int IO24 = 24;
            public const int IO25 = 25;
            public const int IO26 = 26;
            public const int IO27 = 27;
            public const int IO28 = 28;
            public const int IO29 = 29;

            public const int IO30 = 30;
            public const int IO31 = 31;
            public const int IO32 = 32;
            public const int IO33 = 33;
            public const int IO34 = 34;
            public const int IO35 = 35;
            public const int IO36 = 36;
            public const int IO37 = 37;
            public const int IO38 = 38;
            public const int IO39 = 39;

        }

        //addedd some helper keys ..
        //https://github.com/valoni/netmf-interpreter4x/blob/master/ESP32s_NodeMCU_v11/esp__nodemcu-32s_pinout.jpg
        public const int UserButton1 = Gpio.IO00;
        public const int Led1 = Gpio.IO02;

        public static class AdcChannels
        {
            //https://github.com/nanoframework/nf-interpreter/blob/develop/targets/FreeRTOS_ESP32/ESP32_WROOM_32/nanoCLR/Windows.Devices.Adc/win_dev_adc_native_Windows_Devices_Adc_AdcController.cpp
            // commented because for some pins have conflict with other  
            public static class ADC1
            {
        
           // public const int Channel_0 = Gpio.IO36;

            public const int Channel_1 = Gpio.IO37;

            public const int Channel_2 = Gpio.IO38;

          // public const int Channel_3 = Gpio.IO39;

            public const int Channel_4 = Gpio.IO32;

            public const int Channel_5 = Gpio.IO33;

            public const int Channel_6 = Gpio.IO34;

            public const int Channel_7 = Gpio.IO35;

            }

            //commented because some pins are used for wifi 
            //public static class ADC2
            //{
            //    public const int Channel_10 = Gpio.IO00;

            //    public const int Channel_11 = Gpio.IO02;

            //    public const int Channel_12 = Gpio.IO04;

            //    public const int Channel_13 = Gpio.IO12;

            //    public const int Channel_14 = Gpio.IO13;

            //    public const int Channel_15 = Gpio.IO14;

            //    public const int Channel_16 = Gpio.IO15;

            //    public const int Channel_17 = Gpio.IO25;

            //    public const int Channel_18 = Gpio.IO26;

            //    public const int Channel_19 = Gpio.IO27;

            //}

            public static class AdcChannelMode
            {
                public const int SingleEnded = 0;
                public const int Differential = 1;
            }
        }

        public static class SP1
        {
            public const string Name = "SPI1";
            public const int MOSI = Gpio.IO23;
            public const int MISO = Gpio.IO25;
            public const int SCK = Gpio.IO19;

        }

        public static class Serial
        {
            public static class COM1
            {
                public const string Name = "COM1";
                public const int TX = Gpio.IO01;
                public const int RX = Gpio.IO03;

            }

            public static class COM2
            {
                public const string Name = "COM2";
                public const int TX = Gpio.IO10;
                public const int RX = Gpio.IO09;

            }

            public static class COM3
            {
                public const string Name = "COM3";
                public const int TX = Gpio.IO17;
                public const int RX = Gpio.IO16;

            }

        }
        public static class I2C
        {
            public static class I2C1
            {
                public const string Name = "I2C1";
                public const int SDA = Gpio.IO18;
                public const int SCL = Gpio.IO19;

            }

            public static class I2C2
            {
                public const string Name = "I2C2";
                public const int SDA = Gpio.IO25;
                public const int SCL = Gpio.IO26;

            }
        }
    }

}
