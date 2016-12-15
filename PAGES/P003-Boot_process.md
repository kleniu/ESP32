# BOOT process 
source: https://github.com/espressif/esp-idf/blob/master/docs/general-notes.rst
## 0) High level view of boot process

* First-stage bootloader in ROM loads second-stage bootloader image to RAM (IRAM & DRAM) 
from flash offset 0x1000.
* Second-stage bootloader loads partition table and main app image from flash. Main app 
incorporates both RAM segments and read-only segments mapped via flash cache.
* Main app image executes. At this point the second CPU and RTOS scheduler can be started.

## 1) First stage bootloader

## 2) Second stage bootloader
The binary image which resides at offset 0x1000 in flash is called. 

It is possible to write a fully featured application which would work when flashed to offset 0x1000.
Normaly at offset 0x1000 ESP-IDF second stage boot loader is recorded (source code: esp-idf/components/bootloader) 
to allow:
- use of partition tables
- flash encription
- secure boot
- over-the-air (OTA) updates

When the first stage bootloader is finished checking and loading the second stage bootloader, it 
jumps to the second stage bootloader entry point found in the binary image header.

Second stage bootloader reads the partition table found at offset 0x8000. The bootloader finds factory 
and OTA partitions, and decides which one to boot based on data found in OTA info partition.

For the selected partition, second stage bootloader copies data and code sections which are mapped 
into IRAM and DRAM to their load addresses. 