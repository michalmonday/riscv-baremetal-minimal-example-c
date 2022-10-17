OUTPUT_ARCH( "riscv" )
ENTRY( _start )
MEMORY
{
rom (rx) : ORIGIN = 0x80000000, LENGTH = 17K
rom2 (rx) : ORIGIN = 0x80011000, LENGTH = 16K
}

SECTIONS
{
  START_ADDRESS =  0x80002000; 
  /* START_ADDRESS =  0x7fffffa0; */
  STACK_SIZE = 0x1000;
  
  /* start addresss */
  . = START_ADDRESS;

  /* text */
  /* .text : { *(.text) } */
  /* .text : { ORIGIN=0, LENGTH=0x17B } */
  .text : { *(.text) } >rom
  
  /* data */
  .gnu_build_id : { *(.note.gnu.build-id) } >rom2
  .data : { *(.data) } >rom2
  .rodata : { *(.rodata) } >rom2
  .sdata : { *(.sdata) } >rom2
  .debug : { *(.debug) } >rom2
  .rodata.str1.1 : { *(.rodata.str1.1) } >rom2
  __cap_relocs : { *(__cap_relocs) } >rom2
  .captable : { *(.captable) } >rom2

  /* stack */
  stack_bottom = .;
  . += STACK_SIZE;
  stack_top = .;

  _end = .;
}
