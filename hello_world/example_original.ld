OUTPUT_ARCH( "riscv" )
ENTRY( _start )
SECTIONS
{
  START_ADDRESS = 0x80000000;
  /* STACK_SIZE = 0x800; */
  /* ZC706 has 256KB bram, ending at 0x80040000 

    Disassembly of section .text:
    0000000080000000 <_start>:
        80000000:	f14022f3          	csrr	t0,mhartid
        80000004:	02029063          	bnez	t0,80000024 <park>
        80000008:	00040117          	auipc	sp,0x40
        8000000c:	73410113          	addi	sp,sp,1844 # 8004073c <_end>
  */
  /* STACK_SIZE = 0x10000; */
  STACK_TOP = 0x8003fffc;
  
  /* start addresss */
  . = START_ADDRESS;

  /* text */
  .text : { *(.text) } 
  
  /* data */
  .gnu_build_id : { *(.note.gnu.build-id) }
  .data : ALIGN(4) { *(.data) }
  .rodata : ALIGN(4) { *(.rodata) }
  .sbss : ALIGN(4) { *(.sbss) } /* static local variables */
  .bss : ALIGN(4) { *(.bss) } /* global variables */
  .sdata : ALIGN(4) { *(.sdata) }
  .debug : ALIGN(4) { *(.debug) }

  HEAP_START = ALIGN(4);

  /* .heap : ALIGN(4)
  {
    PROVIDE(__heap_start = .);
    . = __neorv32_heap_size;
    . = ALIGN(4);
    PROVIDE(__heap_end = .);
  } > ram */

  /* stack */
  /* stack_bottom = .;
  . += STACK_SIZE;
  stack_top = .; */

  
  _end = .;
}
