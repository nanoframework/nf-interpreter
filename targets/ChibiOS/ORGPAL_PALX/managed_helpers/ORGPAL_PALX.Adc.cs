//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

namespace nanoFramework.Targets.ORGPAL_PALX
{
    /// <summary>
    /// Enumeration of ADC channels available for the ORGPAL_PALX.
    /// </summary>
    /// <remarks>
    /// Aligned to `target_system_device_adc_config.cpp` and must be kept inline.
    /// </remarks>
    public static class AdcChannels
    {

        /// <summary>
        /// Channel 0, Thermistor port input.
        /// </summary>
        /// <remarks>
        /// Connected to PA2 (ADC1 - IN2).
        /// </remarks>
        public const int Channel0_ThermistorInput = 0;

        /// <summary>
        /// Channel 1, Battery port input.
        /// </summary>
        /// <remarks>
        /// Connected to PB0 (ADC1 - IN8).
        /// </remarks>
        public const int Channel1_BatteryInput = 1;

        /// <summary>
        /// Channel 2, 420mA Current sensor port input.
        /// </summary>
        /// <remarks>
        /// Connected to PC2 (ADC1 - IN12).
        /// </remarks>
        public const int Channel2_CurrentSensorInput = 2;

        /// <summary>
        /// Channel 3, Onboard PCB temperature.
        /// </summary>
        /// <remarks>
        /// Connected to PC3 (ADC1 - IN13).
        /// </remarks>
        public const int Channel3_PcbTemperatureSensor = 3;


        /// <summary>
        /// Channel 4.
        /// </summary>
        /// <remarks>
        /// Internal MCU temperature sensor, connected to ADC1.
        /// </remarks>
        public const int Channel4_McuTemperatureSensor = 4;

        /// <summary>
        /// Channel 5.
        /// </summary>
        /// <remarks>
        /// Internal voltage reference, connected to ADC1.
        /// </remarks>
        public const int Channel5_InternalVoltageReference = 5;

        /// <summary>
        /// Channel 6.
        /// </summary>
        /// <remarks>
        /// Internal VBATT pin, connected to ADC1.
        /// </remarks>
        public const int Channel6_BatteryVoltage = 6;

    }
}
