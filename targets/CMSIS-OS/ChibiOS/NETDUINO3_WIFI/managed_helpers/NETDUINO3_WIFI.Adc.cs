//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

namespace nanoFramework.Targets.NETDUINO3_WIFI
{
    /// <summary>
    /// Enumeration of ADC channels available in NETDUINO3_WIFI
    /// </summary>
    public static class AdcChannels
    {
        /// <summary>
        /// Channel 0, connected to PC0 (ADC1 - IN10)
        /// </summary>
        public const int Channel_0 = 0;

        /// <summary>
        /// Channel 1, connected to PC1 (ADC1 - IN11)
        /// </summary>
        public const int Channel_1 = 1;

        /// <summary>
        /// Channel 2, connected to PC2 (ADC2 - IN14)
        /// </summary>
        public const int Channel_2 = 2;

        /// <summary>
        /// Channel 3, connected to PC3 (ADC2 - IN15)
        /// </summary>
        public const int Channel_3 = 3;

        /// <summary>
        /// Channel 4, connected to PC4 (ADC3 - IN12)
        /// </summary>
        public const int Channel_4 = 4;

        /// <summary>
        /// Channel 5, connected to PC5 (ADC3 - IN13)
        /// </summary>
        public const int Channel_5 = 5;

        /// <summary>
        /// Channel 6, internal temperature sensor, connected to ADC1
        /// </summary>
        public const int Channel_TemperatureSensor = 6;

        /// <summary>
        /// Channel 7, internal voltage reference, connected to ADC1
        /// </summary>
        public const int Channel_VrefIn = 7;

        /// <summary>
        /// Channel 8, connected to VBatt pin, ADC1
        /// </summary>
        public const int Channel_Vbatt = 8;
    }
}
