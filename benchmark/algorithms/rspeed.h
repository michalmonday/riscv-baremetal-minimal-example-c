#ifndef RSPEED_H 
#define RSPEED_H
#include <eembc_adaptation.h>


/* Compilation switches to be defined( or not )based on application follow */

/* Define (=1) when compiling for DOUBLE variables */
#define DATA_SIZE   1       
/* ..or( =0 )when compiling for FLOAT variables */
#if DATA_SIZE == 0           /* SHORT variables */
typedef n_short varsize ;    /* Data and variables are 32 bits */
#define MAX_VARIABLE 0x7FFF  /* Must match sim. real-time ctr in test data */
#else                        /* Else, LONG variables */
typedef n_long varsize ;     /* Data and variables are 16 bits */
#define MAX_VARIABLE 0x7FFF  /* Must match sim. real-time ctr in test data */
#endif /* DATA_SIZE */

#define RAM_OUT 0   /* Define (=1)to direct debug text to RAM file */
                    /* or( =0 )to direct debug text to console */

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
#define NUM_TESTS    500    /* Number of sets of input test data stimuli */
#define VAR_COUNT    1      /* Number of variables which must be allocated */
#define HEADER_SIZE  100    /* Approximate size of title and hdr messages */

/* Arbitrary scale factor for computing internal RPM */
#define SPEEDO_SCALE_FACTOR 36000 
/* Number of teeth on tonewheel */
#define NUM_TEETH           60    
/* Minimum inter-pulse period possible */
#define MIN_TOOTH_TIME      100   
/* Maximum inter-pulse period permitted( zero road-speed )*/
#define MAX_TOOTH_TIME      10000 

/*******************************************************************************
    Global Variables                                                            
*******************************************************************************/

#ifndef ALGO_GLOBALS

extern n_int   tableCount ;     /* Number of passes through table */
/* Current 'tonewheelCounter' pulled from test data */
extern varsize tonewheelCounter ;    
/* Array of 'tonewheelCounter' test data values */
extern varsize *inpTonewheelCount ;
/* Number of teeth on the tonewheel */    
extern varsize tonewheelTeeth ;    

#endif /* ALGO_GLOBALS */    

#endif