//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

namespace nanoFramework.Targets.STM32F769I_DISCOVERY
{
    /// <summary>
    /// Enumeration of ADC channels available in STM32F769I_DISCOVERY
    /// </summary>
    public static class AdcChannels
    {
        /// <summary>
        /// Channel 0, exposed on A0, connected to PA6 (ADC1 - IN6)
        /// </summary>
        public const int Channel_0 = 0;

        /// <summary>
        /// Channel 1, exposed on A1, connected to PA4 (ADC1 - IN4)
        /// </summary>
        public const int Channel_1 = 1;

        /// <summary>
        /// Channel 2, exposed on A2, connected to PC2 (ADC1 - IN12)
        /// </summary>
        public const int Channel_2 = 2;

        /// <summary>
        /// Channel 3, exposed on A3, connected to PF10 (ADC1 - IN8)
        /// </summary>
        public const int Channel_3 = 3;

        /// <summary>
        /// Channel 4, exposed on A4, connected to PF8 (ADC3 - IN6)
        /// </summary>
        public const int Channel_4 = 4;

        /// <summary>
        /// Channel 5, internal temperature sensor, connected to ADC1
        /// </summary>
        public const int Channel_TemperatureSensor = 5;

        /// <summary>
        /// Channel 6, internal voltage reference, connected to ADC1
        /// </summary>
        public const int Channel_VrefIn = 6;

        /// <summary>
        /// Channel 7, connected to VBatt pin, ADC1
        /// </summary>
        public const int Channel_Vbatt = 7;
    }
}
