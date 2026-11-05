//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

namespace nanoFramework.Targets.STM32H735_DISCOVERY
{
    /// <summary>
    /// Enumeration of ADC channels available in STM32H735_DISCOVERY
    /// </summary>
    public static class AdcChannels
    {
        /// <summary>
        /// Channel 0, exposed on Arduino A0, connected to PC0 (ADC1 - INP10)
        /// </summary>
        public const int Channel_0 = 0;

        /// <summary>
        /// Channel 1, exposed on Arduino D3, connected to PA0 (ADC1 - INP16)
        /// </summary>
        public const int Channel_1 = 1;

        /// <summary>
        /// Channel 2, exposed on STMOD+ connector, connected to PF11 (ADC1 - INP2)
        /// </summary>
        public const int Channel_2 = 2;
    }
}
