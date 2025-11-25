/*
 * Minimal, clean linker script for STM32F103C8 (64KB FLASH, 20KB RAM)
 * Compatible with libopencm3
 */

ENTRY(Reset_Handler)

/* Memory regions ---------------------------------------------------------- */
MEMORY
{
    rom (rx) : ORIGIN = 0x08000000, LENGTH = 64K
    ram (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

/* Symbols ----------------------------------------------------------------- */
_estack = ORIGIN(ram) + LENGTH(ram);

/* Sections ---------------------------------------------------------------- */
SECTIONS
{
    /* Vector table + program code */
    .text :
    {
        KEEP(*(.isr_vector))
        KEEP(*(.vectors))        /* used by libopencm3 */

        *(.text*)
        *(.rodata*)
        *(.glue_7*)
        *(.glue_7t*)
        *(.eh_frame*)

        . = ALIGN(4);
        _etext = .;
    } > rom

    /* Initialized data */
    .data : AT (_etext)
    {
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > ram

    /* Zero-init data */
    .bss (NOLOAD) :
    {
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > ram

    /* Heap/stack just occupy the remaining RAM */
    _end = .;
}
