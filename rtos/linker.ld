/* First thing to execute */
ENTRY(Reset_Handler)

/* Define memory */
MEMORY
{
    FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 256K
    RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
}

/* Define _estack based on RAM */
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* Define sections of MEMORY */
SECTIONS
{
    .isr_vector :
    {
        KEEP(*(.isr_vector))
    } > FLASH

    .text :
    {
        *(.text*)
        . = ALIGN(4);
    } > FLASH

    .rodata :
    {
        *(.rodata*)
        . = ALIGN(4);
    } > FLASH

    _sidata = LOADADDR(.data);

    .data :
    {
        . = ALIGN(4);
        _sdata = .;           /* start of .data in RAM */
        *(.data*)
        . = ALIGN(4);
        _edata = .;           /* end of .data in RAM */
    } > RAM AT> FLASH

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > RAM
}
