/* Script for -z combreloc */
/* Copyright (C) 2014-2023 Free Software Foundation, Inc.
   Copying and distribution of this script, with or without modification,
   are permitted in any medium without royalty provided the copyright
   notice and this notice are preserved.  */
OUTPUT_FORMAT("elf64-littleriscv")
OUTPUT_ARCH(riscv)

/* need to control RAM configuration */
MEMORY
{
  /* RAM reserved for memory test */
  ram_to_use(rwx)  : ORIGIN = 0x80000000, LENGTH = 8M
  /* RAM reserved for executable sections */
  ram_to_test(rwx) : ORIGIN = 0x80800000, LENGTH = 120M
}

ENTRY(_start)
SEARCH_DIR("/opt/riscv/riscv64-unknown-elf/lib");
SECTIONS
{
  /* variables accessd from code */
  PROVIDE (__stack_top = ORIGIN(ram_to_use) + LENGTH(ram_to_use));
  PROVIDE (__ram_origin = ORIGIN(ram_to_test));
  PROVIDE (__uart_base_addr = 0x10010000);
  PROVIDE (__pdma_base_addr = 0x3000000);

  .init   : { KEEP (*(SORT_NONE(.init)))}
  .text   : {*(.text)}
  .rodata : { *(.rodata) }

  /* Adjust the address for the data segment.  We want to adjust up to
     the same address within the page on the next page up.  */
  . = DATA_SEGMENT_ALIGN (CONSTANT (MAXPAGESIZE), CONSTANT (COMMONPAGESIZE));

  __DATA_BEGIN__ = .;
  .data   : { *(.data) }

  __bss_start = .;
  .bss    : { *(.bss) }
  __BSS_END__ = .;

  /* need for global pointer relaxations */
  __global_pointer$ = MAX(__DATA_BEGIN__ + 0x800, __BSS_END__ - 0x800);
    
  . = DATA_SEGMENT_END (.);

  /* DWARF 2 debug sections. */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line .debug_line.* .debug_line_end) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
}
