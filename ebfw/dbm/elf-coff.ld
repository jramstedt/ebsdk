OUTPUT_FORMAT("elf64-alpha", "elf64-alpha", "elf64-alpha")
OUTPUT_ARCH(alpha)
ENTRY(__start)
STARTUP(dbmentry.o)
SEARCH_DIR("=/usr/local/lib/alpha-linux-gnu"); SEARCH_DIR("=/lib/alpha-linux-gnu"); SEARCH_DIR("=/usr/lib/alpha-linux-gnu"); SEARCH_DIR("=/usr/local/lib"); SEARCH_DIR("=/lib"); SEARCH_DIR("=/usr/lib"); SEARCH_DIR("=/usr/alpha-linux-gnu/lib");

SECTIONS
{
  PROVIDE (__executable_start = SEGMENT_START("text-segment", 0x10000));
  . = SEGMENT_START("text-segment", 0x10000);

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
  .rodata         : {
    *(.rodata .rodata.*)
    *(.rdata)
    *(.rconst)
    PROVIDE(_fpdata = .);
    *(.pdata)
  }
  .eh_frame       : ONLY_IF_RO { KEEP (*(.eh_frame)) *(.xdata) }
  /* Start of the Read Write Data region.  */
  /* Exception handling.  */
  .eh_frame       : ONLY_IF_RW { KEEP (*(.eh_frame)) *(.eh_frame.*) *(.xdata) }
  .data.rel.ro    : { *(.data.rel.ro.local*) *(.data.rel.ro) }
  .data           :
  {
    PROVIDE(_fdata = .);
    *(.data .data.*)
    *(.xdata)
  }
  .got            : { *(.got.plt) *(.igot.plt) *(.got) *(.igot) }
  HIDDEN (_gp = ALIGN(16) + 0x7ff0);
  /* We want the small data sections together, so single-instruction offsets
     can access them all, and initialized data all before uninitialized, so
     we can shorten the on-disk segment size.  */
  .sdata          :
  {
    *(.lit4)
    *(.lit8)
    *(.lita)
    *(.sdata)
  }
  _edata = .;
  PROVIDE (edata = .);
  . = ALIGN(ALIGNOF(NEXT_SECTION));
  __bss_start = .;
  .sbss           :
  {
    *(.sbss)
    *(.scommon)
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
  /* Start of the Tiny Data region.  */
  /* Stabs debugging sections.  */
  .stab          0 : { *(.stab) }
  .stabstr       0 : { *(.stabstr) }
  .stab.excl     0 : { *(.stab.excl) }
  .stab.exclstr  0 : { *(.stab.exclstr) }
  .stab.index    0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment 0 (INFO) : { *(.comment); LINKER_VERSION; }
  .gnu.build.attributes : { *(.gnu.build.attributes .gnu.build.attributes.*) }
  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
  /* DWARF 1.  */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  /* GNU DWARF 1 extensions.  */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2.  */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2.  */
  .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line .debug_line.* .debug_line_end) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  /* SGI/MIPS DWARF 2 extensions.  */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }
  /* DWARF 3.  */
  .debug_pubtypes 0 : { *(.debug_pubtypes) }
  .debug_ranges   0 : { *(.debug_ranges) }
  /* DWARF 5.  */
  .debug_addr     0 : { *(.debug_addr) }
  .debug_line_str 0 : { *(.debug_line_str) }
  .debug_loclists 0 : { *(.debug_loclists) }
  .debug_macro    0 : { *(.debug_macro) }
  .debug_names    0 : { *(.debug_names) }
  .debug_rnglists 0 : { *(.debug_rnglists) }
  .debug_str_offsets 0 : { *(.debug_str_offsets) }
  .debug_sup      0 : { *(.debug_sup) }
  .gnu.attributes 0 : { KEEP (*(.gnu.attributes)) }
  /DISCARD/ : { *(.note.GNU-stack) *(.gnu_debuglink) }
}

PHDRS
{

}
