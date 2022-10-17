OUTPUT_ARCH( "riscv" )
ENTRY( _start )
SECTIONS
{
  START_ADDRESS = 0x70000000;
  STACK_SIZE = 0x800;
  
  /* start addresss */
  . = START_ADDRESS;

  /* text */
  .text : { *(.text) }
  
  /* data */
  .gnu_build_id : { *(.note.gnu.build-id) }
  .data : { *(.data) }
  .rodata : { *(.rodata) }
  .sdata : { *(.sdata) }
  .debug : { *(.debug) }

  /* stack */
  stack_bottom = 0x80000000;
  . += STACK_SIZE;
  stack_top = 0x80000800;

  _end = stack_top;
}
