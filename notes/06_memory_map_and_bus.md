# 6. Memory Map and Bus

## Memory Map

The **memory map** defines the organization and allocation of different peripheral registers and memory regions within the processor's addressable memory location range. This range is determined by the size of the address bus, and the mapping of various regions within this range constitutes the memory map. 

**Note**: The processor has a fixed default memory map that supports up to **4GB** of addressable memory.

### Code Region
The code region spans **512 MB**, from **0x00000000** to **0x1FFFFFFF**. This region is designated for connecting code memory, where MCU vendors typically interface different types of code storage such as embedded flash, ROM, OTP, and EEPROM. After a processor reset, the processor fetches vector table information from this region by default.

### SRAM Region
The SRAM region is located from **0x20000000** to **0x3FFFFFFF**, occupying the next **512 MB** of memory space following the code region. This area is primarily used for connecting SRAM, typically on-chip SRAM. Notably, the first **1 MB** of the SRAM region is bit-addressable, and program code can also be executed from this memory section.

### Peripherals Region
This region, also spanning **512 MB** from **0x40000000** to **0x5FFFFFFF**, is dedicated primarily to on-chip peripherals. If the optional bit-band feature is enabled, the first **1 MB** of the peripherals region is bit-addressable. It’s important to note that this is a **eXecute Never (XN)** region; any attempt to execute code from this region will result in a fault exception.

### External RAM Region
The external RAM region extends **1 GB** from **0x60000000** to **0x9FFFFFFF**. This region is intended for either on-chip or off-chip memory, allowing for the execution of code within this area, such as connecting external SDRAM.

### External Devices Region
Spanning **1 GB** from **0xA0000000** to **0xDFFFFFFF**, this region is designated for external devices and shared memory. Like the peripherals region, it is also an **eXecute Never (XN)** region.

### Private Peripheral Bus Region
This region covers **1 MB**, ranging from **0xE0000000** to **0xE00FFFFF**, and includes components such as the NVIC (Nested Vector Interrupt Controller), system timer, and system control block. It is likewise marked as an **eXecute Never (XN)** region.

### Vendor-Specific Memory
Finally, the vendor-specific memory region spans **511 MB** from **0xE0100000** to **0xFFFFFFFF**, offering space for additional custom implementations by specific vendors.

## Bus Protocol and Bus Interface