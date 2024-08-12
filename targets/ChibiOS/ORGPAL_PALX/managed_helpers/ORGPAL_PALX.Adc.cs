//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

namespace nanoFramework.Targets.ORGPAL_PALX
{
    /// <summary>
    /// Enumeration of ADC channels available in ORGPAL_PALX
    /// </summary>
    public static class AdcChannels
    {
        /// <summary>
        /// Channel 0, exposed on A0, connected to PA2 (ADC1 - IN1)
        /// </summary>
        public const int Channel_0 = 0;

        /// <summary>
        /// Channel 1, exposed on A1, connected to PB0 (ADC1 - IN8)
        /// </summary>
        public const int Channel_1 = 1;

        /// <summary>
        /// Channel 2, exposed on A2, connected to PC2 (ADC1 - IN12)
        /// </summary>
        public const int Channel_2 = 2;

        /// <summary>
        /// Channel 3, exposed on A3, connected to PC3 (ADC1 - IN13)
        /// </summary>
        public const int Channel_3 = 3;

        /// <summary>
        /// Channel 4, internal temperature sensor, connected to ADC1
        /// </summary>
        public const int Channel_TemperatureSensor = 6;

        /// <summary>
        /// Channel 5, internal voltage reference, connected to ADC1
        /// </summary>
        public const int Channel_VrefIn = 7;

        /// <summary>
        /// Channel 6, connected to VBatt pin, ADC1
        /// </summary>
        public const int Channel_Vbatt = 8;
    }
}
