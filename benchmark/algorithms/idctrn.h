#ifndef IDCTRN_H 
#define IDCTRN_H
#include <eembc_adaptation.h>

#include <math.h>

/*  PLATFORM-SPECIFIC DEFINITIONS  */    

/* Compilation switches to be defined( or not )based on application follow */

/* Define( =1 )when compiling for 32-bit variables */    
/* ...or( =0 )when compiling for 16-bit variables */    
#define DATA_SIZE 1 

#if DATA_SIZE == 1          /* LONG variables */
typedef n_long varsize ;      /* Data and variables are 32 bits */
#else                       /* Else, SHORT variables */
typedef n_short varsize ;     /* Data and variables are 16 bits */
#endif /* DATA_SIZE */

/* Define( =1 )to direct debug text to RAM file */    
/* ...or( =0 )to direct debug text to console */    
#define RAM_OUT 0        

/*******************************************************************************
    Defines                                                                     
*******************************************************************************/

#define false    0
#define true    !false

#if (BMDEBUG && RAM_OUT == 1)	/* Debug buffer size == 32K */ 
#define MAX_FILESIZE    8192    /* Maximum size of output file */
#else
#define MAX_FILESIZE    256     /* Maximum size of output file */
#endif 
#define NUM_TESTS    8192  /* Number of chars of input test data stimuli */
#define VAR_COUNT    2     /* Number of variables which must be allocated */
#define HEADER_SIZE  100   /* Approx size of text title and header messages */

#define ROWS         8     /* Perform 8x8 block compression */
#define COLS         8

/* Integer table of cosines is scaled from float by this */
#define COS_SCALE_FACTOR    4096.0  
/* Scaling expressed as exponent 2**COS_SCALE_FACTOR */
#define COS_SCALE_EXP       12

/*******************************************************************************
    Global Variables                                                            
*******************************************************************************/

#ifndef ALGO_GLOBALS

extern n_int tableCount ;     /* Number of passes through table */

/* Pointer to array of input chars */
extern n_uchar *inpString ;
/* 16 byte string representing one 8x8 input block */
extern n_uchar inChar[] ;   

#endif /* ALGO_GLOBALS */    


#endif