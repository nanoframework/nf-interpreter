//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

namespace nanoFramework.Targets.ST_NUCLEO144_F439ZI
{
    /// <summary>
    /// Enumeration of ADC channels available on ST_NUCLEO144_F439ZI
    /// </summary>
    public static class AdcChannels
    {
        /// <summary>
        /// Channel 0, exposed as A0, connected to pin 1 on CN9 = PA3 (ADC1 - IN3)
        /// </summary>
        public const int Channel_0 = 0;

        /// <summary>
        /// Channel 1, exposed as A1, connected to pin 3 on CN9 = PC0 (ADC1 - IN10)
        /// </summary>
        public const int Channel_1 = 1;

        /// <summary>
        /// Channel 2, exposed as A2, connected to pin 5 on CN9 = PC3 (ADC1 - IN13)
        /// </summary>
        public const int Channel_2 = 2;

        /// <summary>
        /// Channel 3, exposed as A3, connected to pin 7 on CN9 = PF3 (ADC3 - IN9)
        /// </summary>
        public const int Channel_3 = 3;

        /// <summary>
        /// Channel 4, exposed as A4, connected to pin 9 on CN9 = PF5 (ADC3 - IN15)
        /// </summary>
        public const int Channel_4 = 4;

        /// <summary>
        /// Channel 5, exposed on A5, connected to pin 11 on CN9 = PF10 (ADC3 - IN8)
        /// </summary>
        public const int Channel_5 = 5;

        /// <summary>
        /// Channel 6, exposed on PB1, connected to CN10 Pin 7 (ADC2 - IN9)
        /// </summary>
        public const int Channel_6 = 6;

        /// <summary>
        /// Channel 7, exposed on PC2, connected to CN10 Pin 9 (ADC2 - IN12)
        /// </summary>
        public const int Channel_7 = 7;

        /// <summary>
        /// Channel 8, exposed on PF4, connected to CN10 Pin 11 (ADC3 - IN14)
        /// </summary>
        public const int Channel_8 = 8;

        /// <summary>
        /// Channel 9, internal temperature sensor, connected to ADC1
        /// </summary>
        public const int Channel_TemperatureSensor = 9;

        /// <summary>
        /// Channel 10, internal voltage reference, connected to ADC1
        /// </summary>
        public const int Channel_VrefIn = 10;

        /// <summary>
        /// Channel 11, connected to VBatt pin, ADC1
        /// </summary>
        public const int Channel_Vbatt = 11;
    }
}
