#ifndef BITMNP_H 
#define BITMNP_H
#include <eembc_adaptation.h>

/*  PLATFORM-SPECIFIC DEFINITIONS  */    

/* Compilation switches to be defined( or not )based on application follow */

/* Define( =1 )when compiling for 32-bit variables */    
/* ...or( =0 )when compiling for 16-bit variables */    
#define DATA_SIZE 1 

#if DATA_SIZE == 1  /* LONG variables */
typedef n_long varsize ;      /* Data and variables are 32 bits */
#else               /* Else, SHORT variables */
typedef n_short varsize ;     /* Data and variables are 16 bits */
#endif /* DATA_SIZE */

/* Define( =1 )to direct debug text to RAM file */    
/* ...or( =0 )to direct debug text to console */    
#define RAM_OUT 0        

/*******************************************************************************
    Defines                                                                     
*******************************************************************************/

#define false   0
#define true    !false

#if (BMDEBUG && RAM_OUT == 1)	/* Debug buffer size == 32K */ 
#define MAX_FILESIZE    8192    /* Maximum size of output file */
#else
#define MAX_FILESIZE    256     /* Maximum size of output file */
#endif 
#define NUM_TESTS    1    /* Number of sets of input test data stimuli */
#define VAR_COUNT    2      /* Number of variables which must be allocated */
#define HEADER_SIZE  100    /* Approximate size of title and header messages */

#define INPUT_CHARS  20     /* Number of characters to be displayed */
#define CHAR_COLUMNS 5      /* Columns-per-character */

/* Enough columns to display the characters */
#define MAX_COLUMNS  (INPUT_CHARS*CHAR_COLUMNS)

#define BITS_PER_BYTE   8   /* There's 8 bits in a byte */
#define BITS_PER_DIGIT  4   /* There's 4 bits in a BCD digit */
#define DIGIT_MASK      0xF /* Mask for one BCD digit */
#define BLANK           10  /* The 'BLANK' character */

/* Index of the 'BLANK' character's bitmap */
#define BLANK_CHAR   ((BLANK+1)*CHAR_COLUMNS)

#define ERROR_CHAR  0       /* Index of the 'ERROR' character's bitmap */

#define DOT_1       1
#define DOT_2       2
#define DOT_3       4
#define DOT_4       8
#define DOT_5       0x10
#define DOT_6       0x20
#define DOT_7       0x40
#define DOT_8       0x80
#define DOT_9       0x100
#define DOT_10      0x200
#define DOT_11      0x400
#define DOT_12      0x800
#define DOT_13      0x1000
#define DOT_14      0x2000
#define DOT_15      0x4000
#define DOT_16      0x8000
#define DOT_17      0x10000
#define DOT_18      0x20000
#define DOT_19      0x40000
#define DOT_20      0x80000
#define DOT_21      0x100000
#define DOT_22      0x200000
#define DOT_23      0x400000
#define DOT_24      0x800000
#define DOT_25      0x1000000
#define DOT_26      0x2000000
#define DOT_27      0x4000000
#define DOT_28      0x8000000
#define DOT_29      0x10000000
#define DOT_30      0x20000000
#define DOT_31      0x40000000
#define DOT_32      0x80000000

#define BIT_0       1
#define BIT_1       2
#define BIT_2       4
#define BIT_3       8
#define BIT_4       0x10
#define BIT_5       0x20
#define BIT_6       0x40
#define BIT_7       0x80
#define BIT_8       0x100
#define BIT_9       0x200
#define BIT_10      0x400
#define BIT_11      0x800
#define BIT_12      0x1000
#define BIT_13      0x2000
#define BIT_14      0x4000
#define BIT_15      0x8000
#define BIT_16      0x10000
#define BIT_17      0x20000
#define BIT_18      0x40000
#define BIT_19      0x80000
#define BIT_20      0x100000
#define BIT_21      0x200000
#define BIT_22      0x400000
#define BIT_23      0x800000
#define BIT_24      0x1000000
#define BIT_25      0x2000000
#define BIT_26      0x4000000
#define BIT_27      0x8000000
#define BIT_28      0x10000000
#define BIT_29      0x20000000
#define BIT_30      0x40000000
#define BIT_31      0x80000000

/*******************************************************************************
    Global Variables                                                            
*******************************************************************************/

#ifndef ALGO_GLOBALS    /* Don't define these twice! */

// extern n_int   tableCount ;     /* Number of passes through table */
// extern varsize *inpNumber ;     /* Pointer to array of numbers to paint */
// extern n_int   *inpMode ;       /* Pointer to array of painting modes */
// extern varsize inputNum ;       /* The input argument for computation */
// extern n_int   inverted ;       /* Paint 'inverted' character in display */

#endif /* ALGO_GLOBALS */    

/*******************************************************************************
    Function Prototypes                                                         
*******************************************************************************/


#endif