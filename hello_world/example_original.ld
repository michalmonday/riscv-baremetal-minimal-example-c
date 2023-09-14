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
  /* STACK_TOP = 0x8003fffc; */
  STACK_TOP = 0x8003fc00;
  /* STACK_TOP =    0x80030000; */
  
  /* start addresss */
  . = START_ADDRESS;

  /* text */
  .text : { 
    *(.text) 
    *(.text*)
    *(.rodata)
    *(.rodata*)
  } 
/* 
    .fini : {
        KEEP (*(SORT_NONE(.fini)))
    } 

    .preinit_array : ALIGN(8) {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    } 

    .init_array : ALIGN(8) {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
        KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
        PROVIDE_HIDDEN (__init_array_end = .);
        PROVIDE_HIDDEN ( metal_constructors_start = .);
        KEEP (*(SORT_BY_INIT_PRIORITY(.metal.init_array.*)));
        KEEP (*(.metal.init_array));
        PROVIDE_HIDDEN ( metal_constructors_end = .);
    } 

    .fini_array : ALIGN(8) {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
        KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
        PROVIDE_HIDDEN (__fini_array_end = .);
        PROVIDE_HIDDEN ( metal_destructors_start = .);
        KEEP (*(SORT_BY_INIT_PRIORITY(.metal.fini_array.*)));
        KEEP (*(.metal.fini_array));
        PROVIDE_HIDDEN ( metal_destructors_end = .);
    } 

 

    .ctors : {
        KEEP (*crtbegin.o(.ctors))
        KEEP (*crtbegin?.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))
        KEEP (*(.metal.ctors .metal.ctors.*))
    } 

    .dtors : {
        KEEP (*crtbegin.o(.dtors))
        KEEP (*crtbegin?.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
        KEEP (*(.metal.dtors .metal.dtors.*))
    }  */


  
  /* data */
  .gnu_build_id : { *(.note.gnu.build-id) }
  .data : ALIGN(8) { 
    *(.data) 
    *(.data*)
    *(.gnu.linkonce.d.*)
    . = ALIGN(8);
    PROVIDE( __global_pointer$ = . + 0x800 );
    *(.sdata .sdata.* .sdata2.*)
    *(.gnu.linkonce.s.*)
  }

  .sbss : ALIGN(8) {
    SBSS_START = .;
     *(.sbss) 
     *(.sbss*)
     *(.scommon)
    . = ALIGN(8);
    SBSS_END = .;
  } /* static local variables */
  .bss : ALIGN(8) { 
    BSS_START = .;
    *(.bss) 
    *(.bss*)
    *(COMMON)
    . = ALIGN(8);
    BSS_END = .;
  } /* global variables */

  /* .sdata : ALIGN(8) { 
    *(.sdata) 
    *(.sdata*)
  } */

/* from https://github.com/five-embeddev/riscv-scratchpad/blob/master/baremetal-startup-c/src/linker.lds 
//   .tdata : ALIGN(8) {
//         PROVIDE( __tls_base = . );
// 	*(.tdata .tdata.* .gnu.linkonce.td.*)
//     } 
//     PROVIDE( __tdata_source = LOADADDR(.tdata) );
//     PROVIDE( __tdata_size = SIZEOF(.tdata) );
//     PROVIDE( metal_segment_data_source_start = LOADADDR(.data) );
//     PROVIDE( metal_segment_data_target_start = ADDR(.data) );
//     PROVIDE( metal_segment_data_target_end = ADDR(.tdata) + SIZEOF(.tdata) );
//     .tbss : ALIGN(8) {
// 	*(.tbss .tbss.* .gnu.linkonce.tb.*)
// 	*(.tcommon .tcommon.*)
// 	PROVIDE( __tls_end = . );
//     }
//     PROVIDE( __tbss_size = SIZEOF(.tbss) );
//     PROVIDE( __tls_size = __tls_end - __tls_base );
//   .tbss_space : ALIGN(8) {
// 	. = . + __tbss_size;
//   } 
//   PROVIDE( metal_segment_bss_source_start = LOADADDR(.tbss) );
//   PROVIDE( metal_segment_bss_target_start = ADDR(.tbss) );
//   PROVIDE( metal_segment_bss_target_end = ADDR(.bss) + SIZEOF(.bss) );
// 
*/
  .debug : ALIGN(8) { 
    *(.debug)
  }

  HEAP_START = ALIGN(8);

  /* .heap : ALIGN(8)
  {
    PROVIDE(__heap_start = .);
    . = __neorv32_heap_size;
    . = ALIGN(8);
    PROVIDE(__heap_end = .);
  } > ram */

  /* stack */
  /* stack_bottom = .;
  . += STACK_SIZE;
  stack_top = .; */

  
  _end = .;
}
