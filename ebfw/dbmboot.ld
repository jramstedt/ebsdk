OUTPUT_FORMAT("elf64-alpha", "elf64-alpha", "elf64-alpha")
OUTPUT_ARCH(alpha)
ENTRY(__start)
SEARCH_DIR("=/usr/local/lib/alpha-linux-gnu"); SEARCH_DIR("=/lib/alpha-linux-gnu"); SEARCH_DIR("=/usr/lib/alpha-linux-gnu"); SEARCH_DIR("=/usr/local/lib"); SEARCH_DIR("=/lib"); SEARCH_DIR("=/usr/lib"); SEARCH_DIR("=/usr/alpha-linux-gnu/lib");

SECTIONS
{
  PROVIDE (__executable_start = SEGMENT_START("text-segment", 0x300000));
  . = SEGMENT_START("text-segment", 0x300000);

  /* Start of the executable code region.  */
  .init           :
  {
    KEEP (*(SORT_NONE(.init)))
  } =0x0000fe2f1f04ff47
  .text           :
  {
    *(.text.startup .text.startup.*)
    *(.text)
  } =0x0000fe2f1f04ff47
  .fini           :
  {
    KEEP (*(SORT_NONE(.fini)))
  } =0x0000fe2f1f04ff47
  PROVIDE (__etext = .);
  PROVIDE (_etext = .);
  PROVIDE (etext = .);
  /* Start of the Read Only Data region.  */
  .rodata         : { *(.rodata .rodata.*) }
  .eh_frame       : ONLY_IF_RO { KEEP (*(.eh_frame)) }
  /* Start of the Read Write Data region.  */
  /* Exception handling.  */
  .eh_frame       : ONLY_IF_RW { KEEP (*(.eh_frame)) *(.eh_frame.*) }
  .data           :
  {
    *(.data)
  }
  .got            : { *(.got.plt) *(.igot.plt) *(.got) *(.igot) }
  /* We want the small data sections together, so single-instruction offsets
     can access them all, and initialized data all before uninitialized, so
     we can shorten the on-disk segment size.  */
  .sdata          :
  {
    *(.sdata)
  }
  _edata = .;
  PROVIDE (edata = .);
  . = ALIGN(ALIGNOF(NEXT_SECTION));
  __bss_start = .;
  .sbss           :
  {
    *(.sbss)
  }
  .bss            :
  {
    *(.bss)
    *(COMMON)
  }
  . = ALIGN(64 / 8);
  /* Start of the Large Data region.  */
  . = SEGMENT_START("ldata-segment", .);
  . = ALIGN(64 / 8);
  _end = .;
  PROVIDE (end = .);
  .comment 0 (INFO) : { *(.comment); LINKER_VERSION; }
  /DISCARD/ : { *(.note.GNU-stack) *(.gnu_debuglink) }
}

PHDRS
{

}