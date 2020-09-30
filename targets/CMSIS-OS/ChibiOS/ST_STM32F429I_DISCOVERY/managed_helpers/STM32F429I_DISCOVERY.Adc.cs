//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

namespace nanoFramework.Targets.STM32F429I_DISCOVERY
{
    /// <summary>
    /// Enumeration of ADC channels available in STM32F429I_DISCOVERY
    /// </summary>
    public static class AdcChannels
    {
        /// <summary>
        /// Channel 0, exposed on PA5, connected to PA5 (ADC1 - IN5)
        /// </summary>
        public const int Channel_0 = 0;

        /// <summary>
        /// Channel 1, exposed on PC3, connected to PC3 (ADC2 - IN13)
        /// </summary>
        public const int Channel_1 = 1;

        /// <summary>
        /// Channel 2, exposed on PF6, connected to PF6 (ADC3 - IN4)
        /// </summary>
        public const int Channel_2 = 2;

        /// <summary>
        /// Channel 3, internal temperature sensor, connected to ADC1
        /// </summary>
        public const int Channel_TemperatureSensor = 3;

        /// <summary>
        /// Channel 4, internal voltage reference, connected to ADC1
        /// </summary>
        public const int Channel_VrefIn = 4;

        /// <summary>
        /// Channel 5, connected to VBatt pin, ADC1
        /// </summary>
        public const int Channel_Vbatt = 5;
    }
}
