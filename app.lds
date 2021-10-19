OUTPUT_ARCH( "riscv" )

SECTIONS 
{
  . = 0x00001000;
  .text : {
    *(.text._start)
    *(.text)
  }
  . = ALIGN(0x1000);
  .rodata : 
  { 
    *(.rdata)
    *(.rodata)
  }
  .data : { *(.data) }
  .bss : { *(.bss) }
  .debug : { *(.debug) }

  . = ALIGN(0x1000);
  .brk_area :
  {
    __brk_start0 = .;
    . = . + 4096 * 64;
    __brk_end = .;
  }

  . = ALIGN(0x1000);
  .mmap_area :
  {
    __mmap_start0 = .;
    . = . + 4096 * 64;
    __mmap_end = .;
  }
  _end = .;
}
