/*
 * Memory Spaces Definitions.
 *
 * Need modifying for a specific board. 
 *   FLASH.ORIGIN: starting address of flash
 *   FLASH.LENGTH: length of flash
 *   RAM.ORIGIN: starting address of RAM bank 0
 *   RAM.LENGTH: length of RAM bank 0
 *
 * The values below can be addressed in further linker scripts
 * using functions like 'ORIGIN(RAM)' or 'LENGTH(RAM)'.
 */

MEMORY
{
  FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 384K 
  RAM (xrw) : ORIGIN = 0x20000000, LENGTH = 16K
  RAM2 (xrw) : ORIGIN = 0x20040000, LENGTH = 16K

  /*
   * Optional sections; define the origin and length to match
   * the the specific requirements of your hardware. The zero
   * length prevents inadvertent allocation.
   */
  CCMRAM (xrw) : ORIGIN = 0x10000000, LENGTH = 0
  FLASHB1 (rx) : ORIGIN = 0x00000000, LENGTH = 0
  EXTMEMB0 (rx) : ORIGIN = 0x00000000, LENGTH = 0
  EXTMEMB1 (rx) : ORIGIN = 0x00000000, LENGTH = 0
  EXTMEMB2 (rx) : ORIGIN = 0x00000000, LENGTH = 0
  EXTMEMB3 (rx) : ORIGIN = 0x00000000, LENGTH = 0
  /* constants for integrity checks ADuCM350 (e.g. parity, CRC,..) */
  FLASHC (r) : ORIGIN = 0x000007fc, LENGTH = 4
  FLASHCODE (rx): ORIGIN = 0x00000800, LENGTH = 200k
}

/*
 * For external ram use something like:
 *  RAM (xrw) : ORIGIN = 0x64000000, LENGTH = 2048K
 *
 * For special RAM areas use something like:
 *  MEMORY_ARRAY (xrw)  : ORIGIN = 0x20002000, LENGTH = 32
 */