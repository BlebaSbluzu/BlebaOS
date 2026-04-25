ENTRY(start)
OUTPUT_FORMAT(binary)

SECTIONS
{
    . = 0x10000;          /* The address your bootloader jumps to */
    .text : { *(.text) }  /* Code section */
    .data : { *(.data) }  /* Data section */
    .bss  : { *(.bss)  }  /* Uninitialized data */
}