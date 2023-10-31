#ifndef FILE_H // TODO: change the name of the define to match the filename
#define FILE_H
#include <eembc_adaptation.h>

#include <math.h>

/* PLATFORM-SPECIFIC DEFINITIONS  */    

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
#define NUM_TESTS    4096  /* Number of sets of input test data stimuli */
#define VAR_COUNT    3     /* Number of variables which must be allocated */
#define HEADER_SIZE  100   /* Approximate size of title and header messages */

// #define NUM_POINTS   512   /* Number of points in FFT */
#define NUM_POINTS   3   /* Number of points in FFT */
#define FFT_LENGTH   9     /* length = log2( NUM_POINTS )*/

#ifdef PI
#undef PI
#endif
#define PI 3.141592654 

/* TRIG_SCALE_FACTOR is used for greatest twiddle factor precision */    
#define TRIG_SCALE_FACTOR       32768

/* BUTTERFLY_SCALE_FACTOR = log2( TRIG_SCALE_FACTOR )is used to
accomodate accumulator size limit of 32 bits */    
#define BUTTERFLY_SCALE_FACTOR  15          /* Divide by 32768 */

/* STAGE_SCALE_FACTOR is used to prevent overflow from
accumulation at each processing stage */    
#define STAGE_SCALE_FACTOR      1           /* Divide by 2 */

/*******************************************************************************
    Global Variables                                                            
*******************************************************************************/

#ifndef ALGO_GLOBALS         /* Don't define these twice! */

extern n_int   tableCount ;  /* Number of passes through table */
extern varsize *inpSignal ;  /* Pointer to array of input signal data values */

#endif /* ALGO_GLOBALS */    


#endif