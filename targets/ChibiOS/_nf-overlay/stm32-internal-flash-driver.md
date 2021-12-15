# Driver for STM32 internal flash

This driver enables **nanoFramework** to access the internal flash for STM32 parts taking care of the low level details and implementation specifics.

In order to enable/use it the developer has to set HAL_NF_USE_STM32_FLASH to TRUE on the halconf_nf.h provided in the reference implementations.

The following versions of the driver are available:
- FLASHv1: suitable for F0 series
- FLASHv2: suitable for F4 and F7 series
- FLASHv3: suitable for L0 series
- FLASHv4: suitable for L4 series


The functions provided by driver are:
- ```void stm32FlashReadBytes(uint32_t startAddress, uint32_t length, uint8_t* buffer)```
  - startAddress: the flash start address from where to read data
  - length: how many bytes to be read from the flash
  - buffer: pointer to the buffer where to read the flash data

- ```int stm32FlashWrite(uint32_t startAddress, uint32_t length, const uint8_t* buffer)```
  - startAddress: the flash start address where to write data to
  - length: how many bytes to be written to the flash
  - buffer: pointer to the buffer whit the data to be written to the flash

- ```int stm32FlashIsErased(uint32_t startAddress, uint32_t length)```
  - startAddress: the flash start address to check if it's erased
  - length: length of the flash segment to check if it's erased

- ```int stm32FlashErase(uint32_t address)```
  - address: the flash address belonging to the segment to be erased (this can be a block or a page depending on the flash organization of the target series)

The driver performs basic and trivial checks on the arguments passed.
